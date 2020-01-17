/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkDataMeshPluginExport.h"
#include "vtkDataMeshReaderBase.h"

class vtkMetaDataSet;

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshReader: public vtkDataMeshReaderBase
{
    Q_OBJECT

public:
    vtkDataMeshReader();

    virtual QStringList handled() const;

    static QStringList s_handled();

public slots:

    virtual bool canRead(const QString& path);
    virtual bool readInformation(const QString& path);
    virtual bool read(const QString& path);

    virtual QString identifier()  const;
    virtual QString description() const;

    static bool registered();

private:

    static const char ID[];
    bool extractMetaData(QString path, vtkMetaDataSet* dataSet);
    bool extractMetaDataFromHeader(QString path, vtkMetaDataSet* dataSet);
    bool extractCartoMetaData(vtkMetaDataSet* dataSet);
};

dtkAbstractDataReader *createVtkDataMeshReader();
