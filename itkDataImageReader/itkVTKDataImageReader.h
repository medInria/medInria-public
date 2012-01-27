#ifndef ITKVTKDATAIMAGEREADER_H
#define ITKVTKDATAIMAGEREADER_H

#include <itkDataImageReaderBase/itkDataImageReaderBase.h>

#include "itkDataImageReaderPluginExport.h"

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkVTKDataImageReader: public itkDataImageReaderBase {
public:
    itkVTKDataImageReader(void);
    virtual ~itkVTKDataImageReader(void);

    virtual QString identifier(void) const;
    virtual QString description(void) const;

    QStringList handled(void) const;

    static QStringList s_handled (void);

    static bool registered(void);

private:

    static const char ID[];
};

dtkAbstractDataReader *createItkVTKDataImageReader(void);

#endif
