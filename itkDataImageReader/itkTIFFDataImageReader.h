#ifndef ITKTIFFDATAIMAGEREADER_H
#define ITKTIFFDATAIMAGEREADER_H

#include <itkDataImageReaderBase.h>
#include <itkDataImageReaderPluginExport.h>

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkTIFFDataImageReader: public itkDataImageReaderBase {
public:
    itkTIFFDataImageReader();
    virtual ~itkTIFFDataImageReader();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled();

    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataReader *createItkTIFFDataImageReader();

#endif
