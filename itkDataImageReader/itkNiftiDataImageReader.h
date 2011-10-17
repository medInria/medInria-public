#ifndef ITKNIFTIDATAIMAGEREADER_H
#define ITKNIFTIDATAIMAGEREADER_H

#include <itkDataImageReaderBase.h>
#include <itkDataImageReaderPluginExport.h>

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkNiftiDataImageReader: public itkDataImageReaderBase {
public:
    itkNiftiDataImageReader();
    virtual ~itkNiftiDataImageReader();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled();

    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataReader *createItkNiftiDataImageReader();

#endif
