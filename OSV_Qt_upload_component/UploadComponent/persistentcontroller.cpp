#include "persistentcontroller.h"
#include <QFile>
#include <QDir>
#include <QtCore/QVariant>
#include <QCoreApplication>
#include "uploadcomponentconstants.h"
#include <QtAlgorithms>

// HACK - made to put the directories in the same place for 2 different OS
inline QDir getCurrentFolder()
{
    QDir current(QCoreApplication::applicationDirPath());
#ifdef Q_OS_MACX
    current.cdUp();
    current.cdUp();
#endif

    return current;
}

PersistentController::PersistentController(QObject *parent)
    : QObject(parent),
      m_saveFilePath(getCurrentFolder().path()+"/save.json"), //path declared as const member to make sure that QApplication object is initialized
      m_sequences(new QQmlObjectListModel<PersistentSequence>(this))
{
    reset();
}

void PersistentController::reset()
{
    set_totalFiles(0);
    set_totalSize(0);
    m_sequences->clear();
    m_persistentSequences.clear();
    m_enteredDirPath.clear();

    load();
    int seqIndex = 0;
    foreach(PersistentSequence * s, m_persistentSequences)
    {
        if(s->get_status() != SequenceStatus::SUCCESS)
        {
            m_enteredDirPath.append(s->get_path());
            checkPaths(seqIndex);
            if(s->get_size())
            {
                m_sequences->append(s);
            }
        }
        ++seqIndex;
    }
}

QString PersistentController::convertFolderPath(const QString& folderPath)
{
    if(folderPath.endsWith("/")) // adding files by drag&drop puts a "/" at the end
    {
        QString fpath = folderPath;
        fpath.chop(1);
        return fpath;
    }
    return folderPath;
}

/*
 * Checks if the path of the directory already exists inside the area.
 * It is called through a signal from entering the area of the drag and drop region.
 * @param pathReceived represents a qml url path and used to get the subfiles of it.
 */
void PersistentController::addPreviewPath(const QVariant &pathReceived)
{
    const QString pathString(QUrl(pathReceived.toString()).toLocalFile());
    const QFileInfo pathInfo(pathString);

    if(pathInfo.isDir())
    {
        m_enteredDirPath.push_back(convertFolderPath(pathInfo.absoluteFilePath()));
    }
}

void PersistentController::onDropped()
{
    foreach(PersistentSequence * s, m_persistentSequences)
    {
        if(m_enteredDirPath.contains(s->get_path()))
            m_enteredDirPath.removeOne(s->get_path());
    }
    checkPaths();
}

bool PersistentController::checkMetadata(const QString &path, PersistentSequence * sequence, qint64 &totalSize)
{
    QDirIterator itMetaData(path, QStringList() << "track.txt.gz"<<"track.txt", QDir::Files);

    while(itMetaData.hasNext())
    {
        itMetaData.next();
        if(itMetaData.fileInfo().size() < kGigaByte && itMetaData.fileInfo().suffix() == "gz")
        {
            sequence->set_metadata(new Metadata(itMetaData.filePath(), sequence));
            totalSize += sequence->get_metadata()->getSize();
            return true;
        }
    }
    sequence->set_metadata(new Metadata("", sequence));
    return false;

}

void PersistentController::checkPhotosExif(const QString &path, PersistentSequence * sequence, qint64 &totalSize)
{
    QDirIterator itPhotoFile(path, QStringList() << "*.jpg" << "*.jpeg", QDir::Files);

    QList<Photo *> photos;
    while(itPhotoFile.hasNext())
    {
        itPhotoFile.next();
        Photo *photo = new Photo(sequence);

        if(photo->processPhoto(itPhotoFile.filePath()))
        {
            photos.append(photo);
            totalSize += photo->getSize();
        }
    }

    if(photos.size())
    {
        sequence->set_photos(photos);
        sequence->addPhotoInfo(path, totalSize);
        addPersistentObject(sequence);
    }

}

void PersistentController::checkVideos(const QString &path, PersistentSequence * sequence, qint64 &totalSize)
{
    QDirIterator itVideoFile(path, QStringList()<<"*.mp4", QDir::Files);

    QList<Video *> videos;
    long long videoSize;

    while(itVideoFile.hasNext())
    {
        itVideoFile.next();
        videoSize = itVideoFile.fileInfo().size();
        if(videoSize > 0 && videoSize < kGigaByte)
        {
            Video *video = new Video(itVideoFile.filePath(), sequence);
            videos.append(video);
            totalSize += videoSize;
        }
    }

    qSort(videos.begin(), videos.end(), Video::lessThan);

    if(videos.size()) // exists valid videos
    {
        qSort(videos.begin(), videos.end(), Video::lessThan);
        sequence->set_videos(videos);
        sequence->addVideoInfo(path, totalSize);
        addPersistentObject(sequence);
    }
}

/*
 * onDroppedArea validates the files inside the folders dropped.
 * It is called through a signal from dropping inside the area of the drag and drop region.
 * The folders that passes the test are then added to the model list with some of their information: folder name, size, files type
 * This function is declared as a public slot.
 */
void PersistentController::checkPaths(const int index)
{
    int i;
    QList<QString> queue;
    for(i=0;i<m_enteredDirPath.size();i++)
    {
        queue.push_back(m_enteredDirPath.at(i));
        while(!queue.empty())
        {
            QString front = queue.takeFirst();

            PersistentSequence *validSequence(nullptr);
            if(index == -1)
                validSequence = new PersistentSequence(this);
            else
                validSequence = m_persistentSequences.at(index);

            qint64 totalSize=0;

            if(!checkMetadata(front, validSequence, totalSize))
            {
                checkPhotosExif(front, validSequence, totalSize);
            }
            else
            {
                checkVideos(front, validSequence, totalSize);
            }

            QDirIterator itDir(front, QDir::Dirs | QDir::NoDotAndDotDot);
            while (itDir.hasNext()) // search for sub directories
            {
                itDir.next();
                if(!folderExist(itDir.filePath()))
                {
                    queue.push_back(itDir.filePath());
                }
            }
        }
    }

    calculateTotalInformation();
    emit informationChanged();

    m_enteredDirPath.clear();
}

/*
 * This function just clears the entered folders if they are not dropped.
 */
void PersistentController::onExitedDropArea()
{
    m_enteredDirPath.clear();
}

/*
 * folderExist verifies if he given path already exist in the drag and drop list
 * @param the filepath which will be verified
 * @return true if the path exists in the drag and drop list, and false if it does not.
 */
bool PersistentController::folderExist(const QString &filepath)
{
    foreach(PersistentSequence * s, m_persistentSequences)
    {
        if(s->get_path() == filepath)
        {
            return true;
        }
    }
    return false;
}

void PersistentController::calculateTotalInformation()
{
    qint64 sum = 0;
    int noFiles = 0;
    foreach(PersistentSequence* s, m_persistentSequences)
    {
        if(s->get_status() != SequenceStatus::SUCCESS && s->get_size())
        {
            sum += s->get_size();
            noFiles += s->get_sequence()->get_photo_no();
        }
    }
    set_totalFiles(noFiles);
    set_totalSize(sum);
}

void PersistentController::onFileDialogButton(const QVariant &pathReceived)
{
    addPreviewPath(pathReceived);
    onDropped();
}

void PersistentController::removeFolders(const QList<QVariant> indexes)
{
    for(int index = indexes.count()-1 ; index >= 0 ; --index)
    {
        int indexToRemove = indexes[index].toInt();
        m_persistentSequences.removeOne(m_sequences->at(indexToRemove));
        m_sequences->remove(indexToRemove);
    }
    save();
    calculateTotalInformation();
    emit informationChanged();
}



//add persistent object
void PersistentController::addPersistentObject(PersistentSequence *sequence)
{
    if(!m_persistentSequences.contains(sequence))
    {
        m_persistentSequences.append(sequence);
        m_sequences->append(sequence);
        save();
    }

}

//update information of an already existing object
void PersistentController::updatePersistentObject(PersistentSequence *sequence)
{
    if(m_persistentSequences.contains(sequence))
    {
        int index = findIndex(sequence);
        m_persistentSequences.replace(index, sequence);
        save();
    }
}

PersistentSequence* PersistentController::getElement(const int index)
{
    if(index<m_persistentSequences.size())
        return m_persistentSequences.at(index);
    return nullptr;
}

QList<PersistentSequence *> PersistentController::getPersistentSequences()
{
    return m_persistentSequences;
}

//return index if the sequence is inside , else return -1
int PersistentController::findIndex(PersistentSequence *sequence)
{
    int i=0;
    foreach(PersistentSequence * s, m_persistentSequences)
    {
        if(s->equals(sequence))
            return i;
        i++;
    }
    return -1;
}


//write all seq in a json object
void PersistentController::write(QJsonObject &json)
{
    QJsonArray sequenceArray;
    for(int i = 0; i<m_persistentSequences.count(); i++)
    {
        QJsonObject sequenceObject;
        m_persistentSequences.at(i)->write(sequenceObject);
        sequenceArray.append(sequenceObject);
    }
    json["sequences"] = sequenceArray;
}

// save all sequences in a folder
bool PersistentController::save()
{
    QFile saveFile(m_saveFilePath);
    saveFile.remove();
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        qDebug()<<"Can not open!";
        return false;
    }

    QJsonObject progressObject;
    write(progressObject);

    QJsonDocument saveDoc(progressObject);
    saveFile.write(saveDoc.toJson());
    saveFile.close();
    qDebug()<<"Saved!";
    return true;
}

void PersistentController::read(const QJsonObject &json)
{
    QJsonArray sequenceArray = json["sequences"].toArray();
    for (int seqIndex = 0; seqIndex < sequenceArray.size(); ++seqIndex)
    {
        QJsonObject sequenceObject = sequenceArray[seqIndex].toObject();
        PersistentSequence *s = new PersistentSequence(this);

        s->read(sequenceObject);

        if(QFileInfo(s->get_path()).exists())
        {
            m_persistentSequences.append(s);
        }

    }
}

bool PersistentController::load()
{
    QFile loadFile(m_saveFilePath);

    if(!loadFile.open(QIODevice::ReadOnly))
    {
        qDebug()<<"Can not open!";
        return false;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    m_persistentSequences.clear();
    read(loadDoc.object());

    qDebug()<<"Loaded! "<<QFileInfo(loadFile).absoluteFilePath();
    loadFile.close();
    return true;
}

void PersistentController::resetStatusForUnsentSequenceFiles()
{
    foreach(PersistentSequence * s, m_persistentSequences)
    {
        s->resetStatusForUnsentFiles();
    }
}

void PersistentController::resetProperties()
{
    set_totalFiles(0);
    set_totalSize(0);
    m_sequences->clear();

    foreach(PersistentSequence * s, m_persistentSequences)
    {
        if(s->get_status() == SequenceStatus::SUCCESS)
        {
            s->resetInformation();
        }
    }
}
