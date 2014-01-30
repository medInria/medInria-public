/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSettingsWidgetFactory.h>

#include <medSettingsWidget.h>

class medSettingsWidgetFactoryPrivate
{
public:
    medSettingsWidgetFactory::medSettingsWidgetCreatorHash creators;
};

medSettingsWidgetFactory *medSettingsWidgetFactory::instance(void)
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

QList<QString> medSettingsWidgetFactory::settingsWidgets(void)
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


medSettingsWidgetFactory::medSettingsWidgetFactory(void) : dtkAbstractFactory(), d(new medSettingsWidgetFactoryPrivate)
{

}

medSettingsWidgetFactory::~medSettingsWidgetFactory(void)
{
    qDeleteAll(d->creators);
    d->creators.clear();
    delete d;

    d = NULL;
}

medSettingsWidgetFactory *medSettingsWidgetFactory::s_instance = NULL;
