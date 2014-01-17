/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDiffusionSelectorToolBox.h>

#include "medDiffusionAbstractToolBox.h"
#include "medDiffusionAbstractToolBox_p.h"
#include <medDataManager.h>

#include <dtkCore/dtkAbstractObject.h>

medDiffusionAbstractToolBox::medDiffusionAbstractToolBox(QWidget *parent) : medToolBox(parent), d(new medDiffusionAbstractToolBoxPrivate)
{
    d->parent  = NULL;
    connect(this, SIGNAL(success()), this, SLOT(processSuccess()));
}

medDiffusionAbstractToolBox::~medDiffusionAbstractToolBox(void)
{
    delete d;

    d = NULL;
}

void medDiffusionAbstractToolBox::setDiffusionToolBox(medDiffusionSelectorToolBox *toolBox)
{
    d->parent = toolBox;
}

medDiffusionSelectorToolBox *medDiffusionAbstractToolBox::parentToolBox(void)
{
    return d->parent;
}


dtkAbstractData *medDiffusionAbstractToolBox::output() const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return NULL;
}


void medDiffusionAbstractToolBox::processSuccess()
{
    medDataManager::instance()->importNonPersistent ( this->output());
    emit newOutput(this->output());
}
