#include "medSettingsWidgetFactory.h"

#include "medSettingsWidget.h"

class medSettingsWidgetFactoryPrivate
{
public:
    medSettingsWidgetFactory::medSettingsWidgetCreatorHash settingsWidget_creators;
};

medSettingsWidgetFactory *medSettingsWidgetFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medSettingsWidgetFactory;

    return s_instance;
}

bool medSettingsWidgetFactory::registerSettingsWidget(const QString& type, medSettingsWidgetCreator func)
{
    if(!d->settingsWidget_creators.contains(type)) {
        d->settingsWidget_creators.insert(type, func);
        return true;
    }
    qWarning() << "failed registering" << type;

    return false;
}

QList<QString> medSettingsWidgetFactory::settingsWidgets(void)
{
    return d->settingsWidget_creators.keys();
}

medSettingsWidget *medSettingsWidgetFactory::createSettingsWidget(QString type,QWidget * parent)
{
    if(!d->settingsWidget_creators.contains(type))
        return NULL;

    medSettingsWidget *conf = d->settingsWidget_creators[type](parent);

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
