#include "medViewerConfigurationFactory.h"


#include "medViewerConfiguration.h"
#include "medViewerConfigurationFactory.h"

class medViewerConfigurationFactoryPrivate
{
public:
    typedef QHash<QString, medViewerConfigurationDetails*> medViewerConfigurationCreatorHash;
    medViewerConfigurationCreatorHash creators;
};

medViewerConfigurationFactory *medViewerConfigurationFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medViewerConfigurationFactory;

    return s_instance;
}

bool medViewerConfigurationFactory::registerConfiguration(QString identifier,
                                                          QString name,
                                                          QString description,
                                                          medViewerConfigurationCreator creator)
{

    if(!d->creators.contains(identifier))
    {
        medViewerConfigurationDetails* holder = new medViewerConfigurationDetails
                (name,
                 description,
                 creator);
        d->creators.insert( identifier,
                            holder);
        return true;
    }
    return false;
}

QList<QString> medViewerConfigurationFactory::configurations(void)
{
    return d->creators.keys();
}

medViewerConfiguration *medViewerConfigurationFactory::createConfiguration(QString type,QWidget* parent)
{
    if(!d->creators.contains(type))
        return NULL;

    medViewerConfiguration *conf = d->creators[type]->creator(parent);

    return conf;
}

QHash<QString, medViewerConfigurationDetails *> medViewerConfigurationFactory::configurationDetails() const
{
    return d->creators;
}

medViewerConfigurationFactory::medViewerConfigurationFactory(void) : dtkAbstractFactory(), d(new medViewerConfigurationFactoryPrivate)
{

}

medViewerConfigurationFactory::~medViewerConfigurationFactory(void)
{
    foreach (medViewerConfigurationDetails * detail, d->creators.values())
    {
        delete detail;
        detail = NULL;
    }

    delete d;

    d = NULL;
}


medViewerConfigurationDetails * medViewerConfigurationFactory::configurationDetailsFromId(QString identifier) const
{
    return d->creators.value(identifier);
}

medViewerConfigurationFactory *medViewerConfigurationFactory::s_instance = NULL;

