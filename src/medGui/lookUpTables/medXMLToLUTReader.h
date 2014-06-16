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
#include <qlist.h>


class medXMLToLUTReaderPrivate;

class medXMLToLUTReader {
public:
    medXMLToLUTReader(QList<medClutEditorTable*> * tables);
    ~medXMLToLUTReader();
    bool read(QIODevice *device);
    QString errorString() const;
private:
    medXMLToLUTReaderPrivate * d;
};



