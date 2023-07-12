#pragma once
#include <QSettings>
#include <QJsonObject>

class SettingsManager
{
private:
	static QSettings& getPluginSettings();

public:
	static void store(QString key,QString value);

	static QString retrieve(QString key);

	static void reset(QString key);

	static void storeJson(QString key, QJsonObject value);

	static QJsonObject retrieveJson(QString key);
};

