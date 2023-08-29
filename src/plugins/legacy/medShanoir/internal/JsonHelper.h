#ifndef JSON_HELPER
#define JSON_HELPER

#include <QJsonObject>
#include <QStringList>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonArray>

inline bool verifyJsonKeys(const QJsonObject & jsobj, const QStringList & listOfKeys, QStringList listOfTypes = {})
{
    bool type_verification = listOfKeys.size()==listOfTypes.size();
	for (int i=0; i<listOfKeys.size(); i++) 
	{
		const QString& key = listOfKeys.at(i);
		auto key_it = jsobj.find(key);
		if(key_it!=jsobj.end())
		{
			QJsonValue value = *key_it;
			if(type_verification)
			{
				QString type = listOfTypes.at(i);
				if(
					(type == "String" && !value.isString()) ||
					(type == "Number" && !value.isDouble()) ||
					(type == "Bool"   && !value.isBool())   ||
					(type == "Array"  && !value.isArray()) 
				)
				{
					return false; // The type announced is not the type received
				}
			}
		}
		else {
			return false; // the key has not been found
		}
	}
	return true; 
}

inline QJsonObject qByteArrayPairListToJsonObject(const QList<QPair<QByteArray, QByteArray>> list)
{
    QJsonObject jsonobj;
	for (const auto& header : list) {
		const QByteArray& name = header.first;
		const QByteArray& value = header.second;

		jsonobj[QString::fromUtf8(name)] = QString::fromUtf8(value);
	}
	return jsonobj;
}

inline QMap<QString, QString> jsonObjectToQmap(QJsonObject jsobj)
{
	QMap<QString, QString> qmap;
	for (auto it = jsobj.begin(); it != jsobj.end(); ++it) {
		QString key = it.key();
		QString value = it.value().toString();
		qmap[key] = value;
	}
	return qmap;
}

inline QJsonObject qbytearrayToQJson(QByteArray array)
{
	return QJsonDocument::fromJson(array).object();
}

inline QJsonArray qbytearrayToQJsonArray(QByteArray array)
{
	return  QJsonDocument::fromJson(array).array();
}

#endif