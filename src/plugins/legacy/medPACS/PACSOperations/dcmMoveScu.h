/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef DCMMOVESCU_H
#define DCMMOVESCU_H

#include <QtGui>

#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmnet/assoc.h>
#include <dcmtk/dcmnet/cond.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/dcdicent.h>
#include <dcmtk/dcmdata/dcelem.h>
#include <dcmtk/dcmdata/dcitem.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcmetinf.h>


class DcmDataset;
class DcmTransportLayer;
class OFConsoleApplication;
struct T_ASC_Association;
struct T_ASC_Parameters;
struct T_DIMSE_C_MoveRQ;
struct T_DIMSE_C_MoveRSP;
struct T_DIMSE_C_EchoRQ;
struct T_DIMSE_C_EchoRSP;
struct T_DIMSE_C_StoreRQ;
struct T_DIMSE_C_StoreRSP;

class dcmMoveScu : public QThread
{
    Q_OBJECT

public:
    dcmMoveScu(const QStringList &data, const QString &outputDir, const QString &queryLevel, QObject *parent = 0);
    ~dcmMoveScu();

    void setLocalParameters(const QString &aetitle, const QString &hostName, const int &port);
    void setRemoteParameters(const QString &aetitle, const QString &hostName, const int &port);
    QString getOutputDir();

    void run();

public slots:
    void stopMoveNetwork();

protected:
    static void addOverrideKey(const QString &key);
    static void substituteOverrideKeys(DcmDataset *dataset);

    static void storeSCPCallback(void *storeCallbackData, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ *rq, char *imageFileName, DcmDataset **imageDataset, T_DIMSE_C_StoreRSP *rsp, DcmDataset **statusDetail);
    static OFCondition storeSCP(void *subOpCallback, T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID pcId); 
    static OFCondition echoSCP(T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID pcId);
    static void subOperationCallback(void *, T_ASC_Network *subNet, T_ASC_Association **subAssoc);

    static void moveCallback(void *callbackData, T_DIMSE_C_MoveRQ *request, int responseCount, T_DIMSE_C_MoveRSP *response);
    OFCondition moveSCU(T_ASC_Association *assoc);
    OFCondition moveRequest(const QString &uid);

signals:
    void moveFinished(const QString &path, const QString &uid, int number);
    void moveInProgress(const QString &message);
    void moveFailed();

private:
    DcmDataset *requestIdentifiers;
    DcmDataset *responseIdentifiers;

    T_ASC_Network *m_net{}; // network struct, contains DICOM upper layer FSM etc.
    T_ASC_Parameters *m_params{}; // parameters of association request
    T_ASC_Association *m_assoc{};

    struct StoreCallbackData
    {
        char *imageFileName;
        DcmFileFormat *dcmFile;
        T_ASC_Association *assoc;
        OFString outputDirectory;
    };

    struct CallbackInfo
    {
        T_ASC_PresentationContextID presId;
        T_ASC_Association *assoc;
    };

    QString m_inputDir;
    QStringList m_data;
    QString m_outputDir;
    QString m_queryLevel;
    OFString m_outputDirectory = ".";

    // local
    QString m_localAetitle;
    QString m_localHostName;
    int m_localPort;

    // remote
    QString m_remoteAetitle;
    QString m_remoteHostName;
    int m_remotePort;
};




/**
 * @fn void dcmMoveScu::setLocalParameters(const QString &aetitle, const QString &hostName, const int &port)
 * @brief set local server parameters as specified in the plugin settings
 * @param aetitle [in] application entity title : local server identifier
 * @param hostName [in] local server network address
 * @param port [in] local server port
*/

/**
 * @fn void dcmMoveScu::setRemoteParameters(const QString &aetitle, const QString &hostName, const int &port)
 * @brief set remote server parameters as specified in the plugin settings
 * @param aetitle [in] application entity title : remote server identifier
 * @param hostName [in] remote server network address
 * @param port [in] remote server port
*/

/**
 * @fn QString dcmMoveScu::getOutputDir()
 * @brief get the local folder path in which the data from MOVE request was downloaded
 * @return a QString of the absolute local path
*/

/**
 * @fn void dcmMoveScu::run()
 * @brief thread execution creates the local data download path and calls the function that launches MOVE request
 * @details send a signal depending on the request result
*/

/**
 * @fn void dcmMoveScu::addOverrideKey(const QString &key)
 * @brief customize request keys with the values of the data we wish to retrieve, according to the level
 * @param key [in] include the DICOM tag key and its new value, in the form : "key=value"
*/

/**
 * @fn void dcmMoveScu::substituteOverrideKeys(DcmDataset *dataset)
 * @brief substitute keys of the default file with those override in the addOverrideKey function
 * @param dataset [in] [out] contains the new file dataset with the override keys
*/

/**
 * @fn void dcmMoveScu::storeSCPCallback(void *storeCallback, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ *rq, char *imageFileName, DcmDataset **imageDataset, T_DIMSE_C_StoreRSP *rsp, DcmDataset **statusDetail)
 * @brief STORE request callback : save the DICOM file with the data retrieved from the MOVE request to the path defined at the start of thread execution
 * @details send a signal to say that the MOVE operation is in progress
 * @param storeCallback [in] [in request] pointer to data transmitted to the function
 * @param progress [in] [in request] progess state
 * @param rq [in] [in request] STORE request
 * @param imageFileName [in] [in request] file name in which information wil be saved
 * @param imageDataset [in] [in request] information received corresponding to file data
 * @param rsp [in] [out request] STORE response
 * @param statusDetail [in] [out request] detailed response status
*/

/**
 * @fn OFCondition dcmMoveScu::storeSCP(void *subOpCallback, T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID pcId)
 * @brief execute the DIMSE STORE Provider request and set some parameters to pass to the callback (storeSCPCallback)
 * @param subOpCallback [in] transmitted callback from the MOVE request : pointer to data transmitted to the function
 * @param assoc [in] containing association specific information
 * @param msg [in] represent the STORE DIMSE command which shall be sent
 * @param pcId [in] current presentation context ID
 * @return a condition resulting from the STORE DIMSE command
*/

/**
 * @fn OFCondition dcmMoveScu::echoSCP(T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID pcId)
 * @brief execute the DIMSE ECHO Provider response
 * @param assoc [in] containing association specific information
 * @param msg [in] represent the ECHO DIMSE command which shall be sent
 * @param pcId [in] current presentation context ID
 * @return a condition resulting from the ECHO DIMSE command
*/

/**
 * @fn void dcmMoveScu::subOperationCallback(void *subCallback, T_ASC_Network *subNet, T_ASC_Association **subAssoc)
 * @brief execute a sub-operation following the MOVE request
 * @details the sub-operation is decided according to the DIMSE command received in return of the MOVE request
 * In this case, it can be an ECHO request or a STORE request
 * @param subCallback [in] transmitted callback from the MOVE request : pointer to data transmitted to the function
 * @param subNet [in] network structure
 * @param subAssoc [in] containing association specific information
*/

/**
 * @fn void dcmMoveScu::moveCallback(void *callbackData, T_DIMSE_C_MoveRQ *request, int responseCount, T_DIMSE_C_MoveRSP *response)
 * @brief display the response field of the MOVE request result
 * @param callbackData [in] [in request] transmitted callback from the MOVE request : pointer to data transmitted to the function
 * @param request [in] [in request] MOVE request
 * @param responseCount [in] [out request] response number
 * @param response [in] [out request] MOVE response
*/

/**
 * @fn OFCondition dcmMoveScu::moveSCU(T_ASC_Association *assoc)
 * @brief execute the DIMSE MOVE User request
 * @details display information on request and response details
 * @param assoc [in] containing association specific information
 * @return a condition resulting from the MOVE DIMSE command
*/

/**
 * @fn const char *DU_cmoveStatusString(Uint16 statusCode)
 * @brief translate the status code received by the request response into a human-readable text
 * @param statusCode [in] MOVE request status
 * @return a string that translates the received code
*/

/**
 * @fn OFCondition dcmMoveScu::moveRequest(const QString &uid)
 * @brief define network and association parameters according to the local and remote servers before calling moveSCU function
 * @details override keys is called also in this function according to the query level
 * @param uid [in] the uid of the source whose data is to be retrieved : mainly the value of DICOM tag keys StudyInstanceUID or SeriesInstanceUID
 * @return a condition resulting from the MOVE DIMSE command (from moveSCU function)
*/

/**
 * @fn void dcmMoveScu::stopMoveNetwork()
 * @brief if the thread is running, allow to destroy network instance
*/

#endif //DCMMOVESCU_H
