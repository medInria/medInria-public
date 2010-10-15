#ifndef DCMTKPACSNODE_H
#define DCMTKPACSNODE_H


#include <QVector>
#include "dcmtkNode.h"
#include <medPacs/medAbstractPacsNode.h>

class medAbstractPacsResultDatset;
class dcmtkPacsResultDataset;

class dcmtkPacsNode : public medAbstractPacsNode
{
    Q_OBJECT

public:
    inline void setTitle(std::string title) {
        m_title = title;
    }

    inline void setIp(std::string ip) {
        m_ip = ip;
    }
    
    inline void setPort(unsigned int port) {
        m_port = port;
    }

    inline std::string title(void) {
        return m_title;
    }

    inline std::string ip(void) {
        return m_ip;
    }
    
    inline unsigned int port(void) {
        return m_port;
    }

    void convert( dcmtkNode* node);

    QVector<medAbstractPacsResultDataset*> getResultDatasetContainer(void);

protected:
    std::string  m_title;
    std::string  m_ip;
    unsigned int m_port;

private:
    QVector<medAbstractPacsResultDataset*> m_copyCont;
};


#endif //DCMTKPACSFINDSCU_H
