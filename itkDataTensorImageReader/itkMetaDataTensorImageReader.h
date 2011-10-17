#ifndef ITKMETADATATENSORIMAGEREADER_H
#define ITKMETADATATENSORIMAGEREADER_H

#include <itkDataTensorImageReaderBase.h>
#include <itkDataTensorImageReaderPluginExport.h>

class ITKDATATENSORIMAGEREADERPLUGIN_EXPORT itkMetaDataTensorImageReader: public itkDataTensorImageReaderBase {
    Q_OBJECT
	
public:
    itkMetaDataTensorImageReader();
    virtual ~itkMetaDataTensorImageReader();
	
    virtual QString identifier()  const;
    virtual QString description() const;
	
    static bool registered();

private:
    
    static const char ID[];
};

dtkAbstractDataReader *createItkMetaDataTensorImageReader();

#endif
