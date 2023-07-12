#pragma once
#include <QJsonArray>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonObject>
#include <QMap>
#include <QStringList>

class JsonReaderWriter
{
public:
	/**
	 * pretty prints a jsondocument
	*/
	static void printJsonDocument(QJsonDocument doc);

	/**
	 * pretty prints a QByteArray.
	 * JSON format
	*/
	static void printJsonByteArray(QByteArray array);

	/**
	 * converts a file from the given path to a JsonDocument object
	*/
	static QJsonDocument fileToJsonDocument(QString path);

	/**
	 * @param array shaped like a json : {"key":"value"}
	 * @return a conversion of array under a jsonobject object
	*/
	static QJsonObject qbytearrayToQJson(QByteArray array);

	/**
	 * @param array shaped like an array of json arrays : {[]}
	 * @return a conversion of array under a jsonarray object
	*/
	static QJsonArray qbytearrayToQJsonArray(QByteArray array);

	/**
	 * converts all the key:value elements of a json object into a map of qstring
	*/
	static QMap<QString, QString> jsonObjectToQmap(QJsonObject jsobj);

	/**
	 * verifies the presence of specific keys inside a json object. 
	 * [optional] : verifies their types. The available types are : "String", "Number", "Bool", "Array"
	*/
	static bool verifyKeys(const QJsonObject& jsobj, const QStringList& listOfKeys, QStringList listOfTypes={});

	static QJsonObject qByteArrayPairListToJsonObject(const QList<QPair<QByteArray, QByteArray>> list);

};

