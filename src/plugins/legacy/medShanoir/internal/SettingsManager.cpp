#include <QJsonDocument>
#include "SettingsManager.h"

QSettings & SettingsManager::getPluginSettings()
{
	static QSettings plugin_settings("Medinria", "ShanoirPlugin");
	return plugin_settings;
}

void SettingsManager::store(QString key, QString value)
{
	QSettings& settings = SettingsManager::getPluginSettings();
	settings.setValue(key, value);
}

QString SettingsManager::retrieve(QString key)
{
	return SettingsManager::getPluginSettings().value(key).toString();
}

void SettingsManager::reset(QString key)
{
	getPluginSettings().remove(key);
}

void SettingsManager::storeJson(QString key, QJsonObject value)
{
	store(key,QString(QJsonDocument(value).toJson()));
}

QJsonObject SettingsManager::retrieveJson(QString key)
{
	return QJsonDocument::fromJson(retrieve(key).toUtf8()).object();
}

