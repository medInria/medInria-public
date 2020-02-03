#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractPacsBaseScu.h>
#include <medPacsExport.h>

class MEDPACS_EXPORT medAbstractPacsEchoScu : public medAbstractPacsBaseScu
{
    Q_OBJECT

public:
    medAbstractPacsEchoScu() = default;
    ~medAbstractPacsEchoScu() override = default;

    virtual int  sendEchoRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort,
                                 const char* ourTitle, const char* ourIP, unsigned int ourPort);
};
