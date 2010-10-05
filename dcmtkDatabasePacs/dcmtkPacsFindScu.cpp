#include "dcmtkPacsFindScu.h"
#include "medPacs/medAbstractPacsFindScu.h"
#include "medPacs/medAbstractPacsFactory.h"
#include "medPacs/medAbstractPacsNode.h"
#include "dcmtkPacsNode.h"

QString dcmtkPacsFindScu::description(void) const
{
    return "dcmtkFindScu";
}

bool dcmtkPacsFindScu::registered(void)
{
    return medAbstractPacsFactory::instance()->registerFindScuType("dcmtkFindScu", createDcmtkFindScu);
}

int dcmtkPacsFindScu::sendFindRequest( const char* peerTitle, const char* peerIP, unsigned int peerPort, const char* ourTitle, const char* ourIP, unsigned int ourPort )
{
    return scu.sendFindRequest(peerTitle, peerIP, peerPort, ourTitle, ourIP, ourPort);
}

bool dcmtkPacsFindScu::addQueryAttribute(int group, int elem, const char* value)
{
    return scu.addQueryAttribute(group,elem, value);
}

void dcmtkPacsFindScu::setQueryLevel( eQueryLevel level )
{
    scu.setQueryLevel(static_cast<dcmtkBaseScu::eQueryLevel>(level));
}

void dcmtkPacsFindScu::clearAllQueryAttributes()
{
    scu.clearAllQueryAttributes();
}

#include <dtkCore/dtkGlobal.h>

QVector<medAbstractPacsNode*> dcmtkPacsFindScu::getNodeContainer( void )
{
    dcmtkContainer<dcmtkNode*>* container = scu.getNodeContainer();

    qDebug() << "Initial size" << container->size();

    m_copyCont.clear();

    for (int i = 0; i < container->size(); i++) {

        qDebug() << DTK_PRETTY_FUNCTION << i;

        dcmtkPacsNode* ds = new dcmtkPacsNode;
        ds->convert(container->getAtPos(i));
        m_copyCont << ds;
    }

    qDebug() << "Final size" << m_copyCont.count();

    return m_copyCont;
}

medAbstractPacsFindScu *createDcmtkFindScu(void) 
{
    return new dcmtkPacsFindScu;
}
