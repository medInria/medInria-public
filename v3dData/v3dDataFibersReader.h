#ifndef V3DDATAFIBERSREADER_H
#define V3DDATAFIBERSREADER_H

#include <dtkCore/dtkAbstractDataReader.h>

#include "v3dDataPluginExport.h"

class v3dDataFibersReaderPrivate;

class V3DDATAPLUGIN_EXPORT v3dDataFibersReader : public dtkAbstractDataReader
{
  Q_OBJECT
    
public:
  v3dDataFibersReader(void);
  virtual ~v3dDataFibersReader(void);
  
  virtual QStringList handled(void) const;
  
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

private:
  v3dDataFibersReaderPrivate* d;
};

dtkAbstractDataReader *createV3dDataFibersReader(void);

#endif
