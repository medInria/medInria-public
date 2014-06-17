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
    return dynamic_cast<medAbstractData *>(dtkAbstractDataFactory::create(type));
}

bool medAbstractDataFactory::registerDataType(const QString& type, medAbstractDataCreator func)
{
    dtkAbstractDataCreator dtkfunc = reinterpret_cast<dtkAbstractDataCreator>(reinterpret_cast<dtkAbstractData*>(func)) ;
    return dtkAbstractDataFactory::registerDataType(type, dtkfunc);
}

bool medAbstractDataFactory::registerDataType(const QString& type, medAbstractDataCreator func, const QString& nameInterface)
{
    dtkAbstractDataCreator dtkfunc = reinterpret_cast<dtkAbstractDataCreator>(reinterpret_cast<dtkAbstractData*>(func)) ;
    return dtkAbstractDataFactory::registerDataType(type, dtkfunc, nameInterface);
}

bool medAbstractDataFactory::registerDataType(const QString& type, dtkAbstractDataCreator func)
{
    return dtkAbstractDataFactory::registerDataType(type, func);
}

bool medAbstractDataFactory::registerDataType(const QString& type, dtkAbstractDataCreator func, const QString& nameInterface)
{
    return dtkAbstractDataFactory::registerDataType(type, func, nameInterface);
}

medAbstractDataFactory *medAbstractDataFactory::s_instance = NULL;
