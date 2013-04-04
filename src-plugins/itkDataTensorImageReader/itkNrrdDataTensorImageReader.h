#pragma once

#include <itkDataTensorImageReaderBase.h>
#include <itkDataTensorImageReaderPluginExport.h>

class ITKDATATENSORIMAGEREADERPLUGIN_EXPORT itkNrrdDataTensorImageReader: public itkDataTensorImageReaderBase {
    Q_OBJECT
	
public:

    itkNrrdDataTensorImageReader();
    virtual ~itkNrrdDataTensorImageReader();
	
    virtual QString identifier()  const;
    virtual QString description() const;
	
    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataReader *createItkNrrdDataTensorImageReader();


