/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractPacsStoreScp.h>

#include <dtkCore/dtkGlobal.h>

#include <QtCore>

medAbstractPacsStoreScp::medAbstractPacsStoreScp( void )
{

}

medAbstractPacsStoreScp::~medAbstractPacsStoreScp( void )
{

}

int medAbstractPacsStoreScp::start( const char* ourTitle, const char* ourIP, unsigned int ourPort )
{
    DTK_UNUSED(ourTitle);
    DTK_UNUSED(ourIP);
    DTK_UNUSED(ourPort);

    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

void medAbstractPacsStoreScp::stop()
{
    DTK_DEFAULT_IMPLEMENTATION;
}

bool medAbstractPacsStoreScp::setStorageDirectory( const char* directory )
{
    DTK_UNUSED(directory);
    return false;

}
