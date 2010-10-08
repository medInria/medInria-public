#include "medAbstractPacsNode.h"

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
