#pragma once

#include "itkDataTensorImageWriterBase.h"

#include "itkDataTensorImageWriterPluginExport.h"

class ITKDATATENSORIMAGEWRITERPLUGIN_EXPORT itkNiftiDataTensorImageWriter : public itkDataTensorImageWriterBase
{
    Q_OBJECT

public:
    itkNiftiDataTensorImageWriter();
    virtual ~itkNiftiDataTensorImageWriter();

    virtual QString description() const;
    virtual QString identifier() const;

    static bool registered();

private:
    static const char ID[];
};

dtkAbstractDataWriter *createitkNiftiDataTensorImageWriter();


