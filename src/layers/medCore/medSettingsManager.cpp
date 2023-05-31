/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSettingsManager.h>

#include <QSettings>
#include <dtkLog>

class medSettingsManagerPrivate
{
public:
  medSettingsManagerPrivate()
  {
    settings = new QSettings();
    tmpFile.open();
    tmpSettings = new QSettings(tmpFile.fileName(), QSettings::IniFormat);
  }

  ~medSettingsManagerPrivate()
  {
    delete settings;
    delete tmpSettings;
  }

  QSettings *settings;
  QSettings *tmpSettings;
  QTemporaryFile tmpFile;
};

std::unique_ptr<medSettingsManager> medSettingsManager::s_instance = nullptr;

/**
 * instance - singleton access method, returns a singleViewContainer static
 * instance of the manager
 * @return medSettingsManager & - the manager
 */
medSettingsManager &medSettingsManager::instance()
{
    if(!s_instance)
    {
        s_instance = std::unique_ptr<medSettingsManager>(new medSettingsManager());
    }
    return *s_instance.get();
}

medSettingsManager::medSettingsManager(void)
    : d(new medSettingsManagerPrivate) {}

medSettingsManager::~medSettingsManager()
{
  delete d;
  d = nullptr;
}

/**
 * value
 * @param: const QString & section
 * @param: const QString & key
 * @param: const QVariant & defaultValue
 * @return   QT_NAMESPACE::QVariant
 */
QVariant medSettingsManager::value(
    const QString &section, const QString &key,
    const QVariant &defaultValue /*= QVariant() */,
    bool permanentValue /*= true */)
{
  bool valuePresent = false;

  QString sectionKey;
  sectionKey = section + "/" + key;

  QSettings *s = (permanentValue) ? d->settings : d->tmpSettings;
  s->contains(sectionKey);

  // don't write in case of no defaultValue
  if (!defaultValue.isValid())
    valuePresent = true;

  QVariant result = s->value(sectionKey, defaultValue);

  // write default value so we can share it
  if (!valuePresent)
  {
    qDebug() << "writing default value for section: " << section
             << " key: " << key << "value: " << defaultValue;
    setValue(section, key, result);
  }

  return result;
}

/**
 * keys
 * @param: const QString & section
 * @return   QStringList
 */
QStringList medSettingsManager::keys(const QString &section,
                                     bool permanentValue /*= true */)
{
  QSettings *s = (permanentValue) ? d->settings : d->tmpSettings;
  s->beginGroup(section);
  QStringList keys = d->settings->childKeys();
  s->endGroup();

  return keys;
}

/**
 * setValue
 * @param: const QString & key
 * @param: const QVariant & value
 * @return   void
 */
void medSettingsManager::setValue(const QString &section, const QString &key,
                                  const QVariant &value,
                                  bool permanentValue /*= true */)
{
  QSettings *s = (permanentValue) ? d->settings : d->tmpSettings;
  QString sectionKey;
  sectionKey = section + "/" + key;
  s->setValue(sectionKey, value);
  emit settingsChanged(section);
}

/**
 * remove
 * @param: const QString & key
 * @return   void
 */
void medSettingsManager::remove(const QString &section, const QString &key,
                                bool permanentValue /*= true */)
{
  QSettings *s = (permanentValue) ? d->settings : d->tmpSettings;
  s->beginGroup(section);
  s->remove(key);
  s->endGroup();
}
