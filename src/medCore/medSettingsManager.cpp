/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medSettingsManager.h"

#include <QtCore/QSettings>

class medSettingsManagerPrivate
{
public:
    QSettings settings;
};

medSettingsManager * medSettingsManager::instance( void )
{
    if(!s_instance)
        s_instance = new medSettingsManager;
    return s_instance;
}

medSettingsManager::medSettingsManager( void ): d(new medSettingsManagerPrivate)
{

}

medSettingsManager::~medSettingsManager( void )
{
    delete d;
    d = NULL;
}

void medSettingsManager::destroy( void )
{
    if (s_instance) {
        delete s_instance;
        s_instance = 0;
    }
}

QVariant medSettingsManager::value( const QString & section, const QString & key, const QVariant & defaultValue /*= QVariant() */ )
{
    bool valuePresent;

    QString sectionKey;
    sectionKey = section + "/" + key;
    valuePresent = d->settings.contains(sectionKey);

    // don't write in case of no defaultValue
    if (!defaultValue.isValid())
        valuePresent = true;

    QVariant result = d->settings.value(sectionKey, defaultValue);

    // write default value so we can share it
    if (!valuePresent)
    {
        qDebug() << "writing default value for section: " << section << " key: " << key << "value: " << defaultValue;
        setValue(section, key, result);
    }

    return result;
}

void medSettingsManager::setValue( const QString & section, const QString & key, const QVariant & value )
{
    QString sectionKey;
    sectionKey = section + "/" + key;
    d->settings.setValue(sectionKey, value);
    emit settingsChanged(section);
}


medSettingsManager *medSettingsManager::s_instance = NULL;
