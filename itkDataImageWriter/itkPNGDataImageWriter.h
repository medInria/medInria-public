#ifndef ITKPNGDATAIMAGEWRITER_H
#define ITKPNGDATAIMAGEWRITER_H

#include "itkDataImageWriterBase.h"

#include "itkDataImageWriterPluginExport.h"

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkPNGDataImageWriter : public itkDataImageWriterBase
{
    Q_OBJECT

public:
    itkPNGDataImageWriter(void);
    virtual ~itkPNGDataImageWriter(void);

    virtual QString description(void) const;

    QStringList handled(void) const;

    static QStringList s_handled (void);

    static bool registered(void);	
};

dtkAbstractDataWriter *createItkPNGDataImageWriter(void);

#endif
