#include "SpeedTest.h"

#include "dcmtkFindScu.h"
#include "dcmtkMoveScu.h"

#include <QTime>

//---------------------------------------------------------------------------------------------

SpeedTest::SpeedTest(dcmtkNode host, dcmtkNode peer)
{
    hostNode = host;
    peerNode = peer;

    m_findScu = new dcmtkFindScu();
    m_moveScu = new dcmtkMoveScu();
}

//---------------------------------------------------------------------------------------------

SpeedTest::~SpeedTest()
{
    delete m_findScu;
    delete m_moveScu;
}

//---------------------------------------------------------------------------------------------

int SpeedTest::moveStudyLevel(std::string patientName)
{
    QTime timer;
    timer.start();
    std::cout << "************************ STUDY *******************" << std::endl; 

    m_findScu->clearAllQueryAttributes();
    m_findScu->setQueryLevel(dcmtkFindScu::STUDY);
    m_findScu->addQueryAttribute(0x0010,0x0010,patientName.c_str()); // patient name
    m_findScu->addQueryAttribute(0x0020,0x000D,"\0"); // studyInstanceUID

    m_findScu->sendFindRequest(peerNode.title().c_str(),peerNode.ip().c_str(),peerNode.port(),
                               hostNode.title().c_str(),hostNode.ip().c_str(),hostNode.port());
    
    dcmtkContainer<dcmtkNode*>* resNodeCont = m_findScu->getNodeContainer();
    dcmtkNode* myNode = resNodeCont->getFirst();

    while (resNodeCont->isValid())
    {
        dcmtkContainer<dcmtkResultDataset*>* resCont = myNode->getResultDatasetContainer();
        dcmtkResultDataset* resDs = resCont->getFirst();
        
        while ( resCont->isValid())
        {
            // move on study level            
            move(0x0020, 0x000D, resDs->getStudyInstanceUID()); // study inst uid
            resDs = resCont->getNext();
        }
        myNode = resNodeCont->getNext();
    }

    // manually release assoc
    m_moveScu->releaseAssociation();
    return timer.elapsed();
}

//---------------------------------------------------------------------------------------------

int SpeedTest::moveSeriesLevel( std::string patientName )
{
    QTime timer;
    timer.start();
    std::cout << "************************ SERIES *******************" << std::endl; 

    m_findScu->clearAllQueryAttributes();
    m_findScu->setQueryLevel(dcmtkFindScu::STUDY);
    m_findScu->addQueryAttribute(0x0010,0x0010,patientName.c_str()); // patient name
    m_findScu->addQueryAttribute(0x0020,0x000D,"\0"); // studyInstanceUID

    m_findScu->sendFindRequest(peerNode.title().c_str(),peerNode.ip().c_str(),peerNode.port(),
                               hostNode.title().c_str(),hostNode.ip().c_str(),hostNode.port());

    dcmtkContainer<dcmtkNode*>* resNodeCont = m_findScu->getNodeContainer();
    dcmtkNode* myNode = resNodeCont->getFirst();

    
    while (resNodeCont->isValid())
    {
        dcmtkContainer<dcmtkResultDataset*>* resCont = myNode->getResultDatasetContainer();
        dcmtkResultDataset* resDs = resCont->getFirst();

        while ( resCont->isValid())
        {
            dcmtkFindScu* findSeries = new dcmtkFindScu();
            findSeries->setQueryLevel(dcmtkFindScu::SERIES);
            findSeries->addQueryAttribute(0x0020,0x000D,resDs->getStudyInstanceUID());
            findSeries->addQueryAttribute(0x0020,0x000E,"\0"); // series instance UID)
            findSeries->sendFindRequest(peerNode.title().c_str(),peerNode.ip().c_str(),peerNode.port(),
                hostNode.title().c_str(),hostNode.ip().c_str(),hostNode.port());
            dcmtkContainer<dcmtkNode*>* resNodeContSeries = findSeries->getNodeContainer();
            dcmtkNode* myNodeSeries = resNodeContSeries->getFirst();

            if (resNodeContSeries->isValid())
            {
                dcmtkContainer<dcmtkResultDataset*>* resContSeries = myNodeSeries->getResultDatasetContainer();
                dcmtkResultDataset* resDsSeries = resContSeries->getFirst();

                while (resContSeries->isValid())
                {
                    // now do the move on each series
                    move(0x0020,0x000E,resDsSeries->getSeriesInstanceUID());
                    resDsSeries = resContSeries->getNext();
                }
                myNodeSeries = resNodeContSeries->getNext();
            }

            delete findSeries;
            resDs = resCont->getNext();
        }
        myNode = resNodeCont->getNext();
    }

    // manually release assoc
    m_moveScu->releaseAssociation();
    return timer.elapsed();

}

//---------------------------------------------------------------------------------------------

int SpeedTest::moveImageLevel(std::string patientName, bool useBatch)
{
    QTime timer;
    if (useBatch)
        std::cout << "************************ IMAGE BATCH**************" << std::endl; 
    else
        std::cout << "************************ IMAGE ITERATIVE**********" << std::endl; 

    timer.start();

    m_findScu->clearAllQueryAttributes();
    m_findScu->setQueryLevel(dcmtkFindScu::STUDY);
    m_findScu->addQueryAttribute(0x0010,0x0010,patientName.c_str()); // patient name
    m_findScu->addQueryAttribute(0x0020,0x000D,"\0"); // studyInstanceUID

    m_findScu->sendFindRequest(peerNode.title().c_str(),peerNode.ip().c_str(),peerNode.port(),
                               hostNode.title().c_str(),hostNode.ip().c_str(),hostNode.port());

    dcmtkContainer<dcmtkNode*>* resNodeCont = m_findScu->getNodeContainer();
    dcmtkNode* myNode = resNodeCont->getFirst();

    
    while (resNodeCont->isValid())
    {
        dcmtkContainer<dcmtkResultDataset*>* resCont = myNode->getResultDatasetContainer();
        dcmtkResultDataset* resDs = resCont->getFirst();

        while ( resCont->isValid())
        {
            dcmtkFindScu* findSeries = new dcmtkFindScu();
            findSeries->setQueryLevel(dcmtkFindScu::SERIES);
            findSeries->addQueryAttribute(0x0020,0x000D,resDs->getStudyInstanceUID());
            findSeries->addQueryAttribute(0x0020,0x000E,"\0"); // series instance UID)
            findSeries->sendFindRequest(peerNode.title().c_str(),peerNode.ip().c_str(),peerNode.port(),
                                        hostNode.title().c_str(),hostNode.ip().c_str(),hostNode.port());
            dcmtkContainer<dcmtkNode*>* resNodeContSeries = findSeries->getNodeContainer();
            dcmtkNode* myNodeSeries = resNodeContSeries->getFirst();

            if (resNodeContSeries->isValid())
            {
                dcmtkContainer<dcmtkResultDataset*>* resContSeries = myNodeSeries->getResultDatasetContainer();
                dcmtkResultDataset* resDsSeries = resContSeries->getFirst();

                while (resContSeries->isValid())
                {
                    dcmtkFindScu* findImage = new dcmtkFindScu();
                    findImage->setQueryLevel(dcmtkFindScu::IMAGE);
                    findImage->addQueryAttribute(0x0020,0x000E,resDsSeries->getSeriesInstanceUID());
                    findImage->addQueryAttribute(0x0008,0x0018,"\0"); // SOP instance UID)
                    findImage->sendFindRequest(peerNode.title().c_str(),peerNode.ip().c_str(),peerNode.port(),
                                               hostNode.title().c_str(),hostNode.ip().c_str(),hostNode.port());

                    dcmtkContainer<dcmtkNode*>* resNodeContImage = findImage->getNodeContainer();
                    dcmtkNode* nodeImage = resNodeContImage->getFirst();

                    if (resNodeContImage->isValid())
                    {
                        std::string myQueryValue;
                        dcmtkContainer<dcmtkResultDataset*>* resContImage = nodeImage->getResultDatasetContainer();
                        dcmtkResultDataset* resDsImage = resContImage->getFirst();

                        if (useBatch)
                        {
                            while (resContImage->isValid())
                            {
                                // concatenate the uids in reverse order (seems not to have an effect on the pacs)
                                myQueryValue = std::string(resDsImage->getSOPInstanceUID()) +"\\"+ myQueryValue;
                                resDsImage = resContImage->getNext();
                            }
                            // now do a batch move on all image
                            move(0x0008,0x0018,myQueryValue.c_str());
                        }else
                        {
                            while (resContImage->isValid())
                            {
                                // now do the move on each single image
                                move(0x0008,0x0018,resDsImage->getSOPInstanceUID());
                                resDsImage = resContImage->getNext();
                            }
                        }
                    }
                    
                    delete findImage;
                    resDsSeries = resContSeries->getNext();
                }
                
                myNodeSeries = resNodeContSeries->getNext();
            }

            delete findSeries;
            resDs = resCont->getNext();
        }
        myNode = resNodeCont->getNext();
    }
    // manually release assoc
    m_moveScu->releaseAssociation();
    return timer.elapsed();
}

//---------------------------------------------------------------------------------------------

void SpeedTest::move(int group, int elem, const char* query)
{   
    
    // set up search criteria
    m_moveScu->clearAllQueryAttributes();
    m_moveScu->addQueryAttribute(group, elem, query);
    m_moveScu->useBuildInStoreSCP(true);
    m_moveScu->skipWritingFiles(true);
    m_moveScu->keepAssociationOpen(true);
    
    // find out which query level should be used
    switch(elem)
    {
    case 0x0018:
        m_moveScu->setQueryLevel(dcmtkMoveScu::IMAGE);
        break;
    case 0x000E:
        m_moveScu->setQueryLevel(dcmtkMoveScu::SERIES);
        break;
    case 0x000D:
        m_moveScu->setQueryLevel(dcmtkMoveScu::STUDY);
        break;
    default:
        m_moveScu->setQueryLevel(dcmtkMoveScu::STUDY);
    }

    // send the move request using the search crits
    m_moveScu->sendMoveRequest(peerNode.title().c_str(),peerNode.ip().c_str(),peerNode.port(),
                               hostNode.title().c_str(),hostNode.ip().c_str(),hostNode.port());
}

//---------------------------------------------------------------------------------------------
