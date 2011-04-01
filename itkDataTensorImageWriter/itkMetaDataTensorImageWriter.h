#ifndef ITKMETADATATENSORIMAGEWRITER_H_
#define ITKMETADATATENSORIMAGEWRITER_H_

#include "itkDataTensorImageWriterBase.h"

#include "itkDataTensorImageWriterPluginExport.h"

class ITKDATATENSORIMAGEWRITERPLUGIN_EXPORT itkMetaDataTensorImageWriter : public itkDataTensorImageWriterBase
{
    Q_OBJECT

public:
    itkMetaDataTensorImageWriter(void);
    virtual ~itkMetaDataTensorImageWriter(void);

    virtual QString description(void) const;

    static bool registered(void);
};

dtkAbstractDataWriter *createItkMetaDataTensorImageWriter(void);

#endif /* ITKMETADATATENSORIMAGEWRITER_H_ */
