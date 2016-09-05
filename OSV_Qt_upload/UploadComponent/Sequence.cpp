#include "Sequence.h"

Sequence::Sequence(QObject *parent) : JsonSerializable(parent)
{

}

ImageStatus::Type Sequence::toImageStatus (const QJsonValue &value) {
    QString toQString = value.toString();

    if (QString::compare(toQString, "NEW", Qt::CaseInsensitive)) {
       return ImageStatus::Type::NEW;
    }
    if (QString::compare(toQString, "UPLOAD_FINISHED", Qt::CaseInsensitive)) {
       return ImageStatus::Type::UPLOAD_FINISHED;
    }
    if (QString::compare(toQString, "PROCESSING_FINISHED", Qt::CaseInsensitive)) {
       return ImageStatus::Type::PROCESSING_FINISHED;
    }
    if (QString::compare(toQString, "PROCESSING_FAILED", Qt::CaseInsensitive)) {
       return ImageStatus::Type::PROCESSING_FAILED;
    }
    return ImageStatus::Type::NEW;
}

