#ifndef medAbstractDataCollection_h__
#define medAbstractDataCollection_h__

#include "medCoreExport.h"
#include "medAbstractData.h"

class dtkAbstractData;
class medAttachedData;
class medAbstractDataCollectionPrivate;

/**
 * Collection type of dtkAbstractData
 * using qVector internally
 */
class MEDCORE_EXPORT medAbstractDataCollection : public medAbstractData
{

public:
    medAbstractDataCollection(medAbstractDataCollection *parent = 0);
    ~medAbstractDataCollection();

    // satisfying dtkAbstractData

    virtual QString description(void) const;

    virtual void draw(void);

    public slots:
        virtual bool read(const QString& file);
        virtual bool read(const QStringList& files);

        virtual bool write(const QString& file);
        virtual bool write(const QStringList& files);

        virtual dtkAbstractData *convert(const QString& toType);

        virtual void *output(void);
        virtual void *output(int channel);

        virtual void *data(void);
        virtual void *data(int channel);

        virtual double parameter(int channel);

        virtual void setParameter(int parameter);
        virtual void setParameter(int parameter, int channel);

        virtual void setParameter(float parameter);
        virtual void setParameter(float parameter, int channel);

        virtual void setParameter(double parameter);
        virtual void setParameter(double parameter, int channel);

        virtual void setParameter(const QString& parameter);
        virtual void setParameter(const QString& parameter, int channel);

        virtual void setParameter(dtkAbstractData *parameter);
        virtual void setParameter(dtkAbstractData *parameter, int channel);

        virtual void setData(void* data);
        virtual void setData(void* data, int channel);

        virtual void update(void);

        void addReader   (const QString& reader);
        void addWriter   (const QString& writer);
        void addConverter(const QString& converter);

        void  enableReader(const QString& reader);
        void disableReader(const QString& reader);

        void  enableWriter(const QString& writer);
        void disableWriter(const QString& writer);

        void  enableConverter(const QString& converter);
        void disableConverter(const QString& converter);

        dtkAbstractDataReader    *reader   (const QString& type);
        dtkAbstractDataWriter    *writer   (const QString& type);
        dtkAbstractDataConverter *converter(const QString& type);

        QString     path(void);
        QStringList paths(void);

        virtual       QImage & thumbnail(void)  const;
        virtual QList<QImage>& thumbnails(void) const;

public:
    virtual bool casts(const QString& type);


    /**
     * Returns the number of dtkAbstractData items currently collected
     */
    int count();

    /**
     * Get the first item in the container or null
     * rewinds the iterator
     */
    dtkAbstractData* first();

    /**
     * Get the next item or null 
     * increments the iterator
     */
    dtkAbstractData* next();

    /**
     * Return item at index or null
     */
    dtkAbstractData* at(int index);


    /**
    * Add new data to the collection, it will be appended
    * The caller can safely delete the pointer afterwards
    * @params dtkAbstractData * data
    * @return void
    */
    void addData(dtkAbstractData* data);


    /**
    * You can only attach one instance per collection
    * the caller can safely delete the pointer afterwards
    * @params medAttachedData * attachedData
    * @return void
    */
    void setAttachData(medAttachedData* attachedData);

    /**
    * Get the attached data instance
    * @return medAttachedData* pointer to data
    */
    medAttachedData* attachedData(); 

private:
    medAbstractDataCollectionPrivate*   d;
};

#endif // medAbstractDataCollection_h__
