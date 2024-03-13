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
#include <medAbstractData.h>

#include <medCoreExport.h>

#include<QMap>
#include<QMultiMap>
#include<QVector>

#include<QString>
#include<QVariant>

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

    Key2& Key2::operator=(Key2 &&) = default;
    Key2& Key2::operator=(Key2 const &) = default;

    friend bool operator==(Key2 & k1, Key2 & k2);

    operator QString() const { return m_name; }
    //operator char const *() const { return m_name.toUtf8(); };

    const QString& name()        const { return m_name; }
    const QString& label()       const { return m_label; }
    const QString& tag()         const { return m_tag; }
    const QString& medPivot()    const { return m_medPivot; }
    const QVariant::Type& type() const { return m_type; }

    void setName(QString name) { m_name = name; }
    void setTag(QString tag) { m_tag = tag; }
    void setMedPivot(QString pivot) { m_medPivot = pivot; }
    void setLabel(QString label) { m_label = label; }

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

    //static bool registerKey(QString key, QString label, QString medInriaKey, QVariant type);
    //static void registerConverter(std::type_info inputType, std::type_info outputType, keyConverter converter, QString keyIn = "", QString keyOut = "");

    //static bool convertKey(QString keyNameIn, QString keyNameOut, QVariant &valueIn, QVariant valueOut, QString chapterInName = "", QString chapterOutName = "");

    //static Key2  getKeyFromChapterAndPivot(QString chapter, QString pivot);
    //static Key2  getKeyFromKeyOnOtherChapter(QString chapter, QString keyName);
    static Key2    key(QString pivot);
    static Key2    keyFromName(QString keyName, QString chapter = "");
    static QString pivot(QString keyName, QString chapter = "");

    //static QString getValue(Key2, QMap<QString, QString> metaDataList);
    //static QVariant getValue(Key2, QMap<QString, QVariant> metaDataList);

private:
    medMetaDataKeys();

    bool fetchChapterDirectory(QString path);
    QString loadChapter(QByteArray chapter); //return chapter name or empty if it fails
    bool readKey(QJsonObject keyAsJson, Key2 &key);

    bool updateChapterDirectory(QString path);
    bool writeChapter(QString chapterName, QJsonDocument &chapter);
    bool writeKey(Key2 const &key, QJsonObject &keyAsJson);


    bool    registerKeyInternal(Key2 &key, QString& chapter);
    bool    addKeyToChapterInternal(Key2 &key, QString &chapter);

    bool updateKey(Key2 &key, QList<QVector<Key2 *>*> oldKeysLists);

    void strongKeyEval(Key2 const & key, bool &keyTagStrong, bool &keyMedPivotStrong, bool &keyNameStrong, bool &keyLabelStrong);

    Key2    keyInternal(QString &pivot);
    Key2    keyFromNameInternal(QString &keyName, QString chapter = "");
    QString pivotInternal(QString &keyName, QString chapter = "");


    void scheduleUpdate(QString &chapter);
    void delegateWriting();

private:
    static medMetaDataKeys * s_instance;

    QString m_path;

    QMap<QString /*medInriaKey*/, QVector<Key2*>*>                  m_medKeyByPivotMap;
    QMap<QString /*chapter*/, QVector<Key2 >*>                  m_medKeyByChapterMap;
    QMap<QString /*chapter*/, QVector<QString> /*file name*/ >  m_chapterToFileMap;

    QVector<QString> m_chaptersToUpdate;
    QTimer m_saveSheduler;
    QMutex m_mutex;

};
