#ifndef ITKMETADATASHIMAGEWRITER_H_
#define ITKMETADATASHIMAGEWRITER_H_

#include "itkDataSHImageWriterBase.h"

#include "itkDataSHImageWriterPluginExport.h"

class ITKDATASHIMAGEWRITERPLUGIN_EXPORT itkMetaDataSHImageWriter : public itkDataSHImageWriterBase
{
    Q_OBJECT

public:
    itkMetaDataSHImageWriter(void);
    virtual ~itkMetaDataSHImageWriter(void);

    virtual QString description(void) const;
    virtual QString identifier(void) const;

    static bool registered(void);
};

dtkAbstractDataWriter *createItkMetaDataSHImageWriter(void);

#endif /* ITKMETADATASHIMAGEWRITER_H_ */
