#ifndef itkNrrdDataTensorImageWriter_H_
#define itkNrrdDataTensorImageWriter_H_

#include "itkDataTensorImageWriterBase.h"

#include "itkDataTensorImageWriterPluginExport.h"

class ITKDATATENSORIMAGEWRITERPLUGIN_EXPORT itkNrrdDataTensorImageWriter : public itkDataTensorImageWriterBase
{
    Q_OBJECT

public:
    itkNrrdDataTensorImageWriter(void);
    virtual ~itkNrrdDataTensorImageWriter(void);

    virtual QString description(void) const;
    virtual QString identifier(void) const;

    static bool registered(void);

private:
    static const char ID[];
};

dtkAbstractDataWriter *createitkNrrdDataTensorImageWriter(void);

#endif /* itkNrrdDataTensorImageWriter_H_ */
