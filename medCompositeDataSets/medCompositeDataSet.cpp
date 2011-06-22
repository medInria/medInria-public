#include "medCompositeDataSet.h"

#include <dtkCore/dtkAbstractDataFactory.h>

class medCompositeDataSetPrivate
{
public:
  // ??? data
  void* data;
  QList<QImage> thumbnails;
};

medCompositeDataSet::medCompositeDataSet(): dtkAbstractData(), d (new medCompositeDataSetPrivate)
{
  // d->data = 0;
}
medCompositeDataSet::~medCompositeDataSet()
{
  delete d;
  d = 0;
}

bool medCompositeDataSet::registered()				
{
  return dtkAbstractDataFactory::instance()->registerDataType("medCompositeDataSet", createDtkCompositeDataSet);
}

QString medCompositeDataSet::description() const
{
  return "medCompositeDataSet";
}

void medCompositeDataSet::setData(void *data)			
{
  // d->data = data;
}

void *medCompositeDataSet::output(void)				
{
  return d->data;
}

void *medCompositeDataSet::data(void)
{
  return d->data;
}

void medCompositeDataSet::update(void)				
{

}

dtkAbstractData *createDtkCompositeDataSet(void)
{
  return new medCompositeDataSet;
}

QImage & medCompositeDataSet::thumbnail (void) const
{
  if (!d->thumbnails.size())
    return dtkAbstractData::thumbnail();
  
  return (d->thumbnails[0]);
}

QList<QImage> & medCompositeDataSet::thumbnails (void) const
{
  d->thumbnails.clear();
  return d->thumbnails;
}
