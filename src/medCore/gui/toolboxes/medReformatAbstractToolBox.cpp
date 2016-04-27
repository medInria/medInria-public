/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medReformatSelectorToolBox.h>
#include <medReformatAbstractToolBox.h>

class medReformatAbstractToolBoxPrivate
{
public:
    medReformatSelectorToolBox * selectorToolBox;
};

//! Parent should be a medReformatSelectorToolBox
medReformatAbstractToolBox::medReformatAbstractToolBox(QWidget *parent) : medToolBox(parent), d(new medReformatAbstractToolBoxPrivate)
{
    d->selectorToolBox = qobject_cast<medReformatSelectorToolBox*>(parent);
}

medReformatAbstractToolBox::~medReformatAbstractToolBox(void)
{
    delete d;
    d = NULL;
}

//! Get the ReformatToolbox (usually one instance)
medReformatSelectorToolBox *medReformatAbstractToolBox::selectorToolBox(void)
{
    return d->selectorToolBox;
}
