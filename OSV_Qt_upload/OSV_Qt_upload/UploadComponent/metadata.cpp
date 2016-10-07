#include "metadata.h"
#include <QFileInfo>
#include <GZIP.h>

Metadata::Metadata(const QString &filepath, QObject *parent)
    :   QObject(parent),
        m_path(filepath),
        m_size(QFileInfo(filepath).size())
{
    if(!filepath.isEmpty())
    {
        m_decompressPath = filepath;
        m_decompressPath.chop(3);
        GZIP::decompress(filepath, m_decompressPath);
    }
}

void Metadata::processVideoMetadata(double &lat, double &lng)
{
    QFile metafile(m_decompressPath);
    if(metafile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        const QString firstLineString(metafile.readLine());
        const QStringList firstLineFields(firstLineString.split(";"));

        if(!firstLineFields.count() || firstLineFields.count() < 2)
        {
            return;
        }
        else
        {
            m_platformName    = firstLineFields[0];
            m_platformVersion = firstLineFields[1];
        }

        bool foundCoordinates(false);
        while (!metafile.atEnd() && !foundCoordinates)
        {
            const QString lineString(metafile.readLine());
            const QStringList fields(lineString.split(";"));

            // first lat & lng found for the request of a new sequence
            if(fields.count() > 14)
            {
                if(!fields[1].isEmpty())
                {
                    lng = fields[1].toDouble();
                }

                if(!fields[2].isEmpty())
                {
                    lat = fields[2].toDouble();
                }

                if(lat && lng)
                {
                    foundCoordinates = true;
                }
            }
        }
    }
    metafile.close();
}

QString Metadata::getPath()
{
    return m_path;
}

QString Metadata::getDecompressPath()
{
    return m_decompressPath;
}

long long Metadata::getSize()
{
    return m_size;
}

QString Metadata::getPlatformName()
{
    return m_platformName;
}

QString Metadata::getPlatformVersion()
{
    return m_platformVersion;
}
