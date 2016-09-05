#ifndef METADATAQTWRAPPER_H
#define METADATAQTWRAPPER_H

#include <QObject>
#include <qqmlhelpers.h>
#include "metadata.h"

class MetadataQTWrapper : public QObject
{
    Q_OBJECT
public:
    explicit MetadataQTWrapper(QObject *parent = 0);
    Metadata _metadata;
signals:

public slots:

private:

};

#endif // METADATAQTWRAPPER_H
