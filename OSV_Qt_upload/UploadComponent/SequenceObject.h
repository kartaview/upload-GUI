#ifndef SequenceObject_H
#define SequenceObject_H

#include <QObject>

class SequenceObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString seqName READ seqName WRITE setSeqName NOTIFY seqNameChanged)
    Q_PROPERTY(QString seqSize READ seqSize WRITE setSeqSize NOTIFY seqSizeChanged)
    Q_PROPERTY(QString seqNoFiles READ seqNoFiles WRITE setSeqNoFiles NOTIFY seqNoFilesChanged)

public:

    explicit SequenceObject();
    SequenceObject(const QString &seqName, const QString &seqSize, const QString &seqNoFiles);

    QString seqName() const;
    QString seqSize() const;
    QString seqNoFiles() const;

    void setSeqName(QString &seqName);
    void setSeqSize(QString &seqSize);
    void setSeqNoFiles(QString &seqNoFiles);

signals:
    void seqNameChanged(QString);
    void seqSizeChanged(QString);
    void seqNoFilesChanged(QString);


private:
    QString m_seqName;
    QString m_seqSize;
    QString m_seqNoFiles;
};

#endif // SequenceObject_H
