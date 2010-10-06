#ifndef VTKDATAMESHREADER_H
#define VTKDATAMESHREADER_H

#include <dtkCore/dtkAbstractDataReader.h>

#include "vtkDataMeshReaderPluginExport.h"
#include "vtkDataSetReader.h"

class VTKDATAMESHREADERPLUGIN_EXPORT vtkDataMeshReader : public dtkAbstractDataReader
{
  Q_OBJECT
    
public:
  vtkDataMeshReader(void);
  virtual ~vtkDataMeshReader(void);
  
  virtual QStringList handled(void) const;
  
  static QStringList s_handled (void);
  
public slots:
  virtual bool canRead (QString path);
  virtual bool canRead (QStringList paths);
  
  virtual void readInformation (QString path);
  virtual void readInformation (QStringList paths);
  
  virtual bool read (QString path);
  virtual bool read (QStringList paths);
  
  virtual void setProgress (int value);

  virtual QString description(void) const;
  
  static bool registered(void);	

    
protected:

  vtkDataSetReader* reader;
};


dtkAbstractDataReader *createVtkDataMeshReader(void);


#endif
