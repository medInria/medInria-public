#pragma once
#include <QSettings>
#include <QJsonObject>
#include <QString>

class SettingsManager
{
private:

	QString m_instanceName;


public:

	SettingsManager();

	void store(QString key,QString value);

	QString retrieve(QString key);

	void reset(QString key);

	void storeJson(QString key, QJsonObject value);

	QJsonObject retrieveJson(QString key);

	void setInstanceName(QString instanceName);
};

