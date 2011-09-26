#ifndef ITKMETADATAIMAGEREADER_H
#define ITKMETADATAIMAGEREADER_H

#include <itkDataImageReaderBase.h>
#include <itkDataImageReaderPluginExport.h>

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkMetaDataImageReader: public itkDataImageReaderBase {
    Q_OBJECT

public:
    itkMetaDataImageReader();
    virtual ~itkMetaDataImageReader();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled();

    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataReader *createItkMetaDataImageReader();

#endif
