#ifndef JSON_HELPER
#define JSON_HELPER

#include <QJsonObject>
#include <QStringList>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonArray>

/**
 * @brief this file contains helper functions to deal with json
 * (conversion, verification, etc.)
*/


/**
 * @param jsobj the json object to verify
 * @param listOfKeys the list of keys to verify
 * @param listOfTypes the list of types to verify (String, Number, Bool, Array)
 * @return true if the json object contains all the keys and the types are correct, false otherwise
 * @note if listOfTypes hasn't the same size as listOfKeys, the verification of the types is not done
*/
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

///////////////////////////
// Conversion functions
///////////////////////////

/**
 * @param list a list of QByteArray pairs of entry-value
 * @return the QJsonObject corresponding to the list of the form {"entry1":"value1", "entry2":"value2", ...}
*/
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

/**
 * @param jsobj a json object
 * @return a string-string map corresponding to the json object
*/
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

/**
 * @param array a byte array containing a json object
 * @return the json object corresponding to the byte array
*/
inline QJsonObject qbytearrayToQJson(QByteArray array)
{
	return QJsonDocument::fromJson(array).object();
}

/**
 * @param array a byte array containing a json array
 * @return the json array corresponding to the byte array
*/
inline QJsonArray qbytearrayToQJsonArray(QByteArray array)
{
	return  QJsonDocument::fromJson(array).array();
}

#endif