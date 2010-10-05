#ifndef DCMTKNODE_H
#define DCMTKNODE_H

// inc
#include <iostream>

#include "dcmtkResultDataset.h"


class dcmtkNode
{
public:

    struct dcmtkConnectionData
    {
        std::string  title;
        std::string  ip;
        unsigned int port;
    };

    void addConnData(dcmtkConnectionData cdata);

    dcmtkConnectionData getConnData();

    dcmtkContainer<dcmtkResultDataset*>* getResultDatasetContainer();

protected:

    dcmtkConnectionData                    m_connData;
    dcmtkContainer<dcmtkResultDataset*>    m_dsContainer;

};

#endif DCMTKNODE_H