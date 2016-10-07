#include "SequenceObject.h"

SequenceObject::SequenceObject()
{

}

SequenceObject::SequenceObject(const QString &seqName, const QString &seqSize, const QString &seqNoFiles)
{
    m_seqName = seqName;
    m_seqSize = seqSize;
    m_seqNoFiles = seqNoFiles;
}

QString SequenceObject::seqName() const
{
    return m_seqName;
}

QString SequenceObject::seqSize() const
{
    return m_seqSize;
}

QString SequenceObject::seqNoFiles() const
{
    return m_seqNoFiles;
}

void SequenceObject::setSeqName(QString &seqName)
{
    m_seqName = seqName;
    emit seqNameChanged(seqName);
}

void SequenceObject::setSeqSize(QString &seqSize)
{
    m_seqSize = seqSize;
    emit seqSizeChanged(seqSize);
}

void SequenceObject::setSeqNoFiles(QString &seqNoFiles)
{
    m_seqNoFiles = seqNoFiles;
    emit seqSizeChanged(seqNoFiles);
}
