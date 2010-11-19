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

    void setTitle(QString title) {
        m_title = title;
    }

    void setIp(QString ip) {
        m_ip = ip;
    }
    
    void setPort(unsigned int port) {
        m_port = port;
    }

    QString title(void) {
        return m_title;
    }

    QString ip(void) {
        return m_ip;
    }
    
    inline unsigned int port(void) {
        return m_port;
    }

    void convert( dcmtkNode* node);

    QVector<medAbstractPacsResultDataset*> getResultDatasetContainer(void);

protected:
    QString  m_title;
    QString  m_ip;
    unsigned int m_port;

private:
    QVector<medAbstractPacsResultDataset*> m_copyCont;
};


#endif //DCMTKPACSFINDSCU_H
