#pragma once

#include <itkDataImageBase/itkDataImageWriterBase.h>
#include <itkDataImageWriterPluginExport.h>

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkTIFFDataImageWriter: public itkDataImageWriterBase {
public:
    itkTIFFDataImageWriter();
    virtual ~itkTIFFDataImageWriter();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled ();

    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataWriter *createItkTIFFDataImageWriter();


