/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSelectorToolBox.h>
#include <medAbstractToolBox.h>

class medAbstractToolBoxPrivate
{
public:
    medSelectorToolBox * selectorToolBox;
};

//! Parent should be a medSelectorToolBox
medAbstractToolBox::medAbstractToolBox(QWidget *parent) : medToolBox(parent), d(new medAbstractToolBoxPrivate)
{
    d->selectorToolBox = qobject_cast<medSelectorToolBox*>(parent);
}

medAbstractToolBox::~medAbstractToolBox(void)
{
    delete d;
    d = NULL;
}

void medAbstractToolBox::setSelectorToolBox(medSelectorToolBox *toolbox)
{
    d->selectorToolBox = toolbox;
}

//! Get the selectorToolBox (usually one instance)
medSelectorToolBox *medAbstractToolBox::selectorToolBox(void)
{
    return d->selectorToolBox;
}
