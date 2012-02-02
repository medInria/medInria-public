#ifndef ITKGIPLDATAIMAGEWRITER_H
#define ITKGIPLDATAIMAGEWRITER_H

#include <itkDataImageBase/itkDataImageWriterBase.h>
#include <itkDataImageWriterPluginExport.h>

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkGiplDataImageWriter: public itkDataImageWriterBase {
public:
    itkGiplDataImageWriter();
    virtual ~itkGiplDataImageWriter();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled ();

    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataWriter *createItkGiplDataImageWriter();

#endif
