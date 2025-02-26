#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCoreExport.h>

#include <medClutEditor.h>
#include <qiodevice.h>
#include <qstring.h>
#include <qlist.h>

class medXMLToLUTReaderPrivate;

class MEDCORE_EXPORT medXMLToLUTReader
{
public:
    medXMLToLUTReader(QList<medClutEditorTable*> * tables, medClutEditorScene *scene);
    ~medXMLToLUTReader();
    bool read(QIODevice *device);
    QString errorString() const;

private:
    medXMLToLUTReaderPrivate * d;
};



