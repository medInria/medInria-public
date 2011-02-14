#include "medSourceDataPluginFactory.h"
#include "medAbstractSourceDataPlugin.h"

class medSourceDataPluginFactoryPrivate
{
public:
	medSourceDataPluginFactory::medAbstractSourceDataPluginCreatorHash sourcedata_creators;
};

medSourceDataPluginFactory *medSourceDataPluginFactory::instance(void)
{
	if(!s_instance)
		s_instance = new medSourceDataPluginFactory;
	
	return s_instance;
}

bool medSourceDataPluginFactory::registerSourceDataPlugin(QString type, medAbstractSourceDataPluginCreator func)
{
	if(!d->sourcedata_creators.contains(type)) {
		d->sourcedata_creators.insert(type, func);
		return true;
	}
	
	return false;
}

QList<QString> medSourceDataPluginFactory::sourcedata_plugins(void)
{
	return d->sourcedata_creators.keys();
}

medAbstractSourceDataPlugin *medSourceDataPluginFactory::createSourceDataPlugin(QString type)
{
	if(!d->sourcedata_creators.contains(type))
		return NULL;
	
	medAbstractSourceDataPlugin *conf = d->sourcedata_creators[type]();
	
	return conf;
}

medSourceDataPluginFactory::medSourceDataPluginFactory(void) : dtkAbstractFactory(), d(new medSourceDataPluginFactoryPrivate)
{
}

medSourceDataPluginFactory::~medSourceDataPluginFactory(void)
{
	delete d;
	
	d = NULL;
}

medSourceDataPluginFactory *medSourceDataPluginFactory::s_instance = NULL;
