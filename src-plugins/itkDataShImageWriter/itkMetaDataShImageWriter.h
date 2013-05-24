#ifndef ITKMETADATASHIMAGEWRITER_H_
#define ITKMETADATASHIMAGEWRITER_H_

#include "itkDataShImageWriterBase.h"

#include "itkDataShImageWriterPluginExport.h"

class ITKDATASHIMAGEWRITERPLUGIN_EXPORT itkMetaDataShImageWriter : public itkDataShImageWriterBase
{
    Q_OBJECT

public:
    itkMetaDataShImageWriter(void);
    virtual ~itkMetaDataShImageWriter(void);

    virtual QString description(void) const;
    virtual QString identifier(void) const;

    static bool registered(void);
};

dtkAbstractDataWriter *createItkMetaDataShImageWriter(void);

#endif /* ITKMETADATASHIMAGEWRITER_H_ */
