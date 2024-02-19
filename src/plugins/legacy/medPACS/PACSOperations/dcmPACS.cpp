/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <memory>

#include <QDebug>
#include <QTcpSocket>
#include <QThread>

#include "dcmPACS.h"
#include "dcmFindCallbacks.h"

#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcuid.h>

dcmPACS::dcmPACS(){
    m_FifoMover = new dcmFifoMover;
    m_FifoMover->moveToThread(&m_Thread);
    connect(&m_Thread, &QThread::finished, m_FifoMover, &QObject::deleteLater);
    connect(&m_Thread, &QThread::started, m_FifoMover, &dcmFifoMover::processing);
    connect(m_FifoMover, SIGNAL(sendPending(const int, const int)), this, SLOT(sendPending(const int, const int)));
    
    m_Thread.start();
}

dcmPACS::~dcmPACS(){
    m_FifoMover->stopProcessing();
    m_Thread.quit();
    m_Thread.wait();
    m_RequestIdMap.clear();
}


void dcmPACS::updateLocalParameters(const QString &aet, const QString &hostname, int port){
    m_localServer.aetitle = aet;
    m_localServer.hostname = hostname;
    m_localServer.port = port;
}

void dcmPACS::updateRemoteParameters(const QString &aet, const QString &hostname, int port){
    m_remoteServer.name = "TEST";
    m_remoteServer.aetitle = aet;
    m_remoteServer.hostname = hostname;
    m_remoteServer.port = port;
}

bool dcmPACS::isCachedDataPath(int pi_requestId){
    bool bRes = false;
    auto mover = m_RequestIdMap[pi_requestId];
    QString outputPath = mover->getOutputDir();
    if(mover && !outputPath.isEmpty())
    {
        bRes = true;
        emit moveProgress(pi_requestId, medAbstractSource::eRequestStatus::finish, outputPath);
    }

    return bRes;
}





bool dcmPACS::isServerAvailable(const QString &hostName, const int port)
{
    bool bRes = false;
    QTcpSocket socket;
    socket.setSocketOption(QAbstractSocket::LowDelayOption, 1);
    socket.connectToHost(hostName, port);
    if(socket.waitForConnected(10000))
    {
        socket.disconnectFromHost();
        bRes = true;
    }
    return bRes;
}




int dcmPACS::cEcho(){
    OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 30, &m_net);
    if(cond.bad())
    {
        qDebug() << "Network initialization error : " << cond.text();
        return -1;
    }

    if(isServerAvailable(m_remoteServer.hostname, m_remoteServer.port))
    {
        cond = ASC_createAssociationParameters(&m_params, ASC_DEFAULTMAXPDU);
        if(cond.bad())
        {
            qDebug() << "Create parameters association error : " << cond.text();
            return -1;
        }

        cond = ASC_setAPTitles(m_params, m_localServer.aetitle.toUtf8().data(), m_remoteServer.aetitle.toUtf8().data(), NULL);
        if(cond.bad())
        {
            qDebug() << "set APTitles error : " << cond.text();
            return -1;
        }

        cond = ASC_setPresentationAddresses(m_params, m_localServer.hostname.toUtf8().data(), QString(m_remoteServer.hostname + QString(":") + QString::number(m_remoteServer.port)).toUtf8().data());
        if(cond.bad())
        {
            qDebug() << "Presentation addresses error: " << cond.text();
            return -1;
        }

        const char* transferSyntax[] = {UID_LittleEndianImplicitTransferSyntax};
        cond = ASC_addPresentationContext(m_params, 1, UID_VerificationSOPClass, transferSyntax, 1);
        if(cond.bad())
        {
            qDebug() << "Add presentation context error : " << cond.text();
            return -1;
        }

        cond = ASC_requestAssociation(m_net, m_params, &m_assoc);
        if(cond.bad())
        {
            qDebug() << "Request association error : " << cond.text();
            return -1;
        }

        if(ASC_countAcceptedPresentationContexts(m_params) == 0)
        {
            qDebug() << "Count accepted presentation contexts error : " << cond.text();
            return -1;
        }

        DIC_US id = m_assoc->nextMsgID++;
        DIC_US status;
        cond = DIMSE_echoUser(m_assoc, id, DIMSE_BLOCKING, 0, &status, NULL);
        if(cond == EC_Normal)
        {
            cond = ASC_releaseAssociation(m_assoc);
            if(cond.bad())
            {
                qDebug() << "Release association error : " << cond.text();
                return -1;
            }
        }
        else if(cond == DUL_PEERREQUESTEDRELEASE)
        {
            ASC_abortAssociation(m_assoc);
            if(cond.bad())
            {
                qDebug() << "Abort association error (peer request release): " << cond.text();
                return -1;
            }
        }
        else if(cond == DUL_PEERABORTEDASSOCIATION)
        {
            qDebug() << "Peer aborted association";
        }
        else
        {
            qDebug() << "Echo operation failed";
            ASC_abortAssociation(m_assoc);
            if(cond.bad())
            {
                qDebug() << "Abort association error (peer request release): " << cond.text();
                return -1;
            }
        }

        cond = ASC_destroyAssociation(&m_assoc);
        if(cond.bad())
        {
            qDebug() << "Destroy association error : " << cond.text();
            return -1;
        }

        cond = ASC_dropNetwork(&m_net);
        if(cond.bad())
        {
            qDebug() << "Drop network error : " << cond.text();
            return -1;
        }

        qDebug() << "Successful Echo operation";
        return 0;
    }

    qDebug() << "Server unavailable";
    return -1;
}

QList<QMap<DcmTagKey, QString>> dcmPACS::cFind(const QMap<DcmTagKey, QString> &filters)
{
    dcmFindCallbacks callb(filters); 
    if(isServerAvailable(m_remoteServer.hostname, m_remoteServer.port))
    {
        // patientLevelAttributes transmis dans la requête, même key dont value vide
        OFList<OFString> keysRQ;
        for (DcmTagKey key : filters.keys())
        {
            keysRQ.push_back(QString(key.toString().c_str() + QString("=") + filters.value(key)).toUtf8().data());
        }

        DcmFindSCU findscu;
        OFCondition cond = findscu.initializeNetwork(30);
        if(cond.good())
        {
            QString queryInfoModel = UID_FINDPatientRootQueryRetrieveInformationModel;
            cond = findscu.performQuery(m_remoteServer.hostname.toUtf8().data(),
                                        m_remoteServer.port,
                                        m_localServer.aetitle.toUtf8().data(),
                                        m_remoteServer.aetitle.toUtf8().data(),
                                        queryInfoModel.toStdString().c_str(),
                                        EXS_Unknown, DIMSE_BLOCKING, 0, ASC_DEFAULTMAXPDU,
                                        false, false, 1, FEM_none, -1, &keysRQ, &callb);
            
            cond = findscu.dropNetwork();
            if(cond.bad())
            {
                qDebug() << "Drop network error";
            }
        }
    }
    
    return callb.getResponsesList();
}

bool dcmPACS::cMove(int pi_requestId, const QString &queryLevel, const QString &key)
{
    bool bRes = false; 
    QStringList data(key);

    if(m_temporaryDir.isValid())
    {
        QDir dir(m_temporaryDir.path());

        auto mover = new dcmMoveScu(data, dir.path(), queryLevel);
        mover->setLocalParameters(m_localServer.aetitle, m_localServer.hostname, m_localServer.port);
        mover->setRemoteParameters(m_remoteServer.aetitle, m_remoteServer.hostname, m_remoteServer.port);
    
        m_RequestIdMap[pi_requestId] = mover;

        QObject::connect(mover, &dcmMoveScu::moveFinished, this, [=](const QString &path, const QString &serieUid, int number)
        {
            emit moveProgress(pi_requestId, medAbstractSource::eRequestStatus::finish, path);
        });

        QObject::connect( mover, &dcmMoveScu::moveInProgress, this,  [=](const QString &message)
        {
            emit moveProgress(pi_requestId, medAbstractSource::eRequestStatus::pending);
        });

        QObject::connect(mover, &dcmMoveScu::moveFailed, this, [=]()
        {
            emit moveProgress(pi_requestId, medAbstractSource::eRequestStatus::faild);
        });

        m_FifoMover->addRequest(pi_requestId, mover);

        bRes = true;
    }

    return bRes;
}




void dcmPACS::sendPending(int requestId, medAbstractSource::eRequestStatus status)
{
    emit moveProgress(requestId,  medAbstractSource::eRequestStatus::pending);
}

void dcmPACS::stopMove(int pi_requestId)
{
    if(m_RequestIdMap.contains(pi_requestId))
    {
        auto mover = m_RequestIdMap[pi_requestId];
        mover->stopMoveNetwork();
        m_RequestIdMap.remove(pi_requestId);
        delete mover;
    }
}


