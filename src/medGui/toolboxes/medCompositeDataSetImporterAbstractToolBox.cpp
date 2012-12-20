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

#include <medToolBoxCompositeDataSetImporter.h>
#include <medCompositeDataSetImporterAbstractToolBox.h>
#include <dtkCore/dtkAbstractObject.h>

class medCompositeDataSetImporterAbstractToolBoxPrivate
{
public:
    medToolBoxCompositeDataSetImporter *parent;
};


medCompositeDataSetImporterAbstractToolBox::medCompositeDataSetImporterAbstractToolBox(QWidget *parent): medToolBox(parent), d(new medCompositeDataSetImporterAbstractToolBoxPrivate)
{
    d->parent  = NULL;
}

medCompositeDataSetImporterAbstractToolBox::~medCompositeDataSetImporterAbstractToolBox(void)
{
    delete d;

    d = NULL;
}

void medCompositeDataSetImporterAbstractToolBox::setCompositeDataSetImporterToolBox(medToolBoxCompositeDataSetImporter *toolbox)
{
    d->parent = toolbox;
}

medToolBoxCompositeDataSetImporter *medCompositeDataSetImporterAbstractToolBox::parent(void)
{
    return d->parent;
}

dtkAbstractData *medCompositeDataSetImporterAbstractToolBox::output (void) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return NULL;
}
