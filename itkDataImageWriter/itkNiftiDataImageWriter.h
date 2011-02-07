#ifndef ITKNIFTIDATAIMAGEWRITER_H
#define ITKNIFTIDATAIMAGEWRITER_H

#include "itkDataImageWriterBase.h"

#include "itkDataImageWriterPluginExport.h"

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkNiftiDataImageWriter : public itkDataImageWriterBase
{
    Q_OBJECT

public:
    itkNiftiDataImageWriter(void);
    virtual ~itkNiftiDataImageWriter(void);

    virtual QString description(void) const;

    QStringList handled(void) const;

    static QStringList s_handled (void);

    static bool registered(void);	
};

dtkAbstractDataWriter *createItkNiftiDataImageWriter(void);

#endif
