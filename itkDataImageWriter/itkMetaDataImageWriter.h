#ifndef ITKMETADATAIMAGEWRITER_H
#define ITKMETADATAIMAGEWRITER_H

#include <itkDataImageWriterBase.h>
#include <itkDataImageWriterPluginExport.h>

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkMetaDataImageWriter: public itkDataImageWriterBase {
public:
    itkMetaDataImageWriter();
    virtual ~itkMetaDataImageWriter();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled ();

    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataWriter *createItkMetaDataImageWriter();

#endif
