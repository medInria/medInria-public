/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkDataMeshPluginExport.h"

#include <dtkCoreSupport/dtkAbstractDataReader.h>

class vtkDataSetReader;
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

class VTKDATAMESHPLUGIN_EXPORT vtkDataMesh4DReader: public dtkAbstractDataReader {
    Q_OBJECT

public:
    vtkDataMesh4DReader();
    virtual ~vtkDataMesh4DReader();

    virtual QStringList handled() const;

    static QStringList s_handled();

public slots:
    virtual bool canRead(const QString& path);
    virtual bool canRead(const QStringList& paths);

    virtual bool readInformation(const QString& path);
    virtual bool readInformation(const QStringList& paths);

    virtual bool read(const QString& path);
    virtual bool read(const QStringList& paths);

    virtual void setProgress(int value);

    virtual QString identifier()  const;
    virtual QString description() const;

    static bool registered();	

protected:

    vtkDataManagerReader* reader;

private:

    static const char ID[];
};

dtkAbstractDataReader *createVtkDataMesh4DReader();



