/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medFilteringSelectorToolBox.h>
#include <medFilteringAbstractToolBox.h>

class medFilteringAbstractToolBoxPrivate
{
public:
    medFilteringSelectorToolBox *parent;
};

medFilteringAbstractToolBox::medFilteringAbstractToolBox(QWidget *parent) : medToolBox(parent), d(new medFilteringAbstractToolBoxPrivate)
{
    d->parent = NULL;
}

medFilteringAbstractToolBox::~medFilteringAbstractToolBox(void)
{
    delete d;

    d = NULL;
}

void medFilteringAbstractToolBox::setFilteringToolBox(medFilteringSelectorToolBox *toolbox)
{
    d->parent = toolbox;
}

medFilteringSelectorToolBox *medFilteringAbstractToolBox::parentToolBox(void)
{
    return d->parent;
}

