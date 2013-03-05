/* medSegmentationAbstractToolBox.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 15:42:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 15:58:03 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 17
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medSegmentationSelectorToolBox.h"
#include "medSegmentationAbstractToolBox.h"

class medSegmentationAbstractToolBoxPrivate
{
public:
    medSegmentationSelectorToolBox * segmentationToolBox;
};

medSegmentationAbstractToolBox::medSegmentationAbstractToolBox(QWidget *parent) : medToolBox(parent), d(new medSegmentationAbstractToolBoxPrivate)
{
    d->segmentationToolBox = qobject_cast<medSegmentationSelectorToolBox*>(parent);
}

medSegmentationAbstractToolBox::~medSegmentationAbstractToolBox()
{
    delete d;
    d = NULL;
}

medSegmentationSelectorToolBox *medSegmentationAbstractToolBox::segmentationToolBox()
{
    return d->segmentationToolBox;
}
