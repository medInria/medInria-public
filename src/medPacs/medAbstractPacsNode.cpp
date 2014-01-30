/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractPacsNode.h>

#include <dtkCore/dtkGlobal.h>

#include <QtCore>

medAbstractPacsNode::medAbstractPacsNode( void )
{

}

medAbstractPacsNode::~medAbstractPacsNode( void )
{

}

QVector<medAbstractPacsResultDataset*> medAbstractPacsNode::getResultDatasetContainer()
{
    DTK_DEFAULT_IMPLEMENTATION;

    return QVector<medAbstractPacsResultDataset*>();
}

void medAbstractPacsNode::setTitle( QString title )
{
    DTK_UNUSED(title);
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractPacsNode::setIp( QString ip )
{
    DTK_UNUSED(ip);
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractPacsNode::setPort( unsigned int port )
{
    DTK_UNUSED(port);
    DTK_DEFAULT_IMPLEMENTATION;
}

