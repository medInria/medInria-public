#include "itkDataImageBase.h"


#include "itkDataImageBaseImpl.h"


itkDataImageBase::itkDataImageBase( const QString & description, itkDataImageBaseImpl * impl )
    : m_description(description),
    d (impl)
{

}

itkDataImageBase::~itkDataImageBase()
{
    delete d;
    d = NULL;
};

QString itkDataImageBase::description( void ) const
{
    return m_description;
}

QImage        & itkDataImageBase::thumbnail( void ) const
{
    return d->thumbnail();
}

QList<QImage> & itkDataImageBase::thumbnails( void ) const
{
    return d->thumbnails();
}

void * itkDataImageBase::output( void )
{
    return d->output();
}

void * itkDataImageBase::data( void )
{
    return d->data();
}

void itkDataImageBase::setData( void* data )
{
    d->setData(data);
}

void itkDataImageBase::update( void )
{
    d->update();
}

void itkDataImageBase::onMetaDataSet( const QString& key, const QString& value )
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

void itkDataImageBase::onPropertySet( const QString& key, const QString& value )
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

int itkDataImageBase::xDimension( void )
{
    return d->xDimension();
}

int itkDataImageBase::yDimension( void )
{
    return d->yDimension();
}

int itkDataImageBase::zDimension( void )
{
    return d->zDimension();
}

int itkDataImageBase::tDimension( void )
{
    return d->tDimension();
}

int itkDataImageBase::minRangeValue( void )
{
    return d->minRangeValue();
}

int itkDataImageBase::maxRangeValue( void )
{
    return d->maxRangeValue();
}

int itkDataImageBase::scalarValueCount( int value )
{
    return d->scalarValueCount(value);
}

int itkDataImageBase::scalarValueMinCount( void )
{
    return d->scalarValueMinCount();
}

int itkDataImageBase::scalarValueMaxCount( void )
{
    return d->scalarValueMaxCount();
}
