#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractDataReader.h>
#include <medCoreExport.h>

class dtkAbstractData;
class medAbstractData;

/**
 * Extending dtkAbstractDataReader class
 */
class MEDCORE_EXPORT medAbstractDataReader : public dtkAbstractDataReader
{
    Q_OBJECT

public:
    using dtkAbstractDataReader::setData;
    virtual void setData(QList<medAbstractData*> data);
    virtual void setData(dtkAbstractData* data);
    QList<medAbstractData*> getDataList();

    virtual QString getVolumeId(const QString& path);
    virtual QString getVolumeName(const QString& path);

private:
    QList<medAbstractData*> dataList;
};
