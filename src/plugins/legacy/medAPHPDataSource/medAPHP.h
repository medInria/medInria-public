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

    int getIOInterface() override;

    QMap<QString, QStringList> getTypeAndFormat() override;
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

    bool addDirectData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel,
                       QString parentKey) override;

    int addAssyncData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel,
                      QString parentKey) override;

    bool createFolder(levelMinimalEntries &pio_minimalEntries, const datasetAttributes4 &pi_attributes,
                      unsigned int pi_uiLevel, QString parentKey) override;
    bool createPath(QList<levelMinimalEntries> &pio_path, const datasetAttributes4 &pi_attributes, unsigned int pi_uiLevel,
               QString parentKey) override;

    bool alterMetaData(const datasetAttributes4 &pi_attributes, unsigned int pi_uiLevel, QString key) override;

    bool getThumbnail(QPixmap &po_thumbnail, unsigned int pi_uiLevel, QString key) override;

    bool setThumbnail(QPixmap &pi_thumbnail, unsigned int pi_uiLevel, QString key) override;

    bool commitData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel,
                    QString parentKey) override;

    int push(unsigned int pi_uiLevel, QString key) override;

    QVariant getAsyncResults(int pi_iRequest) override;
public slots:
    void abort(int pi_iRequest) override;
    void onSettingsSaved();

private:
    int getQtDcmAsyncData(unsigned int pi_uiLevel, const QString &key);
    int getAnnotationAsyncData(const QString &key);
    void computeDateRange();

    QList<QMap<DcmTagKey, QString>> cFindPatient(const QString &patientID);
    QList<QMap<DcmTagKey, QString>> cFindStudy(const QString &studyInstanceUID, const QString &patientID = "");
    QList<QMap<DcmTagKey, QString>> cFindSeries(const QString &studyInstanceUID);

    QList<levelMinimalEntries> getPatientMinimalEntries(const QList<QMap<DcmTagKey, QString>> &infosMap);
    QList<levelMinimalEntries> getStudyMinimalEntries(const QList<QMap<DcmTagKey, QString>> &infosMap, bool isSeriesFiltered = false);
    QList<levelMinimalEntries> getSeriesMinimalEntries(const QList<QMap<DcmTagKey, QString>> &infosMap);

    QList<QMap<QString, QString>> getPatientMandatoryAttributes(const QList<QMap<DcmTagKey, QString>> &infosMap);
    QList<QMap<QString, QString>> getStudyMandatoryAttributes(const QList<QMap<DcmTagKey, QString>> &infosMap, bool isSeriesFiltered = false);
    QList<QMap<QString, QString>> getSeriesMandatoryAttributes(const QList<QMap<DcmTagKey, QString>> &infosMap);

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


