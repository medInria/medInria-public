/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medExtraNavigatorFactory.h>



medExtraNavigatorFactory *medExtraNavigatorFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medExtraNavigatorFactory;

    return s_instance;
}

class medExtraNavigatorFactoryPrivate
{
    typedef QHash <QString, medExtraNavigatorFactory::navigatorCreator>  navigatorIdCreatorHash;
    typedef QHash <QString, QStringList>        identifierHandledTypeHash;

    public:

    navigatorIdCreatorHash   navigatorCreators;
    identifierHandledTypeHash identifierHash;
};

medExtraNavigatorFactory::medExtraNavigatorFactory()
{
    d = new medExtraNavigatorFactoryPrivate;
}
medExtraNavigatorFactory::~medExtraNavigatorFactory()
{
    delete d;
}

//--------------------------------------------------------------------------
//  create

medAbstractExtraNavigator*  medExtraNavigatorFactory::createNavigator(QString identifier, medAbstractView *parent)
{
    medAbstractExtraNavigator* navigator = NULL;
    navigatorCreator c = NULL;
    c = d->navigatorCreators.value(identifier);
    if(c)
        navigator = (c)(parent);

    return navigator;
}


//--------------------------------------------------------------------------
//  register


bool medExtraNavigatorFactory::registerNavigator(QString identifier, QStringList typeHandled, navigatorCreator creator)
{

    if(d->navigatorCreators.contains(identifier))
        return false;

    d->navigatorCreators.insert(identifier, creator);
    d->identifierHash.insert(identifier, typeHandled);
    return true;
}

//--------------------------------------------------------------------------
//  get handler


QStringList medExtraNavigatorFactory::navigatorsAbleToHandle(const QString viewType) const
{
    if(d->identifierHash.isEmpty())
        return QStringList();

    QHashIterator<QString, QStringList> it(d->identifierHash);

    QList<QString> navigators;

    while(it.hasNext())
    {
        it.next();
        if (it.value().contains(viewType))
            navigators << it.key();
    }

    return navigators;
}

medExtraNavigatorFactory *medExtraNavigatorFactory::s_instance = NULL;



