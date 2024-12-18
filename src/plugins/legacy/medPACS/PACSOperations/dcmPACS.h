/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/



#ifndef DCMPACS_H
#define DCMPACS_H

#include <QTemporaryDir>
#include <QThread>

#include "dcmMoveScu.h"
#include "dcmFifoMover.h"
#include <medAbstractSource.h>

class DcmDataset;
class DcmTransportLayer;
class OFConsoleApplication;
struct T_ASC_Network;
struct T_ASC_Association;
struct T_ASC_Parameters;

class dcmPACS : public QObject
{
    Q_OBJECT

public:
    dcmPACS();
    ~dcmPACS();

    bool isCachedDataPath(int pi_requestId);

    int cEcho();
    QList<QMap<DcmTagKey, QString>> cFind(const QMap<DcmTagKey, QString> &filters);
    bool cMove(int pi_requestId, const QString &queryLevel, const QString &key);

public slots:
    void updateLocalParameters(QString const &aet, QString const &hostname, int port);
    void updateRemoteParameters(QString const &aet, QString const &hostname, int port);
    void stopMove(int pi_requestId);
    void sendPending(int requestId, medAbstractSource::eRequestStatus status);

signals:
    void moveProgress(int requestId, medAbstractSource::eRequestStatus status, QString path = "");

private:
    static bool isServerAvailable(const QString &hostName, int port);

private:
    T_ASC_Network *m_net{}; // network struct, contains DICOM upper layer FSM etc.
    T_ASC_Parameters *m_params{}; // parameters of association request
    T_ASC_Association *m_assoc{};

    QThread m_Thread;
    QTemporaryDir m_temporaryDir;

    QMap<int, dcmMoveScu*> m_RequestIdMap;
    dcmFifoMover *m_FifoMover;

    // settings definition used for local and remote servers
    typedef struct{
        QString aetitle;
        QString hostname;
        int port = -1;
        QString name;
    } ServerSettings;

    ServerSettings m_localServer;
    ServerSettings m_remoteServer;
};

#endif //DCMPACS_H

/**
 * @fn void dcmPACS::updateLocalParameters(const QString &aet, const QString &hostname, int port)
 * @brief set local server parameters as specified in the plugin settings
 * @param aet [in] application entity title : local server identifier
 * @param hostname [in] local server network address
 * @param port [in] local server port
*/

/**
 * @fn void dcmPACS::updateRemoteParameters(const QString &aet, const QString &hostname, int port)
 * @brief set remote server parameters as specified in the plugin settings
 * @param aet [in] application entity title : remote server identifier
 * @param hostname [in] remote server network address
 * @param port [in] remote server port
*/

/**
 * @fn bool dcmPACS::isCachedDataPath(int pi_requestId)
 * @brief get the local path of the data if it has already been retrieved
 * @param pi_requestId [in] request id linked to the data
 * @return true if the requestId exists, and therefore if the data has already been retrieved, false otherwise
*/


/**
 * @fn bool dcmPACS::isServerAvailable(const QString &hostName, const int port)
 * @brief check that the remote server is available by trying to establish a connection to its hostname on its port
 * @param hostname [in] remote server hostname = network address
 * @param port [in] remote server port
 * @return true if the connection has been established, false otherwise
*/

/**
 * @fn int dcmPACS::cEcho()
 * @brief check that the connection settings of the local and remote servers are correct, and they are able to communicate with each other
 * @details set up and execute DCMTK's ECHO request
 * @return an integer, corresponding to a boolean, to indicate whether they are able to communicate or not
*/


/**
 * @fn QList<QMap<DcmTagKey, QString>> dcmPACS::cFind(const QMap<DcmTagKey, QString> &filters)
 * @brief search for information on element (according to the level) according to the filters passed as parameters
 * @details set up and execute DCMTK's FIND request. Using and defining precise values for parameters is optional, but will induce the query response by filtering the information to be retrieved
 * An empty value for a DICOM tag key will be field with any value found for this key by the query
 * element can be a patient, a study or a series
 * @param filters [in] define the filters to be used for the query. DcmTagKey 'QueryRetrieveLevel' determines on which level the search will be performed
 * @return a QList for each element (according to the level) found that matches the query through the values of its DICOM tags, contained in a QMap
*/

/**
 * @fn bool dcmPACS::cMove(int pi_requestId, const QString &queryLevel, const QString &key)
 * @brief retrieve data accessed via FIND query by copying the file contents in a local file
 * @details set up DCMTK's MOVE request. Execute opertion in function moveSCU from dcmMoveScu.cpp file
 * @param pi_requestId [in] the request identifier
 * @param queryLevel [in] level on which the operation is to be performed
 * @param key [in] the uid of the study or the series concerned
 * @return true if the operation went vell, false otherwise
*/

/**
 * @fn void dcmPACS::sendPending(int requestId, medAbstractSource::eRequestStatus status)
 * @brief indicate that the MOVE request is in progress. Links information from 'dcmFifoMover'
 * @param requestId [in] request identifier
 * @param status [in] status of the operation : finish, pending or faild. In this case, it is set to 'pending' 
*/

/**
 * @fn void dcmPACS::stopMove(int pi_requestId)
 * @brief stop execution of the MOVE request for the specified request ID
 * @param pi_requestId [in] request identifier
*/
