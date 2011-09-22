/* medToolBoxCompositeDataSetImporterCustom.cpp ---
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
#include <medToolBoxCompositeDataSetImporterCustom.h>
#include <dtkCore/dtkAbstractObject.h>

class medToolBoxCompositeDataSetImporterCustomPrivate
{
public:
    medToolBoxCompositeDataSetImporter *parent;
};


medToolBoxCompositeDataSetImporterCustom::medToolBoxCompositeDataSetImporterCustom(QWidget *parent) : medToolBox(parent), d(new medToolBoxCompositeDataSetImporterCustomPrivate)
{
    d->parent  = NULL;
}

medToolBoxCompositeDataSetImporterCustom::~medToolBoxCompositeDataSetImporterCustom(void)
{
    delete d;

    d = NULL;
}

void medToolBoxCompositeDataSetImporterCustom::setCompositeDataSetImporterToolBox(medToolBoxCompositeDataSetImporter *toolbox)
{
    d->parent = toolbox;
}

medToolBoxCompositeDataSetImporter *medToolBoxCompositeDataSetImporterCustom::parent(void)
{
    return d->parent;
}

dtkAbstractData *medToolBoxCompositeDataSetImporterCustom::output (void) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return NULL;
}
