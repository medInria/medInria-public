/* medCompositeDataSetImporterAbstractToolBox.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 15:42:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 15:58:03 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include <medCompositeDataSetImporterSelectorToolBox.h>
#include <medCompositeDataSetImporterAbstractToolBox.h>
#include <dtkCore/dtkAbstractObject.h>

class medCompositeDataSetImporterAbstractToolBoxPrivate
{
public:
    medCompositeDataSetImporterSelectorToolBox *parent;
};


medCompositeDataSetImporterAbstractToolBox::medCompositeDataSetImporterAbstractToolBox(QWidget *parent): medToolBox(parent), d(new medCompositeDataSetImporterAbstractToolBoxPrivate)
{
    d->parent  = NULL;
}

medCompositeDataSetImporterAbstractToolBox::~medCompositeDataSetImporterAbstractToolBox()
{
    delete d;

    d = NULL;
}

void medCompositeDataSetImporterAbstractToolBox::setCompositeDataSetImporterToolBox(medCompositeDataSetImporterSelectorToolBox *toolbox)
{
    d->parent = toolbox;
}

medCompositeDataSetImporterSelectorToolBox *medCompositeDataSetImporterAbstractToolBox::parent()
{
    return d->parent;
}

dtkAbstractData *medCompositeDataSetImporterAbstractToolBox::output() const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return NULL;
}
