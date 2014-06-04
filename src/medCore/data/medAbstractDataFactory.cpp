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

medAbstractDataFactory *medAbstractDataFactory::s_instance = NULL;
