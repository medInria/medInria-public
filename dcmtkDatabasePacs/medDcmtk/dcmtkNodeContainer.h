#ifndef DCMTKNODECONTAINER_H
#define DCMTKNODECONTAINER_H

// inc
#include "dcmtkFindDataset.h"
#include "dcmtkContainer.h"

struct dcmtkConnectionData
{
    std::string title;
    std::string ip;
    unsigned int port;
};

class dcmtkResultDatasetContainer : public dcmtkContainer<dcmtkFindDataset*>
{
};

class dcmtkNode
{

protected:

    dcmtkConnectionData                    m_connData;
    dcmtkResultDatasetContainer            m_dsContainer;

public:

    void addConnData(dcmtkConnectionData cdata)
    {
        m_connData = cdata;
    };

    dcmtkConnectionData getConnData()
    {
        return m_connData;
    };

    dcmtkResultDatasetContainer* getResultDatasetContainer()
    {
        return &m_dsContainer;
    };

};

class dcmtkNodeContainer : public dcmtkContainer<dcmtkNode*>
{
};

#endif DCMTKNODECONTAINER_H