#include "medAbstractPacsNode.h"

#include <dtkCore/dtkGlobal.h>

#include <QtCore>

medAbstractPacsNode::medAbstractPacsNode()
{

}

medAbstractPacsNode::~medAbstractPacsNode()
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

