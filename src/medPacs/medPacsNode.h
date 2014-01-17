/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractPacsNode.h>

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

        QString title() {
            return m_title;
        }

        QString ip() {
            return m_ip;
        }

        unsigned int port() {
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


