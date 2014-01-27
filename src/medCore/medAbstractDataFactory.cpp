#include "medAbstractDataFactory.h"

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
    return dynamic_cast<medAbstractData *>(dtkAbstractDataFactory::instance()->create(type));
}

bool medAbstractDataFactory::registerDataType(const QString& type, medAbstractDataCreator func)
{
    dtkAbstractDataCreator dtkfunc = reinterpret_cast<dtkAbstractDataCreator>(reinterpret_cast<dtkAbstractData*>(func)) ;
    return dtkAbstractDataFactory::instance()->registerDataType(type, dtkfunc);
}

bool medAbstractDataFactory::registerDataType(const QString& type, medAbstractDataCreator func, const QString& nameInterface)
{
    dtkAbstractDataCreator dtkfunc = reinterpret_cast<dtkAbstractDataCreator>(reinterpret_cast<dtkAbstractData*>(func)) ;
    return dtkAbstractDataFactory::instance()->registerDataType(type, dtkfunc, nameInterface);
}

bool medAbstractDataFactory::registerDataType(const QString& type, dtkAbstractDataCreator func)
{
    return dtkAbstractDataFactory::instance()->registerDataType(type, func);
}

bool medAbstractDataFactory::registerDataType(const QString& type, dtkAbstractDataCreator func, const QString& nameInterface)
{
    return dtkAbstractDataFactory::instance()->registerDataType(type, func, nameInterface);
}

medAbstractDataFactory *medAbstractDataFactory::s_instance = NULL;
