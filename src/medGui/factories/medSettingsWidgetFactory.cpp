#include "medSettingsWidgetFactory.h"

#include "medSettingsWidget.h"

class medSettingsWidgetFactoryPrivate
{
public:
    medSettingsWidgetFactory::medSettingsWidgetCreatorHash creators;
};

medSettingsWidgetFactory *medSettingsWidgetFactory::instance()
{
    if(!s_instance)
        s_instance = new medSettingsWidgetFactory;

    return s_instance;
}

bool medSettingsWidgetFactory::registerSettingsWidget(const QString& type,
                                                      QString name,
                                                      QString description,
                                                      medSettingsWidgetCreator func)
{
    if(!d->creators.contains(type))
    {
        medSettingDetails* holder = new medSettingDetails(name,
                                                          description,
                                                          func);
        d->creators.insert( type,
                            holder);
        return true;
    }
    return false;
}

QList<QString> medSettingsWidgetFactory::settingsWidgets()
{
    return d->creators.keys();
}

medSettingsWidget *medSettingsWidgetFactory::createSettingsWidget(QString type,QWidget * parent)
{
    if(!d->creators.contains(type))
        return NULL;

    medSettingsWidget *conf = d->creators[type]->creator(parent);

    return conf;
}

medSettingDetails * medSettingsWidgetFactory::settingDetailsFromId(
        const QString &id) const
{
    return d->creators.value(id);
}


medSettingsWidgetFactory::medSettingsWidgetFactory() : dtkAbstractFactory(), d(new medSettingsWidgetFactoryPrivate)
{

}

medSettingsWidgetFactory::~medSettingsWidgetFactory()
{
    delete d;

    d = NULL;
}

medSettingsWidgetFactory *medSettingsWidgetFactory::s_instance = NULL;
