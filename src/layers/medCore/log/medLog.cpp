/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2022. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medLog.h>
#include <medNewLogger.h>

medLog::medLog(eLogLevel level, medNewLogger * logger) : m_pLevel(nullptr)
{
    if (logger == nullptr)
    {
        m_logger = &medNewLogger::mainInstance();
    }
    else
    {
        m_logger = logger;
    }

    setDefaultLevel(level);

    m_logger->registerMedStream(*this);
}

medLog::~medLog()
{
    delete m_pLevel;
    //m_logger->unRegisterMedStream(*this);
}



medLogStream medLog::operator()()
{
    return medLogStream(this);
}

medLogStream medLog::operator()(streamProperty propoerty)
{
    medLogStream logStreamRes(this);
    
    m_logPropMap[&logStreamRes] = propoerty;
    
    return logStreamRes;
}


void medLog::write(medLogStream *logStream, QString data)
{
    if (m_logPropMap.contains(logStream))
    {
        streamProperty prop = m_logPropMap.take(logStream);
        m_logger->writeMsgfromMedStream(this, data, &prop);
    }
    else
    {
        m_logger->writeMsgfromMedStream(this, data);
    }
}

void medLog::setDefaultLevel(eLogLevel level)
{
    if (level != eLogLevel::medNoneMsg)
    {
        m_pLevel = new eLogLevel(level);
    }
    else
    {
        delete m_pLevel;
        m_pLevel = nullptr;
    }
}
