/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMeshingSelectorToolBox.h>
#include <medMeshingAbstractToolBox.h>

class medMeshingAbstractToolBoxPrivate
{
public:
    medMeshingSelectorToolBox * selectorToolBox;
};

//! Parent should be a medMeshingSelectorToolBox
medMeshingAbstractToolBox::medMeshingAbstractToolBox(QWidget *parent) : medToolBox(parent), d(new medMeshingAbstractToolBoxPrivate)
{
    d->selectorToolBox = qobject_cast<medMeshingSelectorToolBox*>(parent);
}

medMeshingAbstractToolBox::~medMeshingAbstractToolBox(void)
{
    delete d;
    d = NULL;
}

//! Get the selectorToolBox (usually one instance)
medMeshingSelectorToolBox *medMeshingAbstractToolBox::selectorToolBox(void)
{
    return d->selectorToolBox;
}
