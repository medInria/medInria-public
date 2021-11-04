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

#include <QSqlDatabase>


template <typename T>
class medSQlite: public medAbstractSource
{

public:
    explicit medSQlite();
    ~medSQlite() override = default;

    /* ***********************************************************************/
    /* *************** Init/set/ctrl source properties ***********************/
    /* ***********************************************************************/
    bool initialization(const QString &pi_instanceId) override;

    bool setInstanceName(const QString &pi_instanceName) override;

    bool connect(bool pi_bEnable) override;

    QList<medAbstractParameter *> getAllParameters() override;

    QList<medAbstractParameter *> getCipherParameters() override;

    QList<medAbstractParameter *> getVolatilParameters() override;

    /* ***********************************************************************/
    /* *************** Get source properties *********************************/
    /* ***********************************************************************/
    bool isWriteable() override;

    bool isLocal() override;

    bool isCached() override;

    bool isOnline() override;

    /* ***********************************************************************/
    /* *************** Get source structure information **********************/
    /* ***********************************************************************/
    QString getInstanceName() override;

    QString getInstanceId() override;

    unsigned int getLevelCount() override;

    QStringList getLevelNames() override;

    QString getLevelName(unsigned int pi_uiLevel) override;

    QStringList getMandatoryAttributesKeys(unsigned int pi_uiLevel) override;

    QStringList getAdditionalAttributesKeys(unsigned int pi_uiLevel) override;


    /* ***********************************************************************/
    /* *************** Get elements data *************************************/
    /* ***********************************************************************/
    QList<levelMinimalEntries> getMinimalEntries(unsigned int pi_uiLevel, QString parentId) override;

    QList<QMap<QString, QString>> getMandatoryAttributes(unsigned int pi_uiLevel, int id) override;

    QList<QMap<QString, QString>> getAdditionalAttributes(unsigned int pi_uiLevel, int id) override;

    /* ***********************************************************************/
    /* *************** Get data          *************************************/
    /* ***********************************************************************/
    QString getDirectData(unsigned int pi_uiLevel, QString key) override;

    int getAssyncData(unsigned int pi_uiLevel, QString id) override;
    
    bool addData(void* data, QString uri) override;

public slots:
    void abort(int pi_iRequest) override;

    void updateDatabaseName(QString const &path);


private:
    // members
    T m_Engine;
    medStringParameter *m_DbPath;
    const QString m_Driver;
    const QString m_ConnectionName;
    QString m_instanceId;
    QString m_instanceName;
    bool m_online;
    QStringList m_LevelNames;

    // methods
    bool isValidDatabaseStructure();
    bool isDatabaseEmpty();
    virtual bool createTable(const QString &strQuery);
    virtual QList<levelMinimalEntries> getPatientMinimalEntries();
    virtual QList<medAbstractSource::levelMinimalEntries> getStudyMinimalEntries(QString &parentId);
    virtual QList<medAbstractSource::levelMinimalEntries> getSeriesMinimalEntries(QString &parentId);
    virtual void optimizeSpeedSQLiteDB();
    virtual QString getSeriesDirectData(QString &key);

    virtual void removeDatabase();
    virtual bool isDriverAvailable();
    virtual T addDatabase();
    virtual T database();
};

#include "medSQLite.txx"

