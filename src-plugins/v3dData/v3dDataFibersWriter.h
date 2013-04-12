#pragma once

#include <dtkCore/dtkAbstractDataWriter.h>

#include "v3dDataPluginExport.h"

class v3dDataFibersWriterPrivate;

class V3DDATAPLUGIN_EXPORT v3dDataFibersWriter : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
             v3dDataFibersWriter();
    virtual ~v3dDataFibersWriter();

    virtual QStringList handled() const;
    
    virtual QString identifier()  const;
    virtual QString description() const;

    static bool registered();

    /** Override base class */
    virtual QStringList supportedFileExtensions() const;

public slots:
    bool write    (const QString& path);
    bool canWrite (const QString& path);

 private:
    v3dDataFibersWriterPrivate *d;
};


dtkAbstractDataWriter *createV3dDataFibersWriter();


