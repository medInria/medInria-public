#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QRegExp>
#include <QVariant>

#include <medCoreLegacyExport.h>

class MEDCORELEGACY_EXPORT medStyleSheetParser
{
public:
    medStyleSheetParser(QString qss);

    QString result() const;

private:
    QString output;
};
