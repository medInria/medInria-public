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

#include <vtkMetaDataSet.h>

class vtkDataManagerReader;

/**
    \class vtkDataMesh4DReader
    \brief This class implements the reading process of a sequence
    (series) of meshes in vtk format.

    It make extensive use of the vtkDataManager class and API (in
    vtkINRIA3D repository). The input file is home-made and xml-based,
    it derives from the API described in vtkDataManagerReader class.

    The default extension (and the only extension currently read) is
    *.v4d. The xml-based file lists a series of vtk files where time
    tags have to be populated (file paths are relative). 
    
    \see vtkDataMesh4D vtkDataManagerReader vtkMetaDataSetSequence
    \author Nicolas Toussaint
*/

class VTKDATAMESHPLUGIN_EXPORT vtkDataMesh4DReader: public vtkDataMeshReaderBase
{
    Q_OBJECT

public:
    vtkDataMesh4DReader();
    ~vtkDataMesh4DReader() override;

    QStringList handled() const override;
    static QStringList s_handled();

public slots:
    bool canRead(const QString& path) override;
    bool readInformation(const QString& path) override;
    bool read(const QString& path) override;

    QString identifier()  const override;
    QString description() const override;

    static bool registered();

protected:
    vtkDataManagerReader* reader;

private:
    static const char ID[];
    bool extractMetaData(vtkMetaDataSet *dataSet);
};

dtkAbstractDataReader *createVtkDataMesh4DReader();
