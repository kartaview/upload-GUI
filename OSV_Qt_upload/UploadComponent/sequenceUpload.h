#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <qqmlhelpers.h>
#include <QList>
#include <QVariantList>
#include <QObject>
#include <QString>

class SequenceUpload : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY (QString, path)
    QML_WRITABLE_PROPERTY (QString, name)
    QML_WRITABLE_PROPERTY (QString, meta_data_filename)
    QML_WRITABLE_PROPERTY (QString, meta_data_filepath)
    QML_WRITABLE_PROPERTY (QStringList, photos_filepath)
    QML_WRITABLE_PROPERTY (int , no_files)
    QML_WRITABLE_PROPERTY (long long, size)
    QML_WRITABLE_PROPERTY(QString, displaySize)

public:
    explicit SequenceUpload(QObject *parent = 0);

};

#endif // SEQUENCE_H
