#include "SpeedTest.h"

#include "dcmtkFindScu.h"
#include "dcmtkMoveScu.h" 

SpeedTest::SpeedTest(dcmtkNode host, dcmtkNode peer)
{
	hostNode = host;
	peerNode = peer;

	m_findScu = new dcmtkFindScu();
	m_moveScu = new dcmtkMoveScu();
}

SpeedTest::~SpeedTest()
{
	delete m_findScu;
	delete m_moveScu;
}

int SpeedTest::moveStudyLevel(std::string patientName)
{
	m_findScu->clearAllQueryAttributes();
	m_findScu->setQueryLevel(dcmtkFindScu::STUDY);
    m_findScu->addQueryAttribute(0x0010,0x0010,patientName.c_str()); // patient name
    m_findScu->addQueryAttribute(0x0008,0x0030,"\0"); // study date
    m_findScu->addQueryAttribute(0x0008,0x0050,"\0"); // accession no
    m_findScu->addQueryAttribute(0x0008,0x0061,"\0"); // modalities in study
    m_findScu->addQueryAttribute(0x0008,0x0090,"\0"); // ref physician
    m_findScu->addQueryAttribute(0x0008,0x1030,"\0"); // study description
    m_findScu->addQueryAttribute(0x0010,0x0020,"\0"); // patient ID
    m_findScu->addQueryAttribute(0x0010,0x0030,"\0"); // patient BD
    m_findScu->addQueryAttribute(0x0010,0x0040,"\0"); // sex
    m_findScu->addQueryAttribute(0x0020,0x000D,"\0"); // studyInstanceUID
    m_findScu->addQueryAttribute(0x0020,0x0010,"\0"); // study ID
	
	dcmtkContainer<dcmtkNode*>* resNodeCont = m_findScu->getNodeContainer();
    dcmtkNode* myNode = resNodeCont->getFirst();
    
    while (resNodeCont->isValid())
    {
        dcmtkContainer<dcmtkResultDataset*>* resCont = myNode->getResultDatasetContainer();
        dcmtkResultDataset* resDs = resCont->getFirst();
        
        while ( resCont->isValid())
        {
            // move on study level            
            move(0x0020, 0x000D, resDs->getStudyInstanceUID()); // patient name
            resDs = resCont->getNext();
        }
        myNode = resNodeCont->getNext();
    }
	
	return 0;
}

int SpeedTest::moveImageLevel(std::string patientName)
{

	return 0;
}

void SpeedTest::move(int group, int elem, const char* query)
{   
      
    // set up search criteria
    m_moveScu->clearAllQueryAttributes();
    m_moveScu->addQueryAttribute(group, elem, query);
	m_moveScu->useBuildInStoreSCP(true);
    
    // send the move request using the search crits
	m_moveScu->sendMoveRequest(peerNode.title().c_str(),peerNode.ip().c_str(),peerNode.port(),hostNode.title().c_str(), hostNode.ip().c_str(), hostNode.port());
}
