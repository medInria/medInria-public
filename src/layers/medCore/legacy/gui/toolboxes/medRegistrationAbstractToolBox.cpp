/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medRegistrationSelectorToolBox.h>
#include <medRegistrationAbstractToolBox.h>

class medRegistrationAbstractToolBoxPrivate
{
public:
    medRegistrationSelectorToolBox *parent;
};

medRegistrationAbstractToolBox::medRegistrationAbstractToolBox(QWidget *parent)
    : medToolBox(parent), d(new medRegistrationAbstractToolBoxPrivate)
{
    d->parent = nullptr;
}

medRegistrationAbstractToolBox::~medRegistrationAbstractToolBox(void)
{
    delete d;

    d = nullptr;
}

void medRegistrationAbstractToolBox::setRegistrationToolBox(medRegistrationSelectorToolBox *toolbox)
{
    d->parent = toolbox;
}

medRegistrationSelectorToolBox *medRegistrationAbstractToolBox::parentToolBox(void)
{
    return d->parent;
}
