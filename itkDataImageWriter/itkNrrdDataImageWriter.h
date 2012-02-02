#ifndef ITKNRRDDATAIMAGEWRITER_H
#define ITKNRRDDATAIMAGEWRITER_H

#include <itkDataImageBase/itkDataImageWriterBase.h>
#include <itkDataImageWriterPluginExport.h>

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkNrrdDataImageWriter: public itkDataImageWriterBase {
public:
    itkNrrdDataImageWriter();
    virtual ~itkNrrdDataImageWriter();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled ();

    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataWriter *createItkNrrdDataImageWriter();

#endif
