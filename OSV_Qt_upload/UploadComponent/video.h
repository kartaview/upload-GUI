#ifndef VIDEO_H
#define VIDEO_H

#include <QObject>
#include <uploadcomponentconstants.h>

class Video : public QObject
{
    Q_OBJECT
public:
    explicit Video(const QString& filePath, QObject* parent = 0);
    bool operator<(const Video& v) const;
    static bool lessThan(Video* v1, Video* v2);

    // Getters
    int        getId() const;
    QString    getPath() const;
    long long  getSize() const;
    FileStatus getStatus() const;

    // Setters
    void setPath(const QString& path);
    void setSize(const long long& size);
    void setStatus(const FileStatus status);

signals:

public slots:

private:
    QString    m_path;
    long long  m_size;
    FileStatus m_status;
};

#endif  // VIDEO_H
