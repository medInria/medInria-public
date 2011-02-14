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

QVariant medSettingsManager::value( ESections section, const QString & key, const QVariant & defaultValue /*= QVariant() */ ) 
{
    QString sectionKey;
    sectionKey = sectionEnumToString(section) + "/" + key;
    return d->settings.value(sectionKey, defaultValue);
}

void medSettingsManager::setValue( ESections section, const QString & key, const QVariant & value )
{
    QString sectionKey;
    sectionKey = sectionEnumToString(section) + "/" + key;
    d->settings.setValue(sectionKey, value);
    emit settingsChanged(section);
}

QString medSettingsManager::sectionEnumToString( ESections section )
{
    QString sectionName;
    switch(section)
    {
    case SEC_NONE:
        sectionName = "";
        break;
    case SEC_GENERAL:
        sectionName = "General";
        break;
    case SEC_DICOM:
        sectionName = "Dicom";
        break;
    case SEC_STARTUP:
        sectionName = "Startup";
        break;
    case SEC_SYSTEM:
        sectionName = "System";
        break;
    default:
        sectionName = "";
    }

    return sectionName;
}


medSettingsManager *medSettingsManager::s_instance = NULL;