/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewFactory.h>


medViewFactory *medViewFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medViewFactory;

    return s_instance;
}


class medViewFactoryPrivate
{
    typedef QHash <QString, viewCreator>        viewIdCreatorHash;
    typedef QHash <QString, navigatorCreator>   navigatorIdCreatorHash;
    typedef QHash <QString, interactorCreator>  interactorIdCreatorHash;
    typedef QHash <QString, addNavigatorCreator>   addNavigatorIdCreatorHash;
    typedef QHash <QString, addInteractorCreator>  addInteractorIdCreatorHash;

    typedef QHash <QString, QStringList>        identifierHandledTypeHash;

    public:

    viewIdCreatorHash         viewCreators;
    navigatorIdCreatorHash    navigatorCreators;
    interactorIdCreatorHash   intercatorCreators;
    addNavigatorIdCreatorHash    addNavigatorCreators;
    addInteractorIdCreatorHash   addInteractorCreators;

    identifierHandledTypeHash viewIdentifierHash;
    identifierHandledTypeHash navigatorIdentifierHash;
    identifierHandledTypeHash interactorIdentifierHash;
    identifierHandledTypeHash addNavigatorIdentifierHash;
    identifierHandledTypeHash addInteractorIdentifierHash;
};

medViewFactory::medViewFactory()
{
    d = new medViewFactoryPrivate;
}
medViewFactory::~medViewFactory()
{
    delete d;
}

//--------------------------------------------------------------------------
//  create

medAbstractNavigator* medViewFactory::createAdditionalNavigator(QString identifier, medAbstractView *parent)
{

    medAbstractNavigator* navigator = NULL;
    addNavigatorCreator c = NULL;
    c = d->addNavigatorCreators.value(identifier);
    if(c)
        navigator = (c)(parent);

    return navigator;
}

medAbstractInteractor*  medViewFactory::createAdditionalInteractor(QString identifier, medAbstractView *parent)
{
    medAbstractInteractor* interactor = NULL;
    addInteractorCreator c = NULL;
    c = d->addInteractorCreators.value(identifier);
    if(c)
        interactor = (c)(parent);

    return interactor;
}

//--------------------------------------------------------------------------
//  register


bool medViewFactory::registerView(QString identifier, QStringList typeHandled, viewCreator creator)
{
    if(d->viewCreators.contains(identifier))
        return false;

    d->viewCreators.insert(identifier, creator);
    d->viewIdentifierHash.insert(identifier, typeHandled);
    return true;
}

bool medViewFactory::registerNavigator(QString identifier, QStringList typeHandled, navigatorCreator creator)
{
    if(d->navigatorCreators.contains(identifier))
        return false;

    d->navigatorCreators.insert(identifier, creator);
    d->navigatorIdentifierHash.insert(identifier, typeHandled);
    return true;
}

bool medViewFactory::registerInteractor(QString identifier, QStringList typeHandled, interactorCreator creator)
{
    if(d->intercatorCreators.contains(identifier))
        return false;

    d->intercatorCreators.insert(identifier, creator);
    d->interactorIdentifierHash.insert(identifier, typeHandled);
    return true;
}

bool medViewFactory::registerAdditionalNavigator(QString identifier, QStringList typeHandled, addNavigatorCreator creator)
{
    if(d->addNavigatorCreators.contains(identifier))
        return false;

    d->addNavigatorCreators.insert(identifier, creator);
    d->addNavigatorIdentifierHash.insert(identifier, typeHandled);
    return true;
}

bool medViewFactory::registerAdditionalInteractor(QString identifier, QStringList typeHandled, addInteractorCreator creator)
{
    if(d->addInteractorCreators.contains(identifier))
        return false;

    d->addInteractorCreators.insert(identifier, creator);
    d->addInteractorIdentifierHash.insert(identifier, typeHandled);
    return true;
}

//--------------------------------------------------------------------------
//  get creators

viewCreator medViewFactory::getViewCreator(QString identifier)
{
    return d->viewCreators.value(identifier);
}

interactorCreator medViewFactory::getInteractorCreator(QString identifier)
{
    return d->intercatorCreators.value(identifier);
}

navigatorCreator medViewFactory::getNavigatorCreator(QString identifier)
{
    return d->navigatorCreators.value(identifier);
}

//--------------------------------------------------------------------------
//  get handler

QStringList medViewFactory::viewsAbleToHandle(const QString dataType) const
{
    if(d->viewIdentifierHash.isEmpty())
        return QStringList();

    QHashIterator<QString, QStringList> it(d->viewIdentifierHash);

    QList<QString> views;

    while(it.hasNext())
    {
        it.next();
        if (it.value().contains(dataType))
            views << it.key();
    }
    return views;
}

QStringList medViewFactory::navigatorsAbleToHandle(const QString viewType) const
{
    if(d->navigatorIdentifierHash.isEmpty())
        return QStringList();

    QHashIterator<QString, QStringList> it(d->navigatorIdentifierHash);

    QList<QString> navigators;

    while(it.hasNext())
    {
        it.next();
        if (it.value().contains(viewType))
                navigators << it.key();
    }

    return navigators;
}

QStringList medViewFactory::interactorsAbleToHandle(const QString viewType, const QString dataType) const
{
    if(d->interactorIdentifierHash.isEmpty())
        return QStringList();

    QHashIterator<QString, QStringList> it(d->interactorIdentifierHash);

    QList<QString> interactors;

    while(it.hasNext())
    {
        it.next();
        if (it.value().contains(viewType) && it.value().contains(dataType))
            interactors << it.key();
    }

    return interactors;
}

QStringList medViewFactory::additionalNavigatorsAbleToHandle(const QString viewType) const
{
    if(d->addNavigatorIdentifierHash.isEmpty())
        return QStringList();

    QHashIterator<QString, QStringList> it(d->addNavigatorIdentifierHash);

    QList<QString> navigators;

    while(it.hasNext())
    {
        it.next();
        if (it.value().contains(viewType))
                navigators << it.key();
    }

    return navigators;
}

QStringList medViewFactory::additionalInteractorsAbleToHandle(const QString viewType, const QString dataType) const
{
    if(d->addInteractorIdentifierHash.isEmpty())
        return QStringList();

    QHashIterator<QString, QStringList> it(d->addInteractorIdentifierHash);

    QList<QString> interactors;

    while(it.hasNext())
    {
        it.next();
        if (it.value().contains(viewType) && it.value().contains(dataType))
            interactors << it.key();
    }

    return interactors;
}


//--------------------------------------------------------------------------
//  construc / destruct factory


medViewFactory *medViewFactory::s_instance = NULL;



