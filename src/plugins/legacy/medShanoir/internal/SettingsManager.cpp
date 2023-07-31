#include <QJsonDocument>
#include "SettingsManager.h"



SettingsManager::SettingsManager()
{
}


void SettingsManager::setInstanceName(QString instanceName)
{
	m_instanceName = instanceName;
}


void SettingsManager::store(QString key, QString value)
{
	QSettings("Medinria", "ShanoirPlugin_" + m_instanceName).setValue(key, value);
}

QString SettingsManager::retrieve(QString key)
{
	QSettings &setting = QSettings("Medinria", "ShanoirPlugin_" + m_instanceName);
	if (setting.status() == QSettings::NoError)
	{
		return setting.value(key).toString();
	}
	return "";
}

void SettingsManager::reset(QString key)
{
	QSettings &setting = QSettings("Medinria", "ShanoirPlugin_" + m_instanceName);
	if (setting.status() == QSettings::NoError)
	{
		setting.remove(key);
	}
}

void SettingsManager::storeJson(QString key, QJsonObject value)
{
	store(key,QString(QJsonDocument(value).toJson()));
}

QJsonObject SettingsManager::retrieveJson(QString key)
{
	return QJsonDocument::fromJson(retrieve(key).toUtf8()).object();
}

