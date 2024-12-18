/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QDir>

#include <medNotif.h>
#include <medNewLogger.h>

#include <dcmMoveScu.h>
#include <medAbstractSource.h>

#include <dcmtk/dcmnet/dimse.h>

static DcmDataset *overrideKeys = NULL;

dcmMoveScu::dcmMoveScu(const QStringList &data, const QString &outputDir, const QString &queryLevel, QObject * parent) : QThread(parent)
{
    m_data = data;
    m_outputDir = outputDir;
    m_queryLevel = queryLevel;
}

dcmMoveScu::~dcmMoveScu()
{
}

void dcmMoveScu::setLocalParameters(const QString &aetitle, const QString &hostName, const int &port)
{
    m_localAetitle = aetitle;
    m_localHostName = hostName;
    m_localPort = port;
}

void dcmMoveScu::setRemoteParameters(const QString &aetitle, const QString &hostName, const int &port)
{
    m_remoteAetitle = aetitle;
    m_remoteHostName = hostName;
    m_remotePort = port;
}

QString dcmMoveScu::getOutputDir()
{
    return m_outputDir;
}


void dcmMoveScu::run()
{
    for(int i=0; i < m_data.size(); i++)
    {
        const QDir subDir(m_outputDir + "/" + m_data.at(i));
        if(!subDir.exists())
        {
            QDir(m_outputDir).mkdir(m_data.at(i));
        }

        m_outputDirectory = subDir.absolutePath().toUtf8().constData();
        mInfo << "Path where DICOM files had to be downloaded : " << m_outputDirectory.c_str();

        OFCondition cond = moveRequest(m_data.at(i));
        if(cond.good())
        {
            emit moveFinished(subDir.absolutePath(), m_data.at(i), i);
        }
        else
        {
            emit moveFailed();
        }
    }
}




void dcmMoveScu::addOverrideKey(const QString &key)
{
    char *overKey = key.toLatin1().data();

    // try to parse group and element number
    unsigned int g = 0xffff;
    unsigned int e = 0xffff;
    int n = sscanf(overKey, "%x,%x=", &g, &e);
    
    int posEq = key.indexOf("=");

    QString levelParameter, levelValue;
    OFString msg;
    char msg2[200];
    if(n < 2)
    {  // if at least no tag could be parsed

        // if value is given, extract it (and dictionary name)
        if(posEq != key.size())
        {
            levelParameter = key.left(posEq);
            levelValue = key.mid(posEq+1);
        }
        else
        { // no value given, just dictionary name
            levelParameter = overKey; // only dictionary name given (without value)
        } 

        // try to lookup in dictionary
        DcmTagKey tagKey(0xffff,0xffff);
        const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
        const DcmDictEntry *dictEntry = globalDataDict.findEntry(levelParameter.toUtf8().data());
        dcmDataDict.rdunlock();
        if(dictEntry!=NULL)
        {
            // found dictionary name, copy group and element number
            tagKey = dictEntry->getKey();
            g = tagKey.getGroup();
            e = tagKey.getElement();
        }
        else
        {
            // not found in dictionary
            msg = "Bad key format or dictionary name not found in dictionary: ";
            msg += levelParameter.toUtf8().data();
            mWarning << "[addOverrideKey] " << msg.c_str();
        }
    }
    else
    { // tag could be parsed, copy value if it exists
        if(posEq != key.size())
        {
            levelValue = key.mid(posEq+1);
        }
    }

    DcmTag tag(OFstatic_cast(Uint16, g), OFstatic_cast(Uint16, e));
    if(tag.error() != EC_Normal)
    {
        sprintf(msg2, "unknown tag: (%04x,%04x)", g, e);
        mWarning << "[addOverrideKey] " << msg2;
    }

    DcmElement *elem = DcmItem::newDicomElement(tag);
    if(elem == NULL)
    {
        sprintf(msg2, "cannot create element for tag: (%04x,%04x)", g, e);
        mWarning << "[addOverrideKey] " << msg2;
    }
    if(!levelValue.isEmpty())
    {
        if(elem->putString(levelValue.toUtf8().data()).bad())
        {
            sprintf(msg2, "cannot put tag value: (%04x,%04x)=", g, e);
            msg = msg2;
            msg += levelValue.toUtf8().data();
            mWarning << "[addOverrideKey] " << msg.c_str();
        }
    }

    if(overrideKeys == NULL)
    {
        overrideKeys = new DcmDataset;
    }

    if(overrideKeys->insert(elem, OFTrue).bad())
    {
        sprintf(msg2, "cannot insert tag: (%04x,%04x)", g, e);
        mWarning << "[addOverrideKey] " << msg2;
    }
}

void dcmMoveScu::substituteOverrideKeys(DcmDataset *dataset)
{
    if(overrideKeys == NULL)
    {
        return;
    }

    DcmDataset overKeys(*overrideKeys);

    for(unsigned long i=0; i <= overKeys.card(); i++)
    {
        DcmElement *elem = overKeys.remove(OFstatic_cast(unsigned long, 0));
        dataset->insert(elem, OFTrue);
    }
}




void dcmMoveScu::storeSCPCallback(void *storeCallback, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ *rq, char *imageFileName, DcmDataset **imageDataset, T_DIMSE_C_StoreRSP *rsp, DcmDataset **statusDetail)
{
    dcmMoveScu *self = reinterpret_cast<dcmMoveScu *>(storeCallback);
    
    if(progress->state == DIMSE_StoreEnd)
    {
        *statusDetail = NULL;                                   // bit preserving && ignore
        if((imageDataset != NULL) && (*imageDataset) != NULL && !OFFalse && !OFFalse)
        {
            OFString dcmFileName;
            StoreCallbackData *storeCbData = OFstatic_cast(StoreCallbackData*, storeCallback);
            OFStandard::combineDirAndFilename(dcmFileName, storeCbData->outputDirectory, storeCbData->imageFileName, OFTrue);
            if(OFStandard::fileExists(dcmFileName))
            {
                mWarning << "[STORE SCP] DICOM file already downloaded, Overwriting " << dcmFileName.c_str();
            }

            E_TransferSyntax xfer = (*imageDataset)->getOriginalXfer();
            OFCondition cond = storeCbData->dcmFile->saveFile(dcmFileName.c_str(), xfer, EET_ExplicitLength, EGL_recalcGL, EPD_withoutPadding, OFstatic_cast(Uint32, 0), OFstatic_cast(Uint32, 0), EWM_createNewMeta);
            if(cond.bad())
            {
                mCritical << "[STORE SCP] Download error, Cannot save DICOM file" << dcmFileName.c_str();
            }

            DIC_UI sopClass;
            DIC_UI sopInstance;
            if((rsp->DimseStatus == STATUS_Success))
            {
                if(!DU_findSOPClassAndInstanceInDataSet(*imageDataset, sopClass, sizeof(sopClass), sopInstance, sizeof(sopInstance), OFFalse))
                {
                    rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
                    OFString temp_str;
                }
                else if(strcmp(sopClass, rq->AffectedSOPClassUID) != 0 || strcmp(sopInstance, rq->AffectedSOPInstanceUID) != 0)
                {
                    rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                }
            }
        }
    }
    else if(progress->state == DIMSE_StoreProgressing)
    {
        emit self->moveInProgress(QString("Move in progress"));
    }
}

OFCondition dcmMoveScu::storeSCP(void *subOpCallback, T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID pcId)
{
    dcmMoveScu *self = reinterpret_cast<dcmMoveScu *>(subOpCallback);
    
    OFCondition cond;
    T_DIMSE_C_StoreRQ *rq = &msg->msg.CStoreRQ;

    char imageFileName[2048];
    sprintf(imageFileName, "%s.%s", dcmSOPClassUIDToModality(rq->AffectedSOPClassUID), rq->AffectedSOPInstanceUID);

    DcmFileFormat file;
    StoreCallbackData callbackData;
    callbackData.assoc = assoc;
    callbackData.imageFileName = imageFileName;
    callbackData.dcmFile = &file;
    callbackData.outputDirectory = self->m_outputDirectory;
    
    if(assoc && assoc->params)
    {
        const char *aet = assoc->params->DULparams.callingAPTitle;
        if(aet)
        {
            file.getMetaInfo()->putAndInsertString(DCM_SourceApplicationEntityTitle, aet);
        }
    }

    DcmDataset *dcmSet = file.getDataset();
    // perform C-STORE SCP response
    cond = DIMSE_storeProvider(assoc, pcId, rq, NULL, OFTrue, &dcmSet, storeSCPCallback, OFreinterpret_cast(void*, &callbackData), DIMSE_BLOCKING, 0);
    if(cond.bad())
    {
        mCritical << "STORE SCP Response Status : Failed";
    }

    return cond;
}

OFCondition dcmMoveScu::echoSCP(T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID pcId)
{
    // perform C-ECHO SCP response
    OFCondition cond = DIMSE_sendEchoResponse(assoc, pcId, &msg->msg.CEchoRQ, STATUS_Success, NULL);
    if(cond.bad())
    {
        mCritical << "ECHO SCP Response Status : Failed";
    }

    return cond;
}

void dcmMoveScu::subOperationCallback(void *subCallback, T_ASC_Network *subNet, T_ASC_Association **subAssoc)
{
    dcmMoveScu *self = reinterpret_cast<dcmMoveScu *>(subCallback);
    
    if(subNet == NULL)
    {
        return;
    }

    if(*subAssoc == NULL)
    { // negotiate association
        const char *abstractSyntaxes[] = {UID_VerificationSOPClass};
        const char *transferSyntaxes[] = {NULL, NULL, NULL};
        if(gLocalByteOrder == EBO_LittleEndian)
        {
            transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        }
        else
        {
            transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;

        OFCondition cond = ASC_receiveAssociation(subNet, subAssoc, ASC_DEFAULTMAXPDU);
        if(cond.good())
        {
            cond = ASC_acceptContextsWithPreferredTransferSyntaxes((*subAssoc)->params, abstractSyntaxes, 1, transferSyntaxes, 3);
            if(cond.good())
            {
                cond = ASC_acceptContextsWithPreferredTransferSyntaxes((*subAssoc)->params, dcmAllStorageSOPClassUIDs, numberOfDcmAllStorageSOPClassUIDs, transferSyntaxes, 3);
            }
        }

        if(cond.good())
        {
            cond = ASC_acknowledgeAssociation(*subAssoc);
        }

        if(cond.bad())
        {
            ASC_dropAssociation(*subAssoc);
            ASC_destroyAssociation(subAssoc);
        }
    }
    else
    { // SCP response
        T_DIMSE_Message msg;
        T_ASC_PresentationContextID pcId;

        OFCondition cond = DIMSE_receiveCommand(*subAssoc, DIMSE_BLOCKING, 0, &pcId, &msg, NULL);

        if(cond == EC_Normal)
        {
            switch(msg.CommandField)
            {
                case DIMSE_C_ECHO_RQ:
                    cond = echoSCP(*subAssoc, &msg, pcId);
                    break;
                case DIMSE_C_STORE_RQ:
                    cond = storeSCP(subCallback, *subAssoc, &msg, pcId);
                    break;
                default:
                    cond = DIMSE_BADCOMMANDTYPE;
                    break;
            }
        }

        if(cond == DUL_PEERREQUESTEDRELEASE)
        {
            cond = ASC_acknowledgeAssociation(*subAssoc);
            ASC_dropAssociation(*subAssoc);
            ASC_destroyAssociation(subAssoc);
        }
        else if(cond == DUL_PEERABORTEDASSOCIATION)
        {
        }
        else if(cond != EC_Normal)
        {
            cond = ASC_abortAssociation(*subAssoc);
            ASC_dropAssociation(*subAssoc);
            ASC_destroyAssociation(subAssoc);
        }
    }
}




void dcmMoveScu::moveCallback(void *callbackData, T_DIMSE_C_MoveRQ *request, int responseCount, T_DIMSE_C_MoveRSP *response)
{
    OFString str;
    DIMSE_dumpMessage(str, *response, DIMSE_INCOMING);
    mInfo << "C-MOVE response" << responseCount << ":" << str.c_str();
}


OFCondition dcmMoveScu::moveSCU(T_ASC_Association *assoc)
{
    T_DIMSE_C_MoveRQ rq;
    T_DIMSE_C_MoveRSP rsp;

    DcmFileFormat file;
    substituteOverrideKeys(file.getDataset());
    
    T_ASC_PresentationContextID pcId = ASC_findAcceptedPresentationContextID(assoc, UID_MOVEPatientRootQueryRetrieveInformationModel);
    if(pcId == 0)
    {
        return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    DIC_US msgId = assoc->nextMsgID++;
    rq.MessageID = msgId;
    OFStandard::strlcpy(rq.AffectedSOPClassUID, UID_MOVEPatientRootQueryRetrieveInformationModel, sizeof(rq.AffectedSOPClassUID));
    rq.Priority = DIMSE_PRIORITY_MEDIUM;
    rq.DataSetType = DIMSE_DATASET_PRESENT;

    ASC_getAPTitles(assoc->params, rq.MoveDestination, sizeof(rq.MoveDestination), NULL, 0, NULL, 0);

    // C-MOVE request message
    OFString temp_str;
    mInfo << "C-MOVE request message :" << DIMSE_dumpMessage(temp_str, rq, DIMSE_OUTGOING, NULL, pcId).c_str();


    DcmDataset *statusDetail = NULL;
    responseIdentifiers = NULL;
    // perform C-MOVE request
    OFCondition cond = DIMSE_moveUser(assoc, pcId, &rq, file.getDataset(), moveCallback, (void*) this, DIMSE_BLOCKING, 60, m_net, subOperationCallback, (void*) this, &rsp, &statusDetail, &responseIdentifiers, OFTrue);    

    // C-MOVE response message
    mInfo << "C-MOVE response message :" << DIMSE_dumpMessage(temp_str, rsp, DIMSE_INCOMING).c_str();


    // C-MOVE operation status
    if(rsp.DimseStatus == STATUS_Success)
    {
        medNotif::createNotif(notifLevel::success, "C-MOVE Request", "Status : " + QString(DU_cmoveStatusString(rsp.DimseStatus)));
    }
    else
    {
        medNotif::createNotif(notifLevel::error, "C-MOVE Request", "Status : " + QString(DU_cmoveStatusString(rsp.DimseStatus)));
    }

    if(statusDetail != NULL)
    {
        delete statusDetail;
    }

    if(responseIdentifiers != NULL)
    {
        delete responseIdentifiers;
    }

    return cond;
}

OFCondition dcmMoveScu::moveRequest(const QString &uid)
{
    m_assoc = NULL;
    m_net = NULL;
    m_params = NULL;

    if(m_queryLevel == "PATIENT")
    {
        addOverrideKey(QString("QueryRetrieveLevel=" + m_queryLevel));
        addOverrideKey(QString("PatientID=" + uid));

    }
    else if(m_queryLevel == "STUDY")
    {
        addOverrideKey(QString("QueryRetrieveLevel=" + m_queryLevel));
        addOverrideKey(QString("StudyInstanceUID=" + uid));

    }
    else if(m_queryLevel == "SERIES")
    {
        addOverrideKey(QString("QueryRetrieveLevel=" + m_queryLevel));
        addOverrideKey(QString("SeriesInstanceUID=" + uid));
    
    }  
    
    OFCondition cond = ASC_initializeNetwork(NET_ACCEPTORREQUESTOR, m_localPort, 60, &m_net);
    if(cond.bad())
    {
        medNotif::createNotif(notifLevel::error, "C-MOVE Request Error", "Network Initialization : " + QString(cond.text()));
        return cond;
    }

    cond = ASC_createAssociationParameters(&m_params, ASC_DEFAULTMAXPDU);
    if(cond.bad())
    {
        mCritical << "[C-MOVE Request Error] Create Parameters Association : " << cond.text();
        return cond;
    }

    cond = ASC_setAPTitles(m_params, m_localAetitle.toUtf8().data(), m_remoteAetitle.toUtf8().data(), NULL);
    if(cond.bad())
    {
        medNotif::createNotif(notifLevel::error, "C-MOVE Request Error", "Set AETitles : " + QString(cond.text()));
        return cond;
    }

    cond = ASC_setPresentationAddresses(m_params, m_localHostName.toUtf8().data(), QString(m_remoteHostName + QString(":") + QString::number(m_remotePort)).toUtf8().data());
    if(cond.bad())
    {
        mCritical << "[C-MOVE Request Error] Set Presentation Addresses : " << cond.text();
        return cond;
    }

    const char *transferSyntaxes[] = {NULL, NULL, NULL};
    if(gLocalByteOrder == EBO_LittleEndian)
    {
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
    }
    else
    {
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
    }
    transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;

    cond = ASC_addPresentationContext(m_params, 1, UID_FINDPatientRootQueryRetrieveInformationModel, transferSyntaxes, 3);
    cond = ASC_addPresentationContext(m_params, 3, UID_MOVEPatientRootQueryRetrieveInformationModel, transferSyntaxes, 3);
    if(cond.bad())
    {
        mCritical << "[C-MOVE Request Error] Add Presentation Context : " << cond.text();
        return cond;
    }

    cond = ASC_requestAssociation(m_net, m_params, &m_assoc);
    if(cond.bad())
    {
        mCritical << "[C-MOVE Request Error] Request Association : " << cond.text();
        return cond;
    }

    if(ASC_countAcceptedPresentationContexts(m_params) == 0)
    {
        mCritical << "[C-MOVE Request Error] Count Accepted Presentation Contexts : " << cond.text();
        return cond;
    }

    cond = moveSCU(m_assoc);
    if(cond == EC_Normal)
    {
        cond = ASC_releaseAssociation(m_assoc);
        if(cond.bad())
        {
            mCritical << "[C-MOVE Request Error] Release Association : " << cond.text();
            return cond;
        }
    }
    else if(cond == DUL_PEERREQUESTEDRELEASE)
    {
        ASC_abortAssociation(m_assoc);
        if(cond.bad())
        {
            mCritical << "[C-MOVE Request Error] Abort Association (Peer Requested Release) : " << cond.text();
            return cond;
        }
    }
    else if(cond == DUL_PEERABORTEDASSOCIATION)
    {
        mWarning << "[C-MOVE Request] Peer Aborted Association : " << cond.text();
    }
    else
    {
        ASC_abortAssociation(m_assoc);
        if(cond.bad())
        {
            mCritical << "[C-MOVE Request Error] Abort Association (Peer Requested Release) : " << cond.text();
            return cond;
        }
    }

    overrideKeys->clear();
    
    cond = ASC_destroyAssociation(&m_assoc);
    if(cond.bad())
    {
        mCritical << "[C-MOVE Request Error] Destroy Association : " << cond.text();
        return cond;
    }

    cond = ASC_dropNetwork(&m_net);
    if(cond.bad())
    {
        mCritical << "[C-MOVE Request Error] Drop Network : " << cond.text();
        return cond;
    }

    return cond;
}




void dcmMoveScu::stopMoveNetwork()
{
    if(this->isRunning())
    {
        ASC_dropNetwork(&m_net);
    }
}
