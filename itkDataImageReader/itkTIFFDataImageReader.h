#ifndef ITKTIFFDATAIMAGEREADER_H
#define ITKTIFFDATAIMAGEREADER_H

#include "itkDataImageReaderBase.h"

#include "itkDataImageReaderPluginExport.h"

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkTIFFDataImageReader : public itkDataImageReaderBase
{
    Q_OBJECT

public:
    itkTIFFDataImageReader(void);
    virtual ~itkTIFFDataImageReader(void);

    virtual QString description(void) const;

    QStringList handled(void) const;

    static QStringList s_handled (void);

    static bool registered(void);	
};

dtkAbstractDataReader *createItkTIFFDataImageReader(void);

#endif
