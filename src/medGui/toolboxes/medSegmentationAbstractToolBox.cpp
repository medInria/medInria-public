/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
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
};

medSegmentationAbstractToolBox::medSegmentationAbstractToolBox(QWidget *parent) : medToolBox(parent), d(new medSegmentationAbstractToolBoxPrivate)
{
    d->segmentationToolBox = qobject_cast<medSegmentationSelectorToolBox*>(parent);
}

medSegmentationAbstractToolBox::~medSegmentationAbstractToolBox(void)
{
    delete d;
    d = NULL;
}

medSegmentationSelectorToolBox *medSegmentationAbstractToolBox::segmentationToolBox(void)
{
    return d->segmentationToolBox;
}
