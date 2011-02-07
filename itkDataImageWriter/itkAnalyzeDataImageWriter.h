#ifndef ITKANALYZEDATAIMAGEWRITER_H
#define ITKANALYZEDATAIMAGEWRITER_H

#include "itkDataImageWriterBase.h"

#include "itkDataImageWriterPluginExport.h"

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkAnalyzeDataImageWriter : public itkDataImageWriterBase
{
    Q_OBJECT
	
public:
    itkAnalyzeDataImageWriter(void);
    virtual ~itkAnalyzeDataImageWriter(void);
	
    virtual QString description(void) const;

    QStringList handled(void) const;

    static QStringList s_handled (void);
	
    static bool registered(void);	
};

dtkAbstractDataWriter *createItkAnalyzeDataImageWriter(void);

#endif
