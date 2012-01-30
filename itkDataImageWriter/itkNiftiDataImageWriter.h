#ifndef ITKNIFTIDATAIMAGEWRITER_H
#define ITKNIFTIDATAIMAGEWRITER_H

#include <itkDataImageBase/itkDataImageWriterBase.h>
#include <itkDataImageWriterPluginExport.h>

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkNiftiDataImageWriter: public itkDataImageWriterBase {
public:
    itkNiftiDataImageWriter();
    virtual ~itkNiftiDataImageWriter();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled ();

    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataWriter *createItkNiftiDataImageWriter();

#endif
