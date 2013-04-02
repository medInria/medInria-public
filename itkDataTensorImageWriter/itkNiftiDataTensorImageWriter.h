#pragma once

#include "itkDataTensorImageWriterBase.h"

#include "itkDataTensorImageWriterPluginExport.h"

class ITKDATATENSORIMAGEWRITERPLUGIN_EXPORT itkNiftiDataTensorImageWriter : public itkDataTensorImageWriterBase
{
    Q_OBJECT

public:
    itkNiftiDataTensorImageWriter(void);
    virtual ~itkNiftiDataTensorImageWriter(void);

    virtual QString description(void) const;
    virtual QString identifier(void) const;

    static bool registered(void);

private:
    static const char ID[];
};

dtkAbstractDataWriter *createitkNiftiDataTensorImageWriter(void);


