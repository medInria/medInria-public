#ifndef ITKANALYZEDATAIMAGEREADER_H
#define ITKANALYZEDATAIMAGEREADER_H

#include <itkDataImageReaderBase/itkDataImageReaderBase.h>
#include <itkDataImageReaderPluginExport.h>

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkAnalyzeDataImageReader: public itkDataImageReaderBase {
public:
    itkAnalyzeDataImageReader();
    virtual ~itkAnalyzeDataImageReader();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled();

    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataReader *createItkAnalyzeDataImageReader();

#endif
