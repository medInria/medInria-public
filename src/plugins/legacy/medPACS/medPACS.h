/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medAbstractSource.h>
#include "medPACSPluginExport.h"

#include "PACSOperations/dcmPACS.h"

#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/dcdicent.h>

#include <atomic>

class medStringParameter;
class medIntParameter;
struct medPACSParametersPrivate;

class medPACS: public medAbstractSource
{
    Q_OBJECT

public:
    explicit medPACS(dcmPACS *dicomLib);

    ~medPACS();

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

    bool getAdditionalAttributes(unsigned int pi_uiLevel, QString id, datasetAttributes &po_attributes) override;

    /* ***********************************************************************/
    /* *************** Get data          *************************************/
    /* ***********************************************************************/
    QVariant getDirectData(unsigned int pi_uiLevel, QString key) override;

    int getAssyncData(unsigned int pi_uiLevel, QString key) override;

    bool addDirectData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel,
                       QString parentKey) override;

    int addAssyncData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel,
                      QString parentKey) override;

    bool createFolder(levelMinimalEntries &pio_minimalEntries, const datasetAttributes &pi_attributes,
                      unsigned int pi_uiLevel, QString parentKey) override;
    bool createPath(QList<levelMinimalEntries> &pio_path, const datasetAttributes &pi_attributes, unsigned int pi_uiLevel,
               QString parentKey) override;

    bool alterMetaData(const datasetAttributes &pi_attributes, unsigned int pi_uiLevel, QString key) override;

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
    void computeDateRange();

    QList<QMap<DcmTagKey, QString>> cFindPatient(const QString &patientID);
    QList<QMap<DcmTagKey, QString>> cFindStudy(const QString &studyInstanceUID, const QString &patientID = "");
    QList<QMap<DcmTagKey, QString>> cFindSeries(const QString &studyInstanceUID);

    QList<levelMinimalEntries> getPatientMinimalEntries(const QList<QMap<DcmTagKey, QString>> &infosMap);
    QList<levelMinimalEntries> getStudyMinimalEntries(const QList<QMap<DcmTagKey, QString>> &infosMap, bool isSeriesFiltered = false);
    QList<levelMinimalEntries> getSeriesMinimalEntries(const QList<QMap<DcmTagKey, QString>> &infosMap);

    QList<QMap<QString, QString>> getPatientMandatoryAttributes(const QList<QMap<DcmTagKey, QString>> &infosMap);
    QList<QMap<QString, QString>> getStudyMandatoryAttributes(QList<QMap<DcmTagKey, QString>> &infosMap, bool isPatientFiltered, bool isSeriesFiltered);
    QList<QMap<QString, QString>> getSeriesMandatoryAttributes(QList<QMap<DcmTagKey, QString>> &infosMap, bool isPatientFiltered, bool isStudiesFiltered);


    // Filter parameters
    const DcmDictEntry * retrieveDcmTag(const QString &key);
    void computeBirthDateRange(const DcmTagKey &tagKey, const QString &start, const QString &end);
    void addPatientFilterParameters(const QJsonObject &patientObject);
    void addStudyFilterParameters(const QJsonObject &studyObject);
    void addSeriesFilterParameters(const QJsonObject &seriesObject);
    void readJsonFile(const QString &filePath);
    void resetFilterParameterValues(QList<medAbstractParameter*> levelFilter, QMap<DcmTagKey, QString> &levelAttributes);

    int getPACSAsyncData(unsigned int pi_uiLevel, const QString &key);
    int getDicomWebAsyncData(unsigned int pi_uiLevel, const QString &key);
    

private:
    medPACSParametersPrivate* d;

    // each plugin instance shares the same local server parameters
    static std::atomic<int> s_RequestId;
    static medStringParameter s_Aetitle;
    static medStringParameter s_Hostname;
    static medIntParameter s_Port;

};

// definitions of functions that are specific to this plugin, in addition to those related to mandatoryAttributes

/**
 * @fn void medPACS::computeDateRange()
 * @brief set the value corresponding to the DCM_StudyDate tag, in the study level, from the dates indicated in the filtering parameters
*/




/**
 * @fn QList<QMap<DcmTagKey, QString>> medPACS::cFindPatient(const QString &patientID)
 * @brief define the parameters to be used for a FIND request from the DICOM tags, possibly with a defined value, of the patient level
 * @param patientID [in] the value of a specific DCM_PatientID tag if there is a need to refine the search, from filtering parameters for exemple
 * @return a QList containing the DICOM tag values for each patient found (in a QMap)
*/

/**
 * @fn QList<QMap<DcmTagKey, QString>> medPACS::cFindStudy(const QString &studyInstanceUID, const QString &patientID = "")
 * @brief define the parameters to be used for a FIND request from the DICOM tags, possibly with a defined value, of the study level to retrieve studies information from a patient
 * @param studyInstanceUID [in] the value of a specific DCM_StudyInstanceUID tag if there is a need to refine the search, from filtering parameters for exemple
 * @param patientID [in] the DCM_PatientID of the parent or a specific patient whose studies are searched
 * @return a QList containing the DICOM tag values for each study found (in a QMap)
*/

/**
 * @fn QList<QMap<DcmTagKey, QString>> medPACS::cFindSeries(const QString &studyInstanceUID)
 * @brief define the parameters to be used for a FIND request from the DICOM tags, possibly with a defined value, of the series level to retrieve series information from a study
 * @param studyInstanceUID [in] the DCM_StudyInstanceUID of the parent or a specific study whose series are searched
 * @return a QList containing the DICOM tag values for each series found (in a QMap)
*/




/**
 * @fn QList<QMap<QString, QString>> medPACS::getPatientMandatoryAttributes(const QList<QMap<DcmTagKey, QString>> &infosMap)
 * @brief set the minimal entries values for patients according to the FIND query response
 * @param infosMap [in] contains the tag values returned by the FIND query response
 * @return a QList containing a QMap for each patient with the values of the patient level's main DICOM tags
*/

/**
 * @fn QList<QMap<QString, QString>> medPACS::getStudyMandatoryAttributes(const QList<QMap<DcmTagKey, QString>> &infosMap, bool isPatientFiltered, bool isSeriesFiltered)
 * @brief set the minimal entries values for studies according to the FIND query response
 * @param infosMap [in] contains the tag values returned by the FIND query response
 * @param isPatientFiltered [in] indicates whether the FIND query response is subject to patient level filtering
 * @param isSeriesFiltered [in] indicates whether the FIND query response is subject to series level filtering
 * @return a QList containing a QMap for each study with the values of the study level's main DICOM tags
*/

/**
 * @fn QList<QMap<QString, QString>> medPACS::getSeriesMandatoryAttributes(const QList<QMap<DcmTagKey, QString>> &infosMap, bool isPatientFiltered, bool isStudiesFiltered)
 * @brief set the minimal entries values for series according to the FIND query response
 * @param infosMap [in] contains the tag values returned by the FIND query response
 * @param isPatientFiltered [in] indicates whether the FIND query response is subject to patient level filtering
 * @param isStudiesFiltered [in] indicates whether the FIND query response is subject to study level filtering
 * @return a QList containing a QMap for each series with the values of the series level's main DICOM tags
*/




/**
 * @fn const DcmDictEntry * medPACS::retrieveDcmTag(const QString &key)
 * @brief check whether a key exists, in the form of tag name or tag key, in the DICOM tag dictionary
 * @param key [in] the key to be searched in the dictionary
 * @return a DcmDictEntry from which the DICOM dictionary tag key can be retrieved, NULL if no match found
*/

/**
 * @fn void medPACS::computeBirthDateRange(const DcmTagKey &tagKey, const QString &start, const QString &end)
 * @brief set the value corresponding to a DcmTagKey, in the patient level, from a start date and an end date defined in the filtering parameters
 * @param tagKey [in] can represent the DCM_PatientBirthDate key or the DCM_PatientBirthTime key
 * @param start [in] the start date on an interval
 * @param end [in] the end date on an interval
*/

/**
 * @fn void medPACS::addPatientFilterParameters(const QJsonObject &patientObject)
 * @brief add new parameters to the existing list of patient level filter parameters
 * @param patientObject [in] a QJsonObject cointaining DICOM patient tag keys to be added, if they are correctly defined in the json file : tag name or tag key
*/

/**
 * @fn void medPACS::addStudyFilterParameters(const QJsonObject &studyObject)
 * @brief add new parameters to the existing list of study level filter parameters
 * @param studyObject [in] a QJsonObject cointaining DICOM study tag keys to be added, if they are correctly defined in the json file : tag name or tag key
*/

/**
 * @fn void medPACS::addSeriesFilterParameters(const QJsonObject &seriesObject)
 * @brief add new parameters to the existing list of series level filter parameters
 * @param seriesObject [in] a QJsonObject cointaining DICOM series tag keys to be added, if they are correctly defined in the json file : tag name or tag key
*/

/**
 * @fn void medPACS::readJsonFile(const QString &filePath)
 * @brief If available, read the json file sent in the plugin settings, which is used to add new filtering parameters
 * @param filePath [in] the local path to the file
*/

/**
 * @fn void medPACS::resetFilterParameterValues(QList<medAbstractParameter*> levelFilter, QMap<DcmTagKey, QString> &levelAttributes)
 * @brief reset filter parameters fields with default values
 * @param levelFilter [in] reset all filter parameters in a level list to their default value
 * @param levelAttributes [in] [out] redefine level attributes with a null value
*/




/**
 * @fn int medPACS::getPACSAsyncData(unsigned int pi_uiLevel, const QString &key)
 * @brief retrieve file data from a remote PACS by calling and using DCMTK's MOVE query
 * @param pi_uiLevel [in] level from which data is to be retrieved
 * @param key [in] key value for PatientID, or StudyInstanceUID, or SeriesInstanceUID, according to the query level
 * @return an integer corresponding to the request ID
*/

/**
 * @fn int medPACS::getDicomWebAsyncData(unsigned int pi_uiLevel, const QString &key)
 * @brief retrieve file data from a remote PACS by calling and using DICOMweb's, a RESTful DICOM service, WADO-RS query
 * @param pi_uiLevel [in] level from which data is to be retrieved
 * @param key [in] key value for StudyInstanceUID or SeriesInstanceUID, according to the query level
 * @return an integer corresponding to the request ID
*/
