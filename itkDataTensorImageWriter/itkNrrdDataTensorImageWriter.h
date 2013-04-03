#pragma once

#include "itkDataTensorImageWriterBase.h"

#include "itkDataTensorImageWriterPluginExport.h"

class ITKDATATENSORIMAGEWRITERPLUGIN_EXPORT itkNrrdDataTensorImageWriter : public itkDataTensorImageWriterBase
{
    Q_OBJECT

public:
    itkNrrdDataTensorImageWriter();
    virtual ~itkNrrdDataTensorImageWriter();

    virtual QString description() const;
    virtual QString identifier() const;

    static bool registered();

private:
    static const char ID[];
};

dtkAbstractDataWriter *createitkNrrdDataTensorImageWriter();


