#ifndef VTKDATAMESH4DREADER_H
#define VTKDATAMESH4DREADER_H

#include <dtkCore/dtkAbstractDataReader.h>

#include "vtkDataMeshReaderPluginExport.h"

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

class VTKDATAMESHREADERPLUGIN_EXPORT vtkDataMesh4DReader : public dtkAbstractDataReader
{
  Q_OBJECT
    
public:
  vtkDataMesh4DReader(void);
  virtual ~vtkDataMesh4DReader(void);
  
  virtual QStringList handled(void) const;
  
  static QStringList s_handled (void);
  
public slots:
  virtual bool canRead (const QString& path);
  virtual bool canRead (const QStringList& paths);
  
  virtual void readInformation (const QString& path);
  virtual void readInformation (const QStringList& paths);
  
  virtual bool read (const QString& path);
  virtual bool read (const QStringList& paths);
  
  virtual void setProgress (int value);

  virtual QString description(void) const;
  
  static bool registered(void);	

    
protected:

  vtkDataManagerReader* reader;
  
};


dtkAbstractDataReader *createVtkDataMesh4DReader(void);


#endif
