#include "medLogStream.h"
#include "medLog.h"

medLogStream::medLogStream(medLog * log) : QDebug(&m_string), m_pLog(log) { }

medLogStream::~medLogStream()
{
    m_pLog->write(this, m_string);
}

