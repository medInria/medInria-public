#ifndef ITKVTKDATAIMAGEWRITER_H
#define ITKVTKDATAIMAGEWRITER_H

#include <itkDataImageWriterBase.h>
#include <itkDataImageWriterPluginExport.h>

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkVTKDataImageWriter: public itkDataImageWriterBase {
public:
    itkVTKDataImageWriter();
    virtual ~itkVTKDataImageWriter();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled ();

    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataWriter *createItkVTKDataImageWriter();

#endif
