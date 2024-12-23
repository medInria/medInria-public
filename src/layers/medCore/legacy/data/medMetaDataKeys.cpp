/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMetaDataKeys.h>

#include <medCoreExport.h>

#define MED_METADATAKEYDICT_DIRNAME "keysDictionnaries"

#define CHAPTER_NAME     "chapterName"
#define CHAPTER_KEYS     "keys"
#define KEY_NAME         "name"
#define KEY_LABEL        "label"
#define KEY_TAG          "tag"
#define KEY_MEDKEY_PIVOT "medKey"
#define KEY_TYPE         "type"

#include <array>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

medMetaDataKeys *medMetaDataKeys::s_instance = nullptr;


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////    Static public part    ///////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
medMetaDataKeys *medMetaDataKeys::instance()
{
    if (!s_instance)
    {
        s_instance = new medMetaDataKeys();
        s_instance->m_path = medMetaDataKeys::path();
        s_instance->fetchChapterDirectory(s_instance->m_path);
    }
    return s_instance;
}
    

bool medMetaDataKeys::registerKey(Key2 key, QString chapter)
{
    return medMetaDataKeys::instance()->registerKeyInternal(key, chapter);
}

bool medMetaDataKeys::addKeyToChapter(Key2 key, QString chapter)
{
    return medMetaDataKeys::instance()->addKeyToChapterInternal(key, chapter);
}

bool medMetaDataKeys::addKeyByTagToChapter(QString tag, QString keyLabel, QString keyName, QString chapter)
{
    if (keyName.isEmpty())
    {
        keyName = tag;
    }
    else
    {
        if (keyLabel.isEmpty()) keyLabel = keyName;
    }

    Key2 key(keyName, keyLabel, tag);
    return medMetaDataKeys::instance()->addKeyToChapterInternal(key, chapter);

}

Key2 medMetaDataKeys::key(QString word)
{
    return medMetaDataKeys::instance()->keyInternal(word);
}

Key2 medMetaDataKeys::keyFromPivot(QString pivot)
{
    return medMetaDataKeys::instance()->keyFromPivotInternal(pivot);
}

Key2 medMetaDataKeys::keyFromName(QString keyName, QString chapter)
{
    return medMetaDataKeys::instance()->keyFromNameInternal(keyName, chapter);
}

Key2 medMetaDataKeys::keyFromTag(QString keyTag, QString chapter)
{
    return medMetaDataKeys::instance()->keyFromTagInternal(keyTag, chapter);
}

QString medMetaDataKeys::pivot(QString keyName, QString chapter)
{
    return medMetaDataKeys::instance()->pivotInternal(keyName, chapter);
}

bool medMetaDataKeys::keyExist(Key2 const & key)
{
    return medMetaDataKeys::instance()->keyExistInternal(key);
}

QString medMetaDataKeys::getValue(Key2 &key, QMap<QString, QString> metaDataList)
{
    QString valRes;

    auto keys = metaDataList.keys();

    if (keys.contains(key.name()))
    {
        valRes = metaDataList[key.name()];
    }
    else if (keys.contains(key.medPivot()))
    {
        valRes = metaDataList[key.medPivot()];
    }
    else if(keys.contains(key.tag()))
    {
        valRes = metaDataList[key.tag()];
    }

    return valRes;
}

QString medMetaDataKeys::path()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName() + "/" + MED_METADATAKEYDICT_DIRNAME;
}



// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////    Private part    /////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
medMetaDataKeys::medMetaDataKeys()
{
    m_saveScheduler.setSingleShot(true);
    m_saveScheduler.callOnTimeout(this, &medMetaDataKeys::delegateWriting);
}

// ////////////////////////////////////////////////////////////////////////////
// read part from file system
bool medMetaDataKeys::fetchChapterDirectory(QString path)
{
    bool bRes = false;
    qDebug() << "medMetaDataKeys: Begin parsing metadata key directory to build a dictionary into : " << path;

    QDirIterator it(path, QStringList() << "*.json", QDir::Files);
    while (it.hasNext())
    {
        QString filePath = it.next();
        QFile file(filePath);
        file.open(QFile::ReadOnly);
        QString chapterName = loadChapter(file.readAll());
        if (!chapterName.isEmpty())
        {
            m_chapterToFileMap[chapterName] << filePath;
        }
        else
        {
            qDebug() << "File : " << filePath << " is a malformed json key file";
        }
    }

    qDebug() << "medMetaDataKeys: Parsing ended into : " << path;
    return bRes;
}

QString medMetaDataKeys::loadChapter(QByteArray chapter)
{
    bool bRes = false;
    QString chapterRes;

    QJsonDocument chapterDoc = QJsonDocument::fromJson(chapter);
    if (chapterDoc.isObject())
    {
        QJsonObject chapterObj = chapterDoc.object();
        if (chapterObj.contains(CHAPTER_NAME) && chapterObj[CHAPTER_NAME].isString() &&
            chapterObj.contains(CHAPTER_KEYS) && chapterObj[CHAPTER_KEYS].isArray())
        {
            QString chapterName = chapterObj[CHAPTER_NAME].toString().trimmed();
            chapterRes = chapterName;
            QJsonArray keys = chapterObj[CHAPTER_KEYS].toArray();
            if (!chapterName.isEmpty() && !keys.isEmpty())
            {
                for (auto entry : keys)
                {
                    Key2 keyOut;
                    if (entry.isObject())
                    {
                        QJsonObject key = entry.toObject();
                        if (readKey(key, keyOut))
                        {
                            bRes = true;
                        }
                        else
                        {
                            qDebug() << "Unable to read a key in " << chapterName;
                        }

                        registerKey(keyOut, chapterName);
                    }
                }
            }
            else
            {
                qDebug() << "Both, " << CHAPTER_KEYS << " and " << CHAPTER_NAME << " must be not empty values " << chapterName;
            }
        }
        else
        {
            qDebug() << "Both, " << CHAPTER_KEYS << " and " << CHAPTER_NAME << "are mandatories keys";
        }
    }

    return bRes ? chapterRes : "";
}


bool medMetaDataKeys::readKey(QJsonObject keyAsJson, Key2 & key)
{
    bool bRes = false;

    QString name;
    QString label;
    QString tag;
    QString medKey;
    QVariant::Type type = QVariant::Type::Invalid;

    if (keyAsJson.contains(KEY_NAME) && keyAsJson[KEY_NAME].isString())
    {
        name = keyAsJson[KEY_NAME].toString();
        bRes = !name.trimmed().isEmpty();

        if (bRes)
        {
            if (keyAsJson.contains(KEY_LABEL) && keyAsJson[KEY_LABEL].isString())
            {
                label = keyAsJson[KEY_LABEL].toString().trimmed();
            }
            if (keyAsJson.contains(KEY_TAG) && keyAsJson[KEY_TAG].isString())
            {
                tag = keyAsJson[KEY_TAG].toString().trimmed();
            }
            if (keyAsJson.contains(KEY_MEDKEY_PIVOT) && keyAsJson[KEY_MEDKEY_PIVOT].isString())
            {
                medKey = keyAsJson[KEY_MEDKEY_PIVOT].toString().trimmed();
            }
            if (keyAsJson.contains(KEY_TYPE) && keyAsJson[KEY_TYPE].isString())
            {
                type = QVariant::nameToType(keyAsJson[KEY_TYPE].toString().toUtf8().data());
            }

            key = Key2(name, label, tag, medKey, type);
        }
        else
        {
            qDebug() << "Key has invalid format. Name must be a non empty string.";
        }

    }
    else
    {
        qDebug() << "Key has invalid format. Name is mandatory.";
    }

    return bRes;
}


// write part to file system
bool medMetaDataKeys::updateChapterDirectory(QString path)
{
    bool bRes = true;


    for (QString const & chapter : m_chaptersToUpdate)
    {
        QString filePath;
        if (m_chapterToFileMap.contains(chapter) && m_chapterToFileMap[chapter].back().startsWith(path))
        {
            filePath = m_chapterToFileMap[chapter].back();
        }
        else
        {
            filePath = path + '/' + chapter + ".json";
            m_chapterToFileMap[chapter].push_back(filePath);
        }
        QJsonDocument chapterJson;
        if (writeChapter(chapter, chapterJson))
        {
            QFile file(filePath);
            if (file.open(QFile::ReadWrite))
            {
                bRes = bRes && file.write(chapterJson.toJson()) != -1;
            }
            else
            {
                qDebug() << "meta data system can't create or update " << filePath;
            }
        }
    }

    return bRes;
}

bool medMetaDataKeys::writeChapter(QString chapterName, QJsonDocument &chapter)
{
    bool bRes = false;


    if (m_medKeyByChapterMap.contains(chapterName))
    {
        bRes = true;
        QJsonArray jsonKeys;
        for (auto & key : *m_medKeyByChapterMap[chapterName])
        {
            QJsonObject keyJson;
            if (writeKey(key, keyJson))
            {
                jsonKeys.push_back(keyJson);
            }
            else
            {
                qDebug() << "An invalid key is found in chapter " << chapterName;
            }
        }

        QJsonObject chapterObject;
        chapterObject.insert(CHAPTER_NAME, chapterName);
        chapterObject.insert(CHAPTER_KEYS, jsonKeys);
        chapter.setObject(chapterObject);
    }

    return bRes;
}

bool medMetaDataKeys::writeKey(Key2 const & key, QJsonObject & keyAsJson)
{
    bool bRes = true;

    if (!key.name().isEmpty())
    {
        keyAsJson.insert(KEY_NAME, key.name().trimmed());
        if (key.label().trimmed().isEmpty())
        {
            keyAsJson.insert(KEY_LABEL, key.name().trimmed());
        }
        else
        {
            keyAsJson.insert(KEY_LABEL, key.label().trimmed());
        }
        if (!key.tag().trimmed().isEmpty())
        {
            keyAsJson.insert(KEY_TAG, key.tag().trimmed());
        }
        if (key.medPivot().trimmed().isEmpty())
        {
            keyAsJson.insert(KEY_MEDKEY_PIVOT, key.name().trimmed().toLower());
        }
        else
        {
            keyAsJson.insert(KEY_MEDKEY_PIVOT, key.medPivot().trimmed().toLower());
        }
    }
    else
    {
        bRes = false;
        qDebug() << "Try to save an invalid key.";
    }

    return bRes;
}




bool medMetaDataKeys::registerKeyInternal(Key2 &key, QString &chapter)
{
    bool bRes = false;
    if (m_medKeyByChapterMap[chapter] == nullptr) m_medKeyByChapterMap[chapter] = new QList<Key2>();
    if (!m_medKeyByChapterMap[chapter]->contains(key.name()))
    {

        if (key.medPivot() == "")
        {
            // We try to find an other similar key to determine the pivot
            auto name = key.name().trimmed().toLower();
            for (auto keysOfChapter : m_medKeyByChapterMap.values())
            {
                for (auto aKey : *keysOfChapter)
                {
                    if (aKey.name().trimmed().toLower() == name)
                    {
                        key.setMedPivot(aKey.medPivot());
                        break;
                    }
                }
            }

            if (key.medPivot() == "")
            {
                key.setMedPivot(key.name());
            }
        }

        key.setMedPivot(key.medPivot().trimmed().toLower());

        m_medKeyByChapterMap[chapter]->push_back(key);
        if (m_medKeyByPivotMap[key.medPivot()] == nullptr)
        {
            m_medKeyByPivotMap[key.medPivot()] = new QList<Key2*>();
        }
        m_medKeyByPivotMap[key.medPivot()]->push_back(&m_medKeyByChapterMap[chapter]->last());

        bRes = true;
    }

    return bRes;
}

bool medMetaDataKeys::addKeyToChapterInternal(Key2 &key, QString &chapter)
{
    bool bRes = true;

    m_medKeyByChapterMap[chapter];

    bool needUpdate = false;

    bool keyMedPivotStrong;
    bool keyTagStrong;
    bool keyNameStrong;
    bool keyLabelStrong;

    strongKeyEval(key, keyTagStrong, keyMedPivotStrong, keyNameStrong, keyLabelStrong);

    if (m_medKeyByChapterMap[chapter] == nullptr)
    {
        m_medKeyByChapterMap[chapter] = new QList<Key2 >();
        m_medKeyByChapterMap[chapter]->push_back(key);
        needUpdate = true;
    }
    else
    {
        auto * keysVector = m_medKeyByChapterMap[chapter];
        QVector <Key2*> oldKeysByTag;
        QVector <Key2*> oldKeysByPivot;
        QVector <Key2*> oldKeysByName;

        for (auto oldKey : *keysVector)
        {
            if (keyTagStrong)
            {
                if (oldKey.tag() == key.tag())
                {
                    oldKeysByTag.push_back(&oldKey);
                }
            }
            if (keyMedPivotStrong)
            {
                if (oldKey.medPivot() == key.medPivot())
                {
                    oldKeysByPivot.push_back(&oldKey);
                }
            }
            if (keyNameStrong)
            {
                if (oldKey.name() == key.name())
                {
                    oldKeysByName.push_back(&oldKey);
                }
            }
        }

        if (oldKeysByTag.isEmpty() && oldKeysByPivot.isEmpty() && oldKeysByName.isEmpty())
        {
            m_medKeyByChapterMap[chapter]->push_back(key);
            needUpdate = true;
        }
        else
        {
            needUpdate = updateKey(key, { &oldKeysByTag , &oldKeysByPivot , &oldKeysByName });
        }
    }

    if (needUpdate)
    {
        scheduleUpdate(chapter);
    }

    return bRes;
}

bool medMetaDataKeys::updateKey(Key2 &key, QList<QVector<Key2 *>*> oldKeysLists)
{
    bool bRes = false;
    bool keyMedPivotStrong;
    bool keyTagStrong;
    bool keyNameStrong;
    bool keyLabelStrong;

    strongKeyEval(key, keyTagStrong, keyMedPivotStrong, keyNameStrong, keyLabelStrong);

    for (auto oldKeys : oldKeysLists)
    {
        for (auto *oldKey : *oldKeys)
        {
            bool oldKeyMedPivotStrong;
            bool oldKeyTagStrong;
            bool oldKeyNameStrong;
            bool oldKeyLabelStrong;

            strongKeyEval(*oldKey, oldKeyTagStrong, oldKeyMedPivotStrong, oldKeyNameStrong, oldKeyLabelStrong);

            if (!oldKeyTagStrong      && keyTagStrong) { bRes = true; oldKey->setTag(key.tag()); }
            if (!oldKeyMedPivotStrong && keyMedPivotStrong) { bRes = true; oldKey->setMedPivot(key.medPivot()); }
            if (!oldKeyNameStrong     && keyNameStrong) { bRes = true; oldKey->setName(key.name()); }
            if (!oldKeyLabelStrong    && keyLabelStrong) { bRes = true; oldKey->setLabel(key.label()); }
        }
    }

    return bRes;
}

void medMetaDataKeys::strongKeyEval(Key2 const & key, bool &keyTagStrong, bool &keyMedPivotStrong, bool &keyNameStrong, bool &keyLabelStrong)
{
    keyMedPivotStrong = !key.medPivot().isEmpty() && key.medPivot() != key.name() && key.medPivot() != key.tag();
    keyTagStrong = !key.tag().isEmpty();
    keyNameStrong = key.name() != key.tag();
    keyLabelStrong = !key.label().isEmpty() && key.label() != key.name();
}


Key2 medMetaDataKeys::keyInternal(QString &word)
{
    Key2 keyRes;

    keyRes = keyFromPivotInternal(word);
    if (!keyRes.isValid())
        {
        for (QString chapter : m_chapterToFileMap.keys())
        {
            keyRes = keyFromNameInternal(word, chapter);
            if (keyRes.isValid())
            {
                break;
            }
        }
    }

    if (!keyRes.isValid())
    {
        for (QString chapter : m_chapterToFileMap.keys())
        {
            keyRes = keyFromTagInternal(word, chapter);
            if (keyRes.isValid())
            {
                break;
            }
        }
    }

    return keyRes;
}

Key2 medMetaDataKeys::keyFromPivotInternal(QString & pivot)
{
    Key2 keyRes;

    pivot = pivot.toLower().trimmed();

    if (m_medKeyByPivotMap.contains(pivot))
    {
        keyRes = *m_medKeyByPivotMap.value(pivot)->first();
    }

    return keyRes;
}

Key2 medMetaDataKeys::keyFromNameInternal(QString &keyName, QString chapter)
{
    Key2 keyRes;

    int pos = -1;
    if (m_medKeyByChapterMap.contains(chapter))
    {
        QList<Key2>* keys = m_medKeyByChapterMap[chapter];
        pos = keys->indexOf(keyName);
        if (pos != -1)
        {
            keyRes = keys->at(pos);
        }
    }

    return keyRes;
}

Key2 medMetaDataKeys::keyFromTagInternal(QString & keyTag, QString chapter)
{
    Key2 keyRes;

    int pos = -1;
    if (m_medKeyByChapterMap.contains(chapter))
    {
        QList<Key2>* keys = m_medKeyByChapterMap[chapter];
        for (auto & key : *keys)
        {
            if (key.tag() == keyTag)
            {
                keyRes = key;
                break;
            }
        }
    }

    return keyRes;
}


QString medMetaDataKeys::pivotInternal(QString &keyName, QString chapter)
{
    return keyFromName(keyName, chapter).medPivot();
}

bool medMetaDataKeys::keyExistInternal(Key2 const & key)
{
    return m_medKeyByPivotMap.contains(key.medPivot());
}

void medMetaDataKeys::scheduleUpdate(QString &chapter)
{
    m_mutex.lock();
    m_saveScheduler.stop();
    if (!m_chaptersToUpdate.contains(chapter))
    {
        m_chaptersToUpdate.push_back(chapter);
    }
    m_mutex.unlock();
    m_saveScheduler.start(5000);
}

void medMetaDataKeys::delegateWriting()
{
    this->m_mutex.lock();
    if (updateChapterDirectory(this->m_path))
    {
        this->m_chaptersToUpdate.clear();
    }
    else
    {
        qDebug() << "medMetaDataKeys: Unable to update chapters at path " << this->m_path;
    }
    this->m_mutex.unlock();
}


bool operator==(Key2 const & k1, Key2 const & k2)
{
    bool bRes = k1.m_medPivot.isEmpty() || k2.m_medPivot.isEmpty() || k1.m_medPivot == k2.m_medPivot;

    bRes = bRes && k1.m_name == k2.m_name;

    return bRes;
}

bool operator==(QString const & s, Key2 const & k)
{
    return s == k.m_name;
}

bool operator==(Key2 const & k, QString const & s)
{
    return s == k.m_name;
}