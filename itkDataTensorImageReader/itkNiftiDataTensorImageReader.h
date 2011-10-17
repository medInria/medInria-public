#ifndef ITKNIFTIDATATENSORIMAGEREADER_H
#define ITKNIFTIDATATENSORIMAGEREADER_H

#include <itkDataTensorImageReaderBase.h>
#include <itkDataTensorImageReaderPluginExport.h>

class ITKDATATENSORIMAGEREADERPLUGIN_EXPORT itkNiftiDataTensorImageReader: public itkDataTensorImageReaderBase {
    Q_OBJECT
	
public:

    itkNiftiDataTensorImageReader();
    virtual ~itkNiftiDataTensorImageReader();
	
    virtual QString identifier()  const;
    virtual QString description() const;
	
    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataReader *createItkNiftiDataTensorImageReader();

#endif
