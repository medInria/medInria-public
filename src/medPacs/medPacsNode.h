#ifndef MEDPACSNODE_H
#define MEDPACSNODE_H

#include "medAbstractPacsNode.h"

class medPacsNode : public medAbstractPacsNode
{

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

        unsigned int port(void) {
            return m_port;
        }
		
		bool operationSuccessful(){
			return m_result;
			}

    protected:

        QString  m_title;
        
        QString  m_ip;
        
        unsigned int m_port;
		
		bool m_result;

};

#endif
