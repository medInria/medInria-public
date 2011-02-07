#ifndef ITKMETADATAIMAGEREADER_H
#define ITKMETADATAIMAGEREADER_H

#include "itkDataImageReaderBase.h"

#include "itkDataImageReaderPluginExport.h"

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkMetaDataImageReader : public itkDataImageReaderBase
{
    Q_OBJECT

public:
    itkMetaDataImageReader(void);
    virtual ~itkMetaDataImageReader(void);

    virtual QString description(void) const;

    QStringList handled(void) const;

    static QStringList s_handled (void);

    static bool registered(void);	
};

dtkAbstractDataReader *createItkMetaDataImageReader(void);

#endif
