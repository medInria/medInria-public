/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractViewCollection.h"

class medAbstractViewCollectionPrivate
{
public:
};

medAbstractViewCollection::medAbstractViewCollection(medAbstractViewCollection *parent)
  : medAbstractView (parent), d(new medAbstractViewCollectionPrivate)
{
}

medAbstractViewCollection::medAbstractViewCollection(const medAbstractViewCollection &view)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

medAbstractViewCollection::~medAbstractViewCollection(void)
{
    delete d;
    d = NULL;
}

QList <medAbstractView *> medAbstractViewCollection::views() const
{
    return QList <medAbstractView *> ();
}