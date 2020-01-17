/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
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
    ~vtkDataMeshReader() override = default;

    QStringList handled() const override;
    static QStringList s_handled();

public slots:
    bool canRead(const QString& path) override;
    bool readInformation(const QString& path) override;
    bool read(const QString& path) override;

    QString identifier()  const override;
    QString description() const override;

    static bool registered();

private:
    static const char ID[];
    bool extractMetaData(QString path, vtkMetaDataSet *dataSet);
    bool extractMetaDataFromHeader(QString path, vtkMetaDataSet *dataSet);
    bool extractCartoMetaData(vtkMetaDataSet *dataSet);
};

dtkAbstractDataReader *createVtkDataMeshReader();
