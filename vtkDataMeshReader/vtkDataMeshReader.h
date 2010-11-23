#ifndef VTKDATAMESHREADER_H
#define VTKDATAMESHREADER_H

#include <dtkCore/dtkAbstractDataReader.h>

#include "vtkDataMeshReaderPluginExport.h"

class vtkDataSetReader;

class VTKDATAMESHREADERPLUGIN_EXPORT vtkDataMeshReader : public dtkAbstractDataReader
{
  Q_OBJECT
    
public:
  vtkDataMeshReader(void);
  virtual ~vtkDataMeshReader(void);
  
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

  vtkDataSetReader* reader;
};


dtkAbstractDataReader *createVtkDataMeshReader(void);


#endif
