#ifndef ITKNRRDDATAIMAGEREADER_H
#define ITKNRRDDATAIMAGEREADER_H

#include <itkDataImageReaderBase.h>
#include <itkDataImageReaderPluginExport.h>

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkNrrdDataImageReader: public itkDataImageReaderBase {

    Q_OBJECT

public:

    itkNrrdDataImageReader();
    virtual ~itkNrrdDataImageReader();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled();

    static bool registered();	

private:

    static const char ID[];
};

dtkAbstractDataReader *createItkNrrdDataImageReader();

#endif
