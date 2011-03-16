#include "medAbstractDataSourceFactory.h"
#include "medAbstractDataSource.h"

class medAbstractDataSourceFactoryPrivate
{
public:
	medAbstractDataSourceFactory::medAbstractDataSourceCreatorHash sourcedata_creators;
};

medAbstractDataSourceFactory *medAbstractDataSourceFactory::instance(void)
{
	if(!s_instance)
		s_instance = new medAbstractDataSourceFactory;
	
	return s_instance;
}

bool medAbstractDataSourceFactory::registerDataSource(QString type, medAbstractDataSourceCreator func)
{
	if(!d->sourcedata_creators.contains(type)) {
		d->sourcedata_creators.insert(type, func);
		return true;
	}
	
	return false;
}

QList<QString> medAbstractDataSourceFactory::sourcedata_plugins(void)
{
	return d->sourcedata_creators.keys();
}

medAbstractDataSource *medAbstractDataSourceFactory::create(QString type)
{
	if(!d->sourcedata_creators.contains(type))
		return NULL;
	
	medAbstractDataSource *conf = d->sourcedata_creators[type]();
	
	return conf;
}

medAbstractDataSourceFactory::medAbstractDataSourceFactory(void) : dtkAbstractFactory(), d(new medAbstractDataSourceFactoryPrivate)
{
}

medAbstractDataSourceFactory::~medAbstractDataSourceFactory(void)
{
	delete d;
	
	d = NULL;
}

medAbstractDataSourceFactory *medAbstractDataSourceFactory::s_instance = NULL;
