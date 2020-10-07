/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractPacsStoreScp.h>

#include <dtkCoreSupport/dtkGlobal.h>

#include <QtCore>

int medAbstractPacsStoreScp::start( const char* ourTitle, const char* ourIP, unsigned int ourPort )
{
    Q_UNUSED(ourTitle);
    Q_UNUSED(ourIP);
    Q_UNUSED(ourPort);

    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

void medAbstractPacsStoreScp::stop()
{
    DTK_DEFAULT_IMPLEMENTATION;
}

bool medAbstractPacsStoreScp::setStorageDirectory( const char* directory )
{
    Q_UNUSED(directory);
    return false;
}
