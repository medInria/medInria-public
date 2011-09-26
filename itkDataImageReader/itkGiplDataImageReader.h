#ifndef ITKGIPLDATAIMAGEREADER_H
#define ITKGIPLDATAIMAGEREADER_H

#include <itkDataImageReaderBase.h>
#include <itkDataImageReaderPluginExport.h>

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkGiplDataImageReader: public itkDataImageReaderBase {
public:
    itkGiplDataImageReader();
    virtual ~itkGiplDataImageReader();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled();

    static bool registered();	

private:
    
    static const char ID[];
};

dtkAbstractDataReader *createItkGiplDataImageReader();

#endif
