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
    medMeshingSelectorToolBox * meshingToolBox;
};

//! Parent should be a medMeshingSelectorToolBox
medMeshingAbstractToolBox::medMeshingAbstractToolBox(QWidget *parent) : medToolBox(parent), d(new medMeshingAbstractToolBoxPrivate)
{
    d->meshingToolBox = qobject_cast<medMeshingSelectorToolBox*>(parent);
}

medMeshingAbstractToolBox::~medMeshingAbstractToolBox(void)
{
    delete d;
    d = NULL;
}

//! Get the meshingToolbox (usually one instance)
medMeshingSelectorToolBox *medMeshingAbstractToolBox::meshingToolBox(void)
{
    return d->meshingToolBox;
}
