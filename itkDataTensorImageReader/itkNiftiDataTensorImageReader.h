#ifndef ITKNIFTIDATATENSORIMAGEREADER_H
#define ITKNIFTIDATATENSORIMAGEREADER_H

#include "itkDataTensorImageReaderBase.h"

#include "itkDataTensorImageReaderPluginExport.h"

class ITKDATATENSORIMAGEREADERPLUGIN_EXPORT itkNiftiDataTensorImageReader : public itkDataTensorImageReaderBase
{
    Q_OBJECT
	
public:
    itkNiftiDataTensorImageReader(void);
    virtual ~itkNiftiDataTensorImageReader(void);
	
    virtual QString description(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataReader *createItkNiftiDataTensorImageReader(void);

#endif
