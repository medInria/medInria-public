/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include "medAPHPPluginExport.h"
#include <sphereDicomWeb/medAbstractAnnotation.h>

#include <medAbstractSource.h>
#include <PluginAPHP/QtDcmInterface.h>

#include <atomic>

class medStringParameter;
class medIntParameter;
struct medAPHPParametersPrivate;

class medAPHP: public medAbstractSource
{
Q_OBJECT
public:
    explicit medAPHP(QtDcmInterface *dicomLib, medAbstractAnnotation *annotationAPI);

    ~medAPHP();

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

    QStringList getLevelNames() override;

    QString getLevelName(unsigned int pi_uiLevel) override;

    QStringList getMandatoryAttributesKeys(unsigned int pi_uiLevel) override;


    /* ***********************************************************************/
    /* *************** Get elements data *************************************/
    /* ***********************************************************************/
    QList<levelMinimalEntries> getMinimalEntries(unsigned int pi_uiLevel, QString parentId) override;

    QList<QMap<QString, QString>> getMandatoryAttributes(unsigned int pi_uiLevel, QString parentId) override;

    bool getAdditionalAttributes(unsigned int pi_uiLevel, QString id, datasetAttributes4 &po_attributes) override;

    /* ***********************************************************************/
    /* *************** Get data          *************************************/
    /* ***********************************************************************/
    QVariant getDirectData(unsigned int pi_uiLevel, QString key) override;

    int getAssyncData(unsigned int pi_uiLevel, QString key) override;

    QString addData(QVariant data, QStringList parentUri, QString name) override;

public slots:
    void abort(int pi_iRequest) override;
    void onSettingsSaved();

private:
    int getQtDcmAsyncData(unsigned int pi_uiLevel, const QString &key);
    int getAnnotationAsyncData(const QString &key);

private:
    medAPHPParametersPrivate* d;

    static std::atomic<int> s_RequestId;
    static medStringParameter s_Aetitle;
    static medStringParameter s_Hostname;
    static medIntParameter s_Port;

    // TODO : remove TimeOut & QTime it is a HACK
    int timeout;
    QTimer timer;

};


