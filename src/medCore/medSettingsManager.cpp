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
    QString sectionKey;
    sectionKey = section + "/" + key;
    return d->settings.value(sectionKey, defaultValue);
}

void medSettingsManager::setValue( const QString & section, const QString & key, const QVariant & value )
{
    QString sectionKey;
    sectionKey = section + "/" + key;
    d->settings.setValue(sectionKey, value);
    emit settingsChanged(section);
}


medSettingsManager *medSettingsManager::s_instance = NULL;
