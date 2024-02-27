#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2022. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCoreExport.h>
#include <medLogStream.h>
#include <medNewLogger.h>
#include <QMap>

class medNewLogger;
enum eLogLevel : int;
struct streamProperty;

class MEDCORE_EXPORT medLog
{
public:
    medLog(eLogLevel level = static_cast<eLogLevel>(-1), medNewLogger * logger = nullptr);
    ~medLog();

    medLogStream operator()();
    medLogStream operator()(streamProperty property);

    void write(medLogStream *logStream, QString data);
    void setDefaultLevel(eLogLevel pLevel);

private:
    friend class medNewLogger;

private:
    QMap<medLogStream*, streamProperty> m_logPropMap;
    eLogLevel * m_pLevel;
    medNewLogger * m_logger;
};
