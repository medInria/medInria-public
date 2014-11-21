/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractDataReader.h>
#include <medVtkMeshExport.h>

class vtkDataSetReader;
class vtkDataManagerReader;

/**
    \class medVtk4DMeshDataReader
    \brief This class implements the reading process of a sequence
    (series) of meshes in vtk format.

    It make extensive use of the vtkDataManager class and API (in
    vtkINRIA3D repository). The input file is home-made and xml-based,
    it derives from the API described in vtkDataManagerReader class.

    The default extension (and the only extension currently read) is
    *.v4d. The xml-based file lists a series of vtk files where time
    tags have to be populated (file paths are relative).

    \see medVtk4DMeshData vtkDataManagerReader vtkMetaDataSetSequence
    \author Nicolas Toussaint
*/

class MEDVTKMESHPLUGIN_EXPORT medVtk4DMeshDataReader: public dtkAbstractDataReader {
    Q_OBJECT

public:
    medVtk4DMeshDataReader();
    virtual ~medVtk4DMeshDataReader();

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

dtkAbstractDataReader *createmedVtk4DMeshDataReader();



