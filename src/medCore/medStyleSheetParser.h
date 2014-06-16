/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QRegExp>
#include <QVariant>
#include <medCoreExport.h>

class MEDCORE_EXPORT medStyleSheetParser
{
public:
    medStyleSheetParser(QString qss);
    
    QString result() const;

private:
    QString output;
};
