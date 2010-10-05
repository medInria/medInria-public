#include "medAbstractPacsNode.h"

#include <dtkCore/dtkGlobal.h>

#include <QtCore>

medAbstractPacsNode::medAbstractPacsNode( void )
{

}

medAbstractPacsNode::~medAbstractPacsNode( void )
{

}

void medAbstractPacsNode::addConnData( ConnData cdata )
{
    DTK_UNUSED(cdata);

    DTK_DEFAULT_IMPLEMENTATION;
}

medAbstractPacsNode::ConnData medAbstractPacsNode::getConnData()
{
    DTK_DEFAULT_IMPLEMENTATION;

    ConnData data;
    return data;
}

QVector<medAbstractPacsResultDataset*>& medAbstractPacsNode::getResultDatasetContainer()
{
    DTK_DEFAULT_IMPLEMENTATION;

    return QVector<medAbstractPacsResultDataset*>();
}
