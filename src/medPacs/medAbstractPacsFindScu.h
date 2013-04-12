/* medAbstractPacsFindScu.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 11:30:13 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct  5 11:30:15 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include "medAbstractPacsBaseScu.h"
#include "medPacsExport.h"

class MEDPACS_EXPORT medAbstractPacsFindScu : public medAbstractPacsBaseScu
{
    Q_OBJECT

public:
             medAbstractPacsFindScu();
    virtual ~medAbstractPacsFindScu();

    virtual int  sendFindRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, const char* ourTitle, const char* ourIP, unsigned int ourPort);

};


