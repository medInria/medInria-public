#pragma once

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


