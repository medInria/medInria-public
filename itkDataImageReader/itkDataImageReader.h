#ifndef _itk_DataImageReader_h_
#define _itk_DataImageReader_h_


#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkAbstractDataReader.h>

#include "itkDataImageReaderPluginExport.h"



class itkDataImageChar3ReaderPrivate;

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkDataImageChar3Reader : public dtkAbstractDataReader
{
    Q_OBJECT

public:
     itkDataImageChar3Reader(void);
    ~itkDataImageChar3Reader(void);

    virtual QString description(void) const;
    virtual QStringList handled(void) const;
    static bool registered(void);
    
public slots:
    bool read(QString path);
    bool canRead (QString path);
    void readInformation (QString path);
    

 private:
    itkDataImageChar3ReaderPrivate* d;
};

dtkAbstractDataReader *createItkDataImageChar3Reader(void);




class itkDataImageUChar3ReaderPrivate;

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkDataImageUChar3Reader : public dtkAbstractDataReader
{
    Q_OBJECT

public:
     itkDataImageUChar3Reader(void);
    ~itkDataImageUChar3Reader(void);

    virtual QString description(void) const;
    virtual QStringList handled(void) const;
    static bool registered(void);
    
public slots:
    bool read(QString path);
    bool canRead (QString path);
    void readInformation (QString path);

 private:
    itkDataImageUChar3ReaderPrivate* d;
};

dtkAbstractDataReader *createItkDataImageUChar3Reader(void);



class itkDataImageShort3ReaderPrivate;

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkDataImageShort3Reader : public dtkAbstractDataReader
{
    Q_OBJECT

public:
     itkDataImageShort3Reader(void);
    ~itkDataImageShort3Reader(void);

    virtual QString description(void) const;
    virtual QStringList handled(void) const;
    static bool registered(void);
    
public slots:
    bool read(QString path);
    bool canRead (QString path);
    void readInformation (QString path);

 private:
    itkDataImageShort3ReaderPrivate* d;
};

dtkAbstractDataReader *createItkDataImageShort3Reader(void);



class itkDataImageUShort3ReaderPrivate;

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkDataImageUShort3Reader : public dtkAbstractDataReader
{
    Q_OBJECT

public:
     itkDataImageUShort3Reader(void);
    ~itkDataImageUShort3Reader(void);

    virtual QString description(void) const;
    virtual QStringList handled(void) const;
    static bool registered(void);
    
public slots:
    bool read(QString path);
    bool canRead (QString path);
    void readInformation (QString path);

 private:
    itkDataImageUShort3ReaderPrivate* d;
};

dtkAbstractDataReader *createItkDataImageUShort3Reader(void);



class itkDataImageInt3ReaderPrivate;

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkDataImageInt3Reader : public dtkAbstractDataReader
{
    Q_OBJECT

public:
     itkDataImageInt3Reader(void);
    ~itkDataImageInt3Reader(void);

    virtual QString description(void) const;
    virtual QStringList handled(void) const;
    static bool registered(void);
    
public slots:
    bool read(QString path);
    bool canRead (QString path);
    void readInformation (QString path);

 private:
    itkDataImageInt3ReaderPrivate* d;
};

dtkAbstractDataReader *createItkDataImageInt3Reader(void);



class itkDataImageUInt3ReaderPrivate;

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkDataImageUInt3Reader : public dtkAbstractDataReader
{
    Q_OBJECT

public:
     itkDataImageUInt3Reader(void);
    ~itkDataImageUInt3Reader(void);

    virtual QString description(void) const;
    virtual QStringList handled(void) const;
    static bool registered(void);
    
public slots:
    bool read(QString path);
    bool canRead (QString path);
    void readInformation (QString path);

 private:
    itkDataImageUInt3ReaderPrivate* d;
};

dtkAbstractDataReader *createItkDataImageUInt3Reader(void);



class itkDataImageLong3ReaderPrivate;

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkDataImageLong3Reader : public dtkAbstractDataReader
{
    Q_OBJECT

public:
     itkDataImageLong3Reader(void);
    ~itkDataImageLong3Reader(void);

    virtual QString description(void) const;
    virtual QStringList handled(void) const;
    static bool registered(void);
    
public slots:
    bool read(QString path);
    bool canRead (QString path);
    void readInformation (QString path);

 private:
    itkDataImageLong3ReaderPrivate* d;
};

dtkAbstractDataReader *createItkDataImageLong3Reader(void);



class itkDataImageULong3ReaderPrivate;

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkDataImageULong3Reader : public dtkAbstractDataReader
{
    Q_OBJECT

public:
     itkDataImageULong3Reader(void);
    ~itkDataImageULong3Reader(void);

    virtual QString description(void) const;
    virtual QStringList handled(void) const;
    static bool registered(void);
    
public slots:
    bool read(QString path);
    bool canRead (QString path);
    void readInformation (QString path);

 private:
    itkDataImageULong3ReaderPrivate* d;
};

dtkAbstractDataReader *createItkDataImageULong3Reader(void);



class itkDataImageFloat3ReaderPrivate;

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkDataImageFloat3Reader : public dtkAbstractDataReader
{
    Q_OBJECT

public:
     itkDataImageFloat3Reader(void);
    ~itkDataImageFloat3Reader(void);

    virtual QString description(void) const;
    virtual QStringList handled(void) const;
    static bool registered(void);
    
public slots:
    bool read(QString path);
    bool canRead (QString path);
    void readInformation (QString path);

 private:
    itkDataImageFloat3ReaderPrivate* d;
};

dtkAbstractDataReader *createItkDataImageFloat3Reader(void);



class itkDataImageDouble3ReaderPrivate;

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkDataImageDouble3Reader : public dtkAbstractDataReader
{
    Q_OBJECT

public:
     itkDataImageDouble3Reader(void);
    ~itkDataImageDouble3Reader(void);

    virtual QString description(void) const;
    virtual QStringList handled(void) const;
    static bool registered(void);
    
public slots:
    bool read(QString path);
    bool canRead (QString path);
    void readInformation (QString path);

 private:
    itkDataImageDouble3ReaderPrivate* d;
};

dtkAbstractDataReader *createItkDataImageDouble3Reader(void);


#endif
