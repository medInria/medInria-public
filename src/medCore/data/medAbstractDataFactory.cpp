/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDataFactory.h>

medAbstractDataFactory::medAbstractDataFactory()
{
}

medAbstractDataFactory *medAbstractDataFactory::instance(void)
{
    if(!s_instance)
    s_instance = new medAbstractDataFactory;

    return s_instance;
}

medAbstractData *medAbstractDataFactory::create(const QString& type)
{
    dtkAbstractData* data = dtkAbstractDataFactory::create(type);
    return dynamic_cast<medAbstractData *>(data);
}

medAbstractDataFactory *medAbstractDataFactory::s_instance = NULL;
