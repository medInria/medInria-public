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
    virtual QString identifier(void) const;

    static bool registered(void);

private:
    static const char ID[];
};

dtkAbstractDataWriter *createItkMetaDataTensorImageWriter(void);

#endif /* ITKMETADATATENSORIMAGEWRITER_H_ */
