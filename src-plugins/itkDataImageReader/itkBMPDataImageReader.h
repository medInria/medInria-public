#pragma once

#include <itkDataImageBase/itkDataImageReaderBase.h>
#include <itkDataImageReaderPluginExport.h>

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkBMPDataImageReader: public itkDataImageReaderBase {
public:
    itkBMPDataImageReader();
    virtual ~itkBMPDataImageReader();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled();

    static bool registered();

private:

    static const char ID[];
};

dtkAbstractDataReader *createItkBMPDataImageReader();


