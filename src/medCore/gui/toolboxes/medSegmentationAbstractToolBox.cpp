/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSegmentationSelectorToolBox.h>
#include <medSegmentationAbstractToolBox.h>

class medSegmentationAbstractToolBoxPrivate
{
public:
    medSegmentationSelectorToolBox * segmentationToolBox;
    medAbstractWorkspace * workspace;
};

//! Parent should be a medSegmentationSelectorToolBox
medSegmentationAbstractToolBox::medSegmentationAbstractToolBox(QWidget *parent) : medToolBox(parent), d(new medSegmentationAbstractToolBoxPrivate)
{
    d->segmentationToolBox = qobject_cast<medSegmentationSelectorToolBox*>(parent);
}

medSegmentationAbstractToolBox::~medSegmentationAbstractToolBox(void)
{
    delete d;
    d = NULL;
}

//! Get the segmentationToolbox (usually one instance)
medSegmentationSelectorToolBox *medSegmentationAbstractToolBox::segmentationToolBox(void)
{
    return d->segmentationToolBox;
}

void medSegmentationAbstractToolBox::setWorkspace(medAbstractWorkspace *workspace)
{
    d->workspace = workspace;
}

medAbstractWorkspace * medSegmentationAbstractToolBox::getWorkspace()
{
    return d->workspace;
}
