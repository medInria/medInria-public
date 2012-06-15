#ifndef V3DDATAFIBERSREADER_H
#define V3DDATAFIBERSREADER_H

#include <dtkCore/dtkAbstractDataReader.h>
#include <v3dDataPluginExport.h>

class v3dDataFibersReaderPrivate;

class V3DDATAPLUGIN_EXPORT v3dDataFibersReader: public dtkAbstractDataReader {
    Q_OBJECT

public:
    v3dDataFibersReader();
    virtual ~v3dDataFibersReader();

    virtual QStringList handled() const;

public slots:
    virtual bool canRead (const QString& path);
    virtual bool canRead (const QStringList& paths);

    virtual bool readInformation (const QString& path);
    virtual bool readInformation (const QStringList& paths);

    virtual bool read (const QString& path);
    virtual bool read (const QStringList& paths);

    virtual void setProgress (int value);

    virtual QString identifier()  const;
    virtual QString description() const;

    static bool registered();	

private:

    v3dDataFibersReaderPrivate* d;

    static const char ID[];
};

dtkAbstractDataReader *createV3dDataFibersReader();

#endif
