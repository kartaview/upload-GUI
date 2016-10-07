#ifndef PHOTO_H
#define PHOTO_H
#include <QObject>
#include <uploadcomponentconstants.h>

class Photo : public QObject
{
    Q_OBJECT
public:
    explicit Photo(QObject* parent = 0);

    bool processPhoto(const QString& filePath);

    static bool lessThan(Photo* p1, Photo* p2);

    int        getId();
    QString    getPath();
    long long  getSize();
    double     getLat();
    double     getLng();
    FileStatus getStatus();

    void setStatus(const FileStatus& status);

private:
    QString    m_path;
    long long  m_size;
    double     m_lat;
    double     m_lng;
    FileStatus m_status;
};

#endif  // PHOTO_H
