#ifndef ITKPHILIPSRECDATAIMAGEREADER_H
#define ITKPHILIPSRECDATAIMAGEREADER_H

#include <itkDataImageBase/itkDataImageReaderBase.h>
#include <itkDataImageReaderPluginExport.h>

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkPhilipsRECDataImageReader: public itkDataImageReaderBase {
public:
    itkPhilipsRECDataImageReader();
    virtual ~itkPhilipsRECDataImageReader();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled();

    static bool registered();

private:

    static const char ID[];
    static dtkAbstractDataReader* create();
};

#endif  //  ! ITKPHILIPSRECDATAIMAGEREADER_H
