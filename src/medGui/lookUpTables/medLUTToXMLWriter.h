/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medClutEditor.h>
#include <qiodevice.h>
#include <qstring.h>

class medLUTToXMLWriterPrivate;

class medLUTToXMLWriter {
public:
    medLUTToXMLWriter(const QList<medClutEditorTable*> & tables);
    ~medLUTToXMLWriter();
    bool writeFile(QIODevice *device);
    QString errorString() const;
private:
    medLUTToXMLWriterPrivate * d;
};




