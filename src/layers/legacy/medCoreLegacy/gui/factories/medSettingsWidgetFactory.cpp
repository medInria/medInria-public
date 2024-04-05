/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
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
    medSettingsWidgetFactory::medSettingsWidgetCreatorList creators;
};

/**
 * @brief Gets an instance of the factory.
 *
 * @param void
 * @return medSettingsWidgetFactory * Factory instance
*/
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
    bool typeFound = false;

    foreach(medSettingDetails* creatorDetails, d->creators)
    {
        if(creatorDetails->type == type)
        {
            typeFound = true;
            break;
        }
    }

    if(!typeFound)
    {
        medSettingDetails* holder = new medSettingDetails(type,
                                                          name,
                                                          description,
                                                          func);
        d->creators.append(holder);
        return true;
    }

    return false;
}

/**
 * @brief Gets a list of registered type names.
 *
 * @return QList<QString>
*/
QList<QString> medSettingsWidgetFactory::settingsWidgets(void)
{
    QList<QString> typeNames;

    foreach(medSettingDetails* creatorDetails, d->creators)
    {
        typeNames.append(creatorDetails->type);
    }

    return typeNames;
}

/**
 * @brief Creates a new widget.
 *
 * @param type the type to instanciate
 * @return medSettingsWidget * the newly allocated widget.
*/
medSettingsWidget *medSettingsWidgetFactory::createSettingsWidget(QString type,QWidget * parent)
{
    foreach(medSettingDetails* creatorDetails, d->creators)
    {
        if(creatorDetails->type == type)
        {
            medSettingsWidget* widget = creatorDetails->creator(parent);
            return widget;
        }
    }

    return nullptr;
}

/**
 * @brief Gets the name, description and creators
 * for the given settings.
 */
medSettingDetails * medSettingsWidgetFactory::settingDetailsFromId(
        const QString &id) const
{
    foreach(medSettingDetails* creatorDetails, d->creators)
    {
        if(creatorDetails->type == id)
        {
            return creatorDetails;
        }
    }

    return nullptr;
}

/**
 * @brief Constructor, not to be used by users.
 *
 * Use the instance() method instead to get a singleton.
 *
 * @param void
*/
medSettingsWidgetFactory::medSettingsWidgetFactory(void) : dtkAbstractFactory(), d(new medSettingsWidgetFactoryPrivate)
{

}

/**
 * @brief Destructor, not to be used by users.
 *
 * The singleton will be deleted at appplication closing time.
 *
 * @param void
*/
medSettingsWidgetFactory::~medSettingsWidgetFactory(void)
{
    qDeleteAll(d->creators);
    d->creators.clear();
    delete d;

    d = nullptr;
}

medSettingsWidgetFactory *medSettingsWidgetFactory::s_instance = nullptr;
