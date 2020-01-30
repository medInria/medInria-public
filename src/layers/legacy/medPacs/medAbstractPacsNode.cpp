/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractPacsNode.h>

#include <dtkCoreSupport/dtkGlobal.h>

#include <QtCore>

QVector<medAbstractPacsResultDataset*> medAbstractPacsNode::getResultDatasetContainer()
{
    DTK_DEFAULT_IMPLEMENTATION;

    return QVector<medAbstractPacsResultDataset*>();
}

void medAbstractPacsNode::setTitle( QString title )
{
    Q_UNUSED(title);
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractPacsNode::setIp( QString ip )
{
    Q_UNUSED(ip);
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractPacsNode::setPort( unsigned int port )
{
    Q_UNUSED(port);
    DTK_DEFAULT_IMPLEMENTATION;
}
