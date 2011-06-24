#include "medAbstractDataCollection.h"

#include "medAttachedData.h"

#include <dtkCore/medAbstractData.h>

#include <QtCore/QVector>

class medAbstractDataCollectionPrivate
{
public:
    QVector<medAbstractData*>     dataVector;
    QVector<medAbstractData*>::iterator dataIterator;
    QSharedPointer<medAttachedData>* attachedData;
};

medAbstractDataCollection::medAbstractDataCollection(medAbstractDataCollection *parent ): medAbstractData(parent), d(new medAbstractDataCollectionPrivate)
{
    // init iterator
    first();
}

medAbstractDataCollection::~medAbstractDataCollection()
{
    foreach (medAbstractData* data, d->dataVector)
        delete data;
    d->dataVector.clear();

    delete d;
    d = NULL;
}

int medAbstractDataCollection::count()
{
    return d->dataVector.count();
}

medAbstractData* medAbstractDataCollection::first()
{
    d->dataIterator = d->dataVector.begin();
    return (*d->dataIterator);
}

medAbstractData* medAbstractDataCollection::next()
{
    d->dataIterator++;
    if (d->dataIterator != d->dataVector.end())
        return (*d->dataIterator);
    else
        return NULL;
}

medAbstractData* medAbstractDataCollection::at( int index )
{
    if ((index > 0) && (index < d->dataVector.count()) )
        return d->dataVector.at(index);
    else
        return NULL;
}

void medAbstractDataCollection::addData( medAbstractData* data )
{
    d->dataVector.push_back(data);
    first();
}


void medAbstractDataCollection::setAttachData( medAttachedData* attachedData )
{
    if (d->attachedData != NULL)
        delete d->attachedData;
    d->attachedData = new QSharedPointer<medAttachedData> (attachedData);
}

medAttachedData* medAbstractDataCollection::attachedData()
{
    return d->attachedData->data();
}

//-----------------------------------------------------------------------------------


QString medAbstractDataCollection::description( void ) const
{
    return (*d->dataIterator)->description();
}

void medAbstractDataCollection::draw( void )
{
    return (*d->dataIterator)->draw();
}

bool medAbstractDataCollection::read( const QString& file )
{
    return (*d->dataIterator)->read(file);
}

bool medAbstractDataCollection::read( const QStringList& files )
{
    return (*d->dataIterator)->read(files);
}

bool medAbstractDataCollection::write( const QString& file )
{
    return (*d->dataIterator)->write(file);
}

bool medAbstractDataCollection::write( const QStringList& files )
{
    return (*d->dataIterator)->write(files);
}

medAbstractData * medAbstractDataCollection::convert( const QString& toType )
{
    return (*d->dataIterator)->convert(toType);
}

void * medAbstractDataCollection::output( void )
{
    return (*d->dataIterator)->output();
}

void * medAbstractDataCollection::output( int channel )
{
    return (*d->dataIterator)->output(channel);
}

void* medAbstractDataCollection::data( void )
{
    return (*d->dataIterator)->data();
}

void * medAbstractDataCollection::data( int channel )
{
    return (*d->dataIterator)->data(channel);
}

double medAbstractDataCollection::parameter( int channel )
{
    return (*d->dataIterator)->parameter(channel);
}

void medAbstractDataCollection::setParameter( int parameter )
{
    return (*d->dataIterator)->setParameter(parameter);
}

void medAbstractDataCollection::setParameter( int parameter, int channel )
{
    return (*d->dataIterator)->setParameter(parameter,channel);
}

void medAbstractDataCollection::setParameter( float parameter )
{
    return (*d->dataIterator)->setParameter(parameter);
}

void medAbstractDataCollection::setParameter( float parameter, int channel )
{
    return (*d->dataIterator)->setParameter(parameter,channel);
}

void medAbstractDataCollection::setParameter( double parameter )
{
    return (*d->dataIterator)->setParameter(parameter);
}

void medAbstractDataCollection::setParameter( double parameter, int channel )
{
    return (*d->dataIterator)->setParameter(parameter,channel);
}

void medAbstractDataCollection::setParameter( const QString& parameter )
{
    return (*d->dataIterator)->setParameter(parameter);
}

void medAbstractDataCollection::setParameter( const QString& parameter, int channel )
{
    return (*d->dataIterator)->setParameter(parameter, channel);
}

void medAbstractDataCollection::setParameter( medAbstractData *parameter )
{
    return (*d->dataIterator)->setParameter(parameter);
}

void medAbstractDataCollection::setParameter( medAbstractData *parameter, int channel )
{
    return (*d->dataIterator)->setParameter(parameter,channel);
}

void medAbstractDataCollection::setData( void* data )
{
    (*d->dataIterator)->setData(data);
}

void medAbstractDataCollection::setData( void* data, int channel )
{
    (*d->dataIterator)->setData(data, channel);
}

void medAbstractDataCollection::update( void )
{
    (*d->dataIterator)->update();
}

void medAbstractDataCollection::addReader( const QString& reader )
{
    (*d->dataIterator)->addReader(reader);
}

void medAbstractDataCollection::addWriter( const QString& writer )
{
    (*d->dataIterator)->addWriter(writer);
}

void medAbstractDataCollection::enableReader( const QString& reader )
{
    (*d->dataIterator)->enableReader(reader);
}

void medAbstractDataCollection::disableReader( const QString& reader )
{
    (*d->dataIterator)->disableReader(reader);
}

void medAbstractDataCollection::enableWriter( const QString& writer )
{
    (*d->dataIterator)->enableWriter(writer);
}

void medAbstractDataCollection::disableWriter( const QString& writer )
{
    (*d->dataIterator)->disableWriter(writer);
}

void medAbstractDataCollection::enableConverter( const QString& converter )
{
    (*d->dataIterator)->enableConverter(converter);
}

void medAbstractDataCollection::disableConverter( const QString& converter )
{
    (*d->dataIterator)->disableConverter(converter);
}

medAbstractDataReader * medAbstractDataCollection::reader( const QString& type )
{
    return (*d->dataIterator)->reader(type);
}

medAbstractDataWriter    * medAbstractDataCollection::writer( const QString& type )
{
    return (*d->dataIterator)->writer(type);
}

medAbstractDataConverter * medAbstractDataCollection::converter( const QString& type )
{
    return (*d->dataIterator)->converter(type);
}

QString medAbstractDataCollection::path( void )
{
    return (*d->dataIterator)->path();
}

QStringList medAbstractDataCollection::paths( void )
{
    return (*d->dataIterator)->paths();
}

QImage & medAbstractDataCollection::thumbnail( void ) const
{
    if (d->dataIterator != d->dataVector.end())
    {
        try
        {
            return (*d->dataIterator)->thumbnail();
        }
        catch(...)
        {
            return *(new QImage());
        }
    }
    else
        return *(new QImage());
}

QList<QImage>& medAbstractDataCollection::thumbnails( void ) const
{
    return (*d->dataIterator)->thumbnails();
}

bool medAbstractDataCollection::casts( const QString& type )
{
    return (*d->dataIterator)->casts(type);
}
