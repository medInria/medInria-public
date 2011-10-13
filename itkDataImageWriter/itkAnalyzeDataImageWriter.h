#ifndef ITKANALYZEDATAIMAGEWRITER_H
#define ITKANALYZEDATAIMAGEWRITER_H

#include <itkDataImageWriterBase.h>
#include <itkDataImageWriterPluginExport.h>

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkAnalyzeDataImageWriter : public itkDataImageWriterBase {
public:
    itkAnalyzeDataImageWriter();
    virtual ~itkAnalyzeDataImageWriter();
	
    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled ();
	
    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataWriter *createItkAnalyzeDataImageWriter();

#endif
