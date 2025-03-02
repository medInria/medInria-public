#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2022. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QDebug>
#include <QString>

#include <medCoreExport.h>


class medLog;

class MEDCORE_EXPORT medLogStream : public QDebug
{
public:
    medLogStream(medLog *log);
    ~medLogStream();

private:
    medLog * m_pLog;
    QString m_string;
};
