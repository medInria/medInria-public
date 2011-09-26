#ifndef ITKBMPDATAIMAGEWRITER_H
#define ITKBMPDATAIMAGEWRITER_H

#include <itkDataImageWriterBase.h>
#include <itkDataImageWriterPluginExport.h>

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkBMPDataImageWriter: public itkDataImageWriterBase {
public:
    itkBMPDataImageWriter();
    virtual ~itkBMPDataImageWriter();
	
    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled ();
	
    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataWriter *createItkBMPDataImageWriter();

#endif
