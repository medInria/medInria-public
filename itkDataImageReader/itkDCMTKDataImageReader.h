// /////////////////////////////////////////////////////////////////
// Generated by dtkPluginGenerator
// /////////////////////////////////////////////////////////////////

#ifndef ITKDCMTKIMAGEDATAREADER_H
#define ITKDCMTKIMAGEDATAREADER_H

#include <dtkCore/dtkAbstractDataReader.h>

#include "itkDataImageReaderPluginExport.h"

class itkDCMTKDataImageReaderPrivate;

namespace itk
{
    class SliceReadCommand;
}

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkDCMTKDataImageReader : public dtkAbstractDataReader
{
    Q_OBJECT

public:
             itkDCMTKDataImageReader(void);
    virtual ~itkDCMTKDataImageReader(void);

    virtual QString description(void) const;

    virtual QStringList handled(void) const;
    static bool registered(void);

    friend class itk::SliceReadCommand;
    
public slots:
    bool canRead (QString path);
    bool canRead (QStringList paths);

    void readInformation (QString path);
    void readInformation (QStringList paths);
    
    bool read (QString path);
    bool read (QStringList paths);

    void setProgress (int value);
    
private:
    itkDCMTKDataImageReaderPrivate *d;
};

dtkAbstractDataReader *createItkDCMTKDataImageReader(void);

#endif
