#include "dtkCompositeDataSet.h"

#include <dtkCore/dtkAbstractDataFactory.h>

class dtkCompositeDataSetPrivate
{
public:
  // ??? data
  void* data;
  QList<QImage> thumbnails;
};

dtkCompositeDataSet::dtkCompositeDataSet(): dtkAbstractData(), d (new dtkCompositeDataSetPrivate)
{
  // d->data = 0;
}
dtkCompositeDataSet::~dtkCompositeDataSet()
{
  delete d;
  d = 0;
}

bool dtkCompositeDataSet::registered()				
{
  return dtkAbstractDataFactory::instance()->registerDataType("dtkCompositeDataSet", createDtkCompositeDataSet);
}

QString dtkCompositeDataSet::description() const
{
  return "dtkCompositeDataSet";
}

void dtkCompositeDataSet::setData(void *data)			
{
  // d->data = data;
}

void *dtkCompositeDataSet::output(void)				
{
  return d->data;
}

void *dtkCompositeDataSet::data(void)
{
  return d->data;
}

void dtkCompositeDataSet::update(void)				
{

}

dtkAbstractData *createDtkCompositeDataSet(void)
{
  return new dtkCompositeDataSet;
}

QImage & dtkCompositeDataSet::thumbnail (void) const
{
  if (!d->thumbnails.size())
    return dtkAbstractData::thumbnail();
  
  return (d->thumbnails[0]);
}

QList<QImage> & dtkCompositeDataSet::thumbnails (void) const
{
  d->thumbnails.clear();
  return d->thumbnails;
}
