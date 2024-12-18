/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include "medSQLitePluginExport.h"

#include <medAbstractSource.h>
#include <medStringParameter.h>
#include <medSQLiteWritingPolicy.h>
#include <medGroupParameter.h>
#include <QSqlDatabase>

class Worker;
template <typename T>
class medSQlite : public medAbstractSource
{

public:
    explicit medSQlite();
    ~medSQlite() override;

    /* ***********************************************************************/
    /* *************** Init/set/ctrl source properties ***********************/
    /* ***********************************************************************/
    bool initialization(const QString &pi_instanceId) override;

    bool setInstanceName(const QString &pi_instanceName) override;

    bool connect(bool pi_bEnable) override;

    QList<medAbstractParameter *> getAllParameters() override;

    QList<medAbstractParameter *> getCipherParameters() override;

    QList<medAbstractParameter *> getVolatilParameters() override;

    QList<medAbstractParameter*> getFilteringParameters() override;

    /* ***********************************************************************/
    /* *************** Get source properties *********************************/
    /* ***********************************************************************/
    bool isWritable() override;

    bool isLocal() override;

    bool isCached() override;

    bool isOnline() override;

    bool isFetchByMinimalEntriesOrMandatoryAttributes() override;

    /* ***********************************************************************/
    /* *************** Get source structure information **********************/
    /* ***********************************************************************/
    QString getInstanceName() override;

    QString getInstanceId() override;

    unsigned int getLevelCount() override;

    unsigned int getLevelDesiredWritable() override;

    QStringList getLevelNames() override;

    QString getLevelName(unsigned int pi_uiLevel) override;

    bool isLevelWritable(unsigned int pi_uiLevel) override;

    virtual int getIOInterface() override;
    virtual QMap<QString, QStringList> getTypeAndFormat() override;

    QStringList getMandatoryAttributesKeys(unsigned int pi_uiLevel) override;

    //QStringList getAdditionalAttributesKeys(unsigned int pi_uiLevel) override;


    /* ***********************************************************************/
    /* *************** Get elements data *************************************/
    /* ***********************************************************************/
    QList<levelMinimalEntries> getMinimalEntries(unsigned int pi_uiLevel, QString parentId) override;

    QList<QMap<QString, QString>> getMandatoryAttributes(unsigned int pi_uiLevel, QString parentId) override;

    bool getAdditionalAttributes(unsigned int pi_uiLevel, QString id, datasetAttributes &po_attributes) override;

    /* ***********************************************************************/
    /* *************** Get data          *************************************/
    /* ***********************************************************************/
    QVariant getDirectData(unsigned int pi_uiLevel, QString key) override;

    int getAssyncData(unsigned int pi_uiLevel, QString id) override;

    //    QString addData(QVariant data, QStringList parentUri, QString name) override;
    virtual bool addDirectData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) override;
    virtual int addAssyncData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) override;
    virtual bool createPath(QList<levelMinimalEntries> &pio_path, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel = 0, QString parentKey = "") override;
    virtual bool createFolder(levelMinimalEntries &pio_minimalEntries, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString parentKey) override;
    virtual bool alterMetaData(datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString key) override;
    virtual bool getThumbnail(QPixmap &po_thumbnail, unsigned int pi_uiLevel, QString key) override;
    virtual bool setThumbnail(QPixmap &pi_thumbnail, unsigned int pi_uiLevel, QString key) override;
    virtual bool commitData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) override { return false; };
    virtual QVariant getAsyncResults(int pi_iRequest) override;
    virtual int push(unsigned int pi_uiLevel, QString key) override { return -1; };

    inline virtual medAbstractWritingPolicy *getWritingPolicy() override { return &m_writingPolicy; };
public slots:
    void abort(int pi_iRequest) override;

    void updateDatabaseName(QString const &path);



private:
    // methods
    bool isValidDatabaseStructure();
    bool isDatabaseEmpty();
    virtual bool createTable(const QString &strQuery);
    virtual QList<medAbstractSource::levelMinimalEntries> getPatientMinimalEntries(QString &key);
    virtual QList<medAbstractSource::levelMinimalEntries> getStudyMinimalEntries(QString &parentId);
    virtual QList<medAbstractSource::levelMinimalEntries> getSeriesMinimalEntries(QString &parentId);
    virtual void optimizeSpeedSQLiteDB();
    virtual bool getSeriesDirectData(QString &key, QString &path);
    virtual QList<QMap<QString, QString>> getPatientMandatoriesAttributes(const QString &key);
    QList<QMap<QString, QString>> getStudyMandatoriesAttributes(const QString &key);

    QList<QMap<QString, QString>> getSeriesMandatoriesAttributes(const QString &key);

    bool getSeriesAdditionalAttributes(const QString &key, datasetAttributes &po_attributes);

    QString addDataToPatientLevel(QMap<QString, QString> &mandatoryAttributes);
    QString addDataToStudyLevel(QMap<QString, QString> mandatoryAttributes);
    QString addDataToSeriesLevel(const QVariant& dataset,const QString & name,const QString &studyId);

private:
    // members
    T m_Engine;
    medStringParameter *m_DbPath;
    const QString m_Driver;
    //const QString m_ConnectionName;
    QString m_instanceId;
    QString m_instanceName;
    bool m_online;
    QStringList m_LevelNames;
    QMap<QString, QStringList> m_MandatoryKeysByLevel;
    QMap<QString, QStringList> m_SupportedTypeAndFormats;
    medSQLiteWritingPolicy m_writingPolicy;

    QMap<QString, QString> m_PatientLevelAttributes;
    medGroupParameter *m_FilterDBSettings;

    static std::atomic<int> s_RequestId;
    QMap<int, QVariant> m_requestToDataMap;
    QMap<int, QTimer*> m_requestToTimerMap;
    QMap<int, QTime*> m_requestToTimeMap;

    QThread m_Thread;
    Worker *m_pWorker;
//    QTimer m_timer;
//    void timeManagement();
};

#include "medSQLite.txx"

