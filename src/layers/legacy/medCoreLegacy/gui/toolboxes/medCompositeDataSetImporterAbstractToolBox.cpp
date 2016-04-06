/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCompositeDataSetImporterSelectorToolBox.h>
#include <medCompositeDataSetImporterAbstractToolBox.h>
#include <dtkCoreSupport/dtkAbstractObject.h>

class medCompositeDataSetImporterAbstractToolBoxPrivate
{
public:
    medCompositeDataSetImporterSelectorToolBox *parent;
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

void medCompositeDataSetImporterAbstractToolBox::setCompositeDataSetImporterToolBox(medCompositeDataSetImporterSelectorToolBox *toolbox)
{
    d->parent = toolbox;
}

medCompositeDataSetImporterSelectorToolBox *medCompositeDataSetImporterAbstractToolBox::parent(void)
{
    return d->parent;
}

medAbstractData *medCompositeDataSetImporterAbstractToolBox::output() const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return NULL;
}
