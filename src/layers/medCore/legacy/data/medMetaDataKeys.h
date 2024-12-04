#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vector>

#include <medCoreExport.h>

#include<QMap>
#include<QMultiMap>
#include<QVector>

#include<QString>
#include<QVariant>
#include<QTimer>
#include<QMutex>

#include <utility>

using keyConverter = bool(*)(QVariant const & inputData, QVariant & outputData);

class MEDCORE_EXPORT Key2
{
    public:
    Key2() = default;
    Key2(QString const & name, QString const & label = "", QString const & tag = "", QString const & medKey = "", QVariant::Type type = QVariant::String) :
        m_name(name), m_label(label), m_tag(tag), m_medPivot(medKey), m_type(type)
    {
        if (label == "") m_label = name;
    }

    Key2(Key2 const & key)
    {
        m_name = key.m_name;
        m_label = key.m_label;
        m_tag = key.m_tag;
        m_medPivot = key.m_medPivot;
        m_type = key.m_type;
    }

    Key2(Key2 const && key) noexcept :

        m_name(std::move(key.m_name)),
        m_label(std::move(key.m_label)),
        m_tag(std::move(key.m_tag)),
        m_medPivot(std::move(key.m_medPivot)),
        m_type(std::move(key.m_type))
    {}

    ~Key2() { }

    Key2& operator=(Key2 && k) noexcept
    {
        this->m_name     = k.m_name;
        this->m_label    = k.m_label;
        this->m_tag      = k.m_tag;
        this->m_medPivot = k.m_medPivot;
        this->m_type     = k.m_type;

        return *this;
    }

    Key2& operator=(Key2 const & k)
        {
        this->m_name =     k.m_name;
        this->m_label =    k.m_label;
        this->m_tag =      k.m_tag;
        this->m_medPivot = k.m_medPivot;
        this->m_type =     k.m_type;

        return *this;
        }

    friend MEDCORE_EXPORT bool operator==(Key2 const & k1, Key2 const & k2);
    friend MEDCORE_EXPORT bool operator==(QString const & s, Key2 const & k);
    friend MEDCORE_EXPORT bool operator==(Key2 const & k, QString const & s);

    operator QString() const { return m_name; }

    const QString& name()        const { return m_name; }
    const QString& label()       const { return m_label; }
    const QString& tag()         const { return m_tag; }
    const QString& medPivot()    const { return m_medPivot; }
    const QVariant::Type& type() const { return m_type; }

    void setName(QString name) { m_name = name; }
    void setTag(QString tag) { m_tag = tag; }
    void setMedPivot(QString pivot) { m_medPivot = pivot; }
    void setLabel(QString label) { m_label = label; }

    bool isValid() { return ! m_medPivot.isEmpty(); }

private:

    QString        m_name;
    QString        m_label;
    QString        m_tag;
    QString        m_medPivot;
    QVariant::Type m_type;
    };




class MEDCORE_EXPORT medMetaDataKeys : public QObject
{
    Q_OBJECT
public:
    ~medMetaDataKeys() = default;

    static medMetaDataKeys * instance();

    static bool registerKey(Key2 key, QString chapter = "default");
    static bool addKeyToChapter(Key2 key, QString chapter = "default");
    static bool addKeyByTagToChapter(QString tag, QString keyLabel = "", QString keyName = "", QString chapter = "default");

    static Key2    key(QString word);
    static Key2    keyFromPivot(QString pivot);
    static Key2    keyFromName(QString keyName, QString chapter = "");
    static Key2    keyFromTag(QString keyTag, QString chapter = "");
    static QString pivot(QString keyName, QString chapter = "");

    static bool    keyExist(Key2 const & key);

    static QString getValue(Key2 &key, QMap<QString, QString> metaDataList);

    static QString path();
private:
    medMetaDataKeys();

    bool fetchChapterDirectory(QString path);
    QString loadChapter(QByteArray chapter); //return chapter name or empty if it fails
    bool readKey(QJsonObject keyAsJson, Key2 &key);

    bool updateChapterDirectory(QString path);
    bool writeChapter(QString chapterName, QJsonDocument &chapter);
    bool writeKey(Key2 const &key, QJsonObject &keyAsJson);


    bool registerKeyInternal(Key2 &key, QString& chapter);
    bool addKeyToChapterInternal(Key2 &key, QString &chapter);

    bool updateKey(Key2 &key, QList<QVector<Key2 *>*> oldKeysLists);

    void strongKeyEval(Key2 const & key, bool &keyTagStrong, bool &keyMedPivotStrong, bool &keyNameStrong, bool &keyLabelStrong);

    Key2    keyInternal(QString &word);
    Key2    keyFromPivotInternal(QString &pivot);
    Key2    keyFromNameInternal(QString &keyName, QString chapter = "");
    Key2    keyFromTagInternal(QString &keyTag, QString chapter = "");
    QString pivotInternal(QString &keyName, QString chapter = "");

    bool    keyExistInternal(Key2 const & key);

    void scheduleUpdate(QString &chapter);
    void delegateWriting();

private:
    static medMetaDataKeys * s_instance;

    QString m_path;

    QMap<QString /*medInriaKey*/, QList<Key2*>*>                  m_medKeyByPivotMap;
    QMap<QString /*chapter*/,     QList<Key2 >*>                  m_medKeyByChapterMap;
    QMap<QString /*chapter*/,     QList<QString> /*file name*/ >  m_chapterToFileMap;

    QVector<QString> m_chaptersToUpdate;
    QTimer m_saveScheduler;
    QMutex m_mutex;
};
