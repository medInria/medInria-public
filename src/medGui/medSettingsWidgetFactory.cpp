#include "medSettingsWidgetFactory.h"

#include "medSettingsWidget.h"
#include "medViewerConfigurationFactory.h"

class medViewerConfigurationFactoryPrivate
{
public:
    medSettingsWidgetFactory::medSettingsWidgetCreatorHash configuration_creators;
};

medSettingsWidgetFactory *medSettingsWidgetFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medSettingsWidgetFactory;

    return s_instance;
}

bool medSettingsWidgetFactory::registerConfiguration(QString type, medSettingsWidgetCreator func)
{
    if(!d->configuration_creators.contains(type)) {
        d->configuration_creators.insert(type, func);
        return true;
    }

    return false;
}

QList<QString> medSettingsWidgetFactory::configurations(void)
{
    return d->configuration_creators.keys();
}

medSettingsWidget *medSettingsWidgetFactory::createConfiguration(QString type)
{
    if(!d->configuration_creators.contains(type))
        return NULL;

    medSettingsWidget *conf = d->configuration_creators[type]();

    return conf;
}

medSettingsWidgetFactory::medSettingsWidgetFactory(void) : dtkAbstractFactory(), d(new medSettingsWidgetFactoryPrivate)
{

}

medSettingsWidgetFactory::~medSettingsWidgetFactory(void)
{
    delete d;

    d = NULL;
}

medSettingsWidgetFactory *medSettingsWidgetFactory::s_instance = NULL;
