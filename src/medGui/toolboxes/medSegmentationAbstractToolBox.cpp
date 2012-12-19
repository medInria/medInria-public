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

#include "medToolBoxSegmentation.h"
#include "medSegmentationAbstractToolBox.h"

class medSegmentationAbstractToolBoxPrivate
{
public:
    medToolBoxSegmentation * segmentationToolBox;
};

medSegmentationAbstractToolBox::medSegmentationAbstractToolBox(QWidget *parent) : medToolBox(parent), d(new medSegmentationAbstractToolBoxPrivate)
{
    d->segmentationToolBox = qobject_cast<medToolBoxSegmentation*>(parent);
}

medSegmentationAbstractToolBox::~medSegmentationAbstractToolBox(void)
{
    delete d;
    d = NULL;
}

medToolBoxSegmentation *medSegmentationAbstractToolBox::segmentationToolBox(void)
{
    return d->segmentationToolBox;
}
