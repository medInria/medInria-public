#include "medAbstractDataCollection.h"

#include "medAbstractData.h"

#include <QtCore/QVector>

class medAbstractDataCollectionPrivate
{
public:
    QVector<QSharedPointer<medAbstractData>>     dataVector;
    QVector<QSharedPointer<medAbstractData>>::iterator dataIterator;
};

medAbstractDataCollection::medAbstractDataCollection() : d(new medAbstractDataCollectionPrivate)
{
    // init iterator
    first();
}

medAbstractDataCollection::~medAbstractDataCollection()
{
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
    return (*d->dataIterator).data();
}

medAbstractData* medAbstractDataCollection::next()
{
    d->dataIterator++;
    if (d->dataIterator != d->dataVector.end())
        return (*d->dataIterator).data();
    else
        return NULL;
}

medAbstractData* medAbstractDataCollection::at( int index )
{
    if ((index > 0) && (index < d->dataVector.count()) )
        return d->dataVector.at(index).data();
    else
        return NULL;
}
