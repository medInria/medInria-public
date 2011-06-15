#include "medViewerConfigurationFactory.h"


#include "medViewerConfiguration.h"
#include "medViewerConfigurationFactory.h"

class medViewerConfigurationFactoryPrivate
{
public:
    medViewerConfigurationFactory::medViewerConfigurationCreatorHash configuration_creators;
};

medViewerConfigurationFactory *medViewerConfigurationFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medViewerConfigurationFactory;

    return s_instance;
}

bool medViewerConfigurationFactory::registerConfiguration(QString type, medViewerConfigurationCreator func)
{
    if(!d->configuration_creators.contains(type)) {
        d->configuration_creators.insert(type, func);
        return true;
    }

    return false;
}

QList<QString> medViewerConfigurationFactory::configurations(void)
{
    return d->configuration_creators.keys();
}

medViewerConfiguration *medViewerConfigurationFactory::createConfiguration(QString type)
{
    if(!d->configuration_creators.contains(type))
        return NULL;

    medViewerConfiguration *conf = d->configuration_creators[type]();

    return conf;
}

medViewerConfigurationFactory::medViewerConfigurationFactory(void) : dtkAbstractFactory(), d(new medViewerConfigurationFactoryPrivate)
{

}

medViewerConfigurationFactory::~medViewerConfigurationFactory(void)
{
    delete d;

    d = NULL;
}

medViewerConfigurationFactory *medViewerConfigurationFactory::s_instance = NULL;
