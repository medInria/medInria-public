/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore/QObject>
#include <medPacsExport.h>
#include <medAbstractPacsBaseScu.h>

class MEDPACS_EXPORT medAbstractPacsStoreScu : public medAbstractPacsBaseScu
{
    Q_OBJECT

public:
    medAbstractPacsStoreScu() = default;
    ~medAbstractPacsStoreScu() = default;

    virtual int  sendStoreRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, const char* ourTitle, const char* ourIP, unsigned int ourPort);
};
