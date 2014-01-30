/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
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

    typedef QHash <QString, QStringList>        identifierHandledTypeHash;

    public:

    viewIdCreatorHash         viewCreators;
    navigatorIdCreatorHash    navigatorCreators;
    interactorIdCreatorHash   intercatorCreators;

    identifierHandledTypeHash identifierHash;
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

medAbstractView* medViewFactory::createView(QString identifier, QObject *parent)
{
    medAbstractView* view = NULL;
    viewCreator c = NULL;
    c = d->viewCreators.value(identifier);
    if(c)
        view = (c)(parent);

    return view;
}

medAbstractViewNavigator* medViewFactory::createNavigator(QString identifier, medAbstractView *parent)
{

    medAbstractViewNavigator* navigator = NULL;
    navigatorCreator c = NULL;
    c = d->navigatorCreators.value(identifier);
    if(c)
        navigator = (c)(parent);

    return navigator;
}

medAbstractViewInteractor*  medViewFactory::createInteractor(QString identifier, medAbstractView *parent)
{
    medAbstractViewInteractor* interactor = NULL;
    interactorCreator c = NULL;
    c = d->intercatorCreators.value(identifier);
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
    d->identifierHash.insert(identifier, typeHandled);
    return true;
}

bool medViewFactory::registerNavigator(QString identifier, QStringList typeHandled, navigatorCreator creator)
{
    if(d->navigatorCreators.contains(identifier))
        return false;

    d->navigatorCreators.insert(identifier, creator);
    d->identifierHash.insert(identifier, typeHandled);
    return true;
}

bool medViewFactory::registerInteractor(QString identifier, QStringList typeHandled, interactorCreator creator)
{
    if(d->intercatorCreators.contains(identifier))
        return false;

    d->intercatorCreators.insert(identifier, creator);
    d->identifierHash.insert(identifier, typeHandled);
    return true;
}

//--------------------------------------------------------------------------
//  get handler

QStringList medViewFactory::navigatorsAbleToHandle(const QString viewType) const
{
    QHashIterator<QString, QStringList> it(d->identifierHash);

    QList<QString> navigators;

    for(it.toFront(); it.hasNext(); it.next())
    {
        if (it.value().contains(viewType))
                navigators << it.key();
    }

    return navigators;
}

QStringList medViewFactory::interactorsAbleToHandle(const QString viewType, const QString dataType) const
{
    QHashIterator<QString, QStringList> it(d->identifierHash);

    QList<QString> interactors;

    for(it.toFront(); it.hasNext(); it.next())
    {
        if (it.value().contains(viewType) && it.value().contains(dataType))
            interactors << it.key();
    }

    return interactors;
}

QStringList medViewFactory::viewsAbleToHandle(const QString dataType) const
{
    QHashIterator<QString, QStringList> it(d->identifierHash);

    QList<QString> views;

    for(it.toFront(); it.hasNext(); it.next())
    {
        if (it.value().contains(dataType))
            views << it.key();
    }
    return views;
}


//--------------------------------------------------------------------------
//  construc / destruct factory


medViewFactory *medViewFactory::s_instance = NULL;



