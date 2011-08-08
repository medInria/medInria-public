#include "medAbstractViewCollection.h"

class medAbstractViewCollectionPrivate
{
public:
};

medAbstractViewCollection::medAbstractViewCollection(medAbstractViewCollection *parent)
  : medAbstractView (parent), d(new medAbstractViewCollectionPrivate)
{
}

medAbstractViewCollection::medAbstractViewCollection(const medAbstractViewCollection &view)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

medAbstractViewCollection::~medAbstractViewCollection(void)
{
    delete d;
    d = NULL;
}

QList <medAbstractView *> medAbstractViewCollection::views (void) const
{
    return QList <medAbstractView *> ();
}