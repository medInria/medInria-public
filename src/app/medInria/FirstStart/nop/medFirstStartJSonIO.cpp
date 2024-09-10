/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medFirstStartJSonIO.h>

#include <QFile>

#include <QMap>
#include <QPair>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>

medFirstStartJSonIO::medFirstStartJSonIO(QString path) : m_confPath(path)
{
}

medFirstStartJSonIO::~medFirstStartJSonIO()
{
}

bool medFirstStartJSonIO::readJsonUpdatedFile()
{
    bool bRes = true;

    QFile configFile;
    QString content;

    configFile.setFileName(m_confPath);
    bRes = configFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (bRes)
    {
        content = configFile.readAll();
        configFile.close();

        QJsonDocument jsonSaveDoc = QJsonDocument::fromJson(content.toUtf8());
        QJsonArray entries = jsonSaveDoc.array();
        for (auto entry : entries)
        {
            auto obj = entry.toObject();

            if (obj.contains("file") && obj["file"].isString() && !obj["file"].toString().isEmpty() &&
                obj.contains("url") && obj["url"].isString() && !obj["url"].toString().isEmpty() &&
                obj.contains("destPath") && obj["destPath"].isString() && !obj["destPath"].toString().isEmpty())
            {
                QString file = obj["file"].toString();
                QString url = obj["url"].toString();
                QString dst = obj["destPath"].toString();

                m_registerdMap[dst] = QPair<QString, QUrl>(m_baseUpdatedPath + file, QUrl(url));
            }
            else
            {
                qWarning() << "[WARN] First Start detects malformed json";
            }
        }
    }

    return bRes;
}

bool medFirstStartJSonIO::writeJsonUpdatedFile()
{
    bool bRes = true;

    QJsonDocument jsonSaveDoc;
    QJsonArray entries;
    for (auto key : m_updatedMap.keys())
    {
        QJsonObject entry;
        entry.insert("file", QJsonValue::fromVariant(key));
        entry.insert("url", QJsonValue::fromVariant(m_updatedMap[key].first));
        entry.insert("destPath", QJsonValue::fromVariant(m_updatedMap[key].second.toString()));
        entries.push_back(entry);
    }


    jsonSaveDoc.setArray(entries);

    QByteArray payload = jsonSaveDoc.toJson();

    QFile configFile(m_confPath);
    bRes = configFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    if (bRes)
    {
        bRes = configFile.write(payload) != -1;
        configFile.close();
    }

    return bRes;
}
