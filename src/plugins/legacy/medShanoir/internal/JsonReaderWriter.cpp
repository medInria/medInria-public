#include <QString>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QJsonValue>


#include "JsonReaderWriter.h"


void JsonReaderWriter::printJsonDocument(QJsonDocument doc)
{
	printJsonByteArray(doc.toJson(QJsonDocument::Indented));
}

void JsonReaderWriter::printJsonByteArray(QByteArray array)
{
	qDebug().noquote() << QString::fromUtf8(array);
}

QJsonDocument JsonReaderWriter::fileToJsonDocument(QString path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
	{
		// Try resolving the path relative to the current working directory
		QString absolutePath = QDir::current().absoluteFilePath(path);
		file.setFileName(absolutePath);
		qDebug() << absolutePath;

		if (!file.open(QIODevice::ReadOnly))
		{
			qDebug() << "Failed to open JSON file:" << file.errorString();
			return QJsonDocument();
		}
	}

	QByteArray jsonData = file.readAll();
	file.close();

	return QJsonDocument::fromJson(jsonData);
}


QJsonObject JsonReaderWriter::qbytearrayToQJson(QByteArray array)
{
	return QJsonDocument::fromJson(array).object();
}

QJsonArray JsonReaderWriter::qbytearrayToQJsonArray(QByteArray array)
{
	return  QJsonDocument::fromJson(array).array();
}

QMap<QString, QString> JsonReaderWriter::jsonObjectToQmap(QJsonObject jsobj)
{
	QMap<QString, QString> qmap;
	for (auto it = jsobj.begin(); it != jsobj.end(); ++it) {
		QString key = it.key();
		QString value = it.value().toString();
		qmap[key] = value;
	}
	return qmap;
}

bool JsonReaderWriter::verifyKeys(const QJsonObject & jsobj, const QStringList & listOfKeys, QStringList listOfTypes)
{
	//TODO: une erreur à cause d'une QList a lieu. A corriger
	// the type will be checked if it has been asked
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

QJsonObject JsonReaderWriter::qByteArrayPairListToJsonObject(const QList<QPair<QByteArray, QByteArray>> list)
{

	QJsonObject jsonobj;
	for (const auto& header : list) {
		const QByteArray& name = header.first;
		const QByteArray& value = header.second;

		jsonobj[QString::fromUtf8(name)] = QString::fromUtf8(value);
	}
	return jsonobj;
}
