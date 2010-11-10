#ifndef DCMTKNODE_H
#define DCMTKNODE_H

// inc
#include <iostream>

#include "dcmtkResultDataset.h"


class dcmtkNode
{
public:

    dcmtkNode(){};

    dcmtkNode(dcmtkNode& node);

    void setTitle(std::string title) {
        m_title = title;
    }

    void setIp(std::string ip) {
        m_ip = ip;
    }
    
    void setPort(unsigned int port) {
        m_port = port;
    }

    std::string title(void) {
        return m_title;
    }

    std::string ip(void) {
        return m_ip;
    }
    
    unsigned int port(void) {
        return m_port;
    }

    dcmtkContainer<dcmtkResultDataset*>* getResultDatasetContainer(void);

protected:

    dcmtkContainer<dcmtkResultDataset*>  m_dsContainer;
    std::string  m_title;
    std::string  m_ip;
    unsigned int m_port;
};

#endif //DCMTKNODE_H
