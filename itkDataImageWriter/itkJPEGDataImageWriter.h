#ifndef ITKJPEGDATAIMAGEWRITER_H
#define ITKJPEGDATAIMAGEWRITER_H

#include "itkDataImageWriterBase.h"

#include "itkDataImageWriterPluginExport.h"

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkJPEGDataImageWriter : public itkDataImageWriterBase
{
    Q_OBJECT

public:
    itkJPEGDataImageWriter(void);
    virtual ~itkJPEGDataImageWriter(void);

    virtual QString description(void) const;

    QStringList handled(void) const;

    static QStringList s_handled (void);

    static bool registered(void);	
};

dtkAbstractDataWriter *createItkJPEGDataImageWriter(void);

#endif
