#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtCore>

#include <medCoreExport.h>
#include <medJobItemL.h>

class dtkAbstractData;
class medAbstractData;

class MEDCORE_EXPORT medDatabaseExporter : public medJobItemL
{
    Q_OBJECT

public:
    using dtkAbstractDataReader::setData;
    virtual void setData(QList<medAbstractData*> data);
    virtual void setData(dtkAbstractData* data);
    QList<medAbstractData*> getDataList();

private:
    QList<medAbstractData*> dataList;
};
