/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medLayeredViewFactory.h>


medLayeredViewFactory *medLayeredViewFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medLayeredViewFactory;

    return s_instance;
}

class medLayeredViewFactoryPrivate
{
    typedef QHash <QString, medLayeredViewFactory::viewCreator>        viewIdCreatorHash;
    typedef QHash <QString, medLayeredViewFactory::navigatorCreator>   navigatorIdCreatorHash;
    typedef QHash <QString, medLayeredViewFactory::interactorCreator>  interactorIdCreatorHash;

    typedef QHash <QString, QStringList>        identifierHandledTypeHash;

    public:

    viewIdCreatorHash         viewCreators;
    navigatorIdCreatorHash    navigatorCreators;
    interactorIdCreatorHash   intercatorCreators;

    identifierHandledTypeHash identifierHash;
};

medLayeredViewFactory::medLayeredViewFactory()
{
    d = new medLayeredViewFactoryPrivate;
}
medLayeredViewFactory::~medLayeredViewFactory()
{
    delete d;
}

//--------------------------------------------------------------------------
//  create

medAbstractLayeredView* medLayeredViewFactory::createView(QString identifier, QObject *parent)
{
    medAbstractLayeredView* view = NULL;
    viewCreator c = NULL;
    c = d->viewCreators.value(identifier);
    if(c)
        view = (c)(parent);

    return view;
}

medAbstractLayeredViewNavigator* medLayeredViewFactory::createNavigator(QString identifier, medAbstractLayeredView *parent)
{

    medAbstractLayeredViewNavigator* navigator = NULL;
    navigatorCreator c = NULL;
    c = d->navigatorCreators.value(identifier);
    if(c)
        navigator = (c)(parent);

    return navigator;
}

medAbstractLayeredViewInteractor*  medLayeredViewFactory::createInteractor(QString identifier, medAbstractLayeredView *parent)
{
    medAbstractLayeredViewInteractor* interactor = NULL;
    interactorCreator c = NULL;
    c = d->intercatorCreators.value(identifier);
    if(c)
        interactor = (c)(parent);

    return interactor;
}


//--------------------------------------------------------------------------
//  register


bool medLayeredViewFactory::registerView(QString identifier, QStringList typeHandled, viewCreator creator)
{
    if(d->viewCreators.contains(identifier))
        return false;

    d->viewCreators.insert(identifier, creator);
    d->identifierHash.insert(identifier, typeHandled);
    return true;
}

bool medLayeredViewFactory::registerNavigator(QString identifier, QStringList typeHandled, navigatorCreator creator)
{
    if(d->navigatorCreators.contains(identifier))
        return false;

    d->navigatorCreators.insert(identifier, creator);
    d->identifierHash.insert(identifier, typeHandled);
    return true;
}

bool medLayeredViewFactory::registerInteractor(QString identifier, QStringList typeHandled, interactorCreator creator)
{
    if(d->intercatorCreators.contains(identifier))
        return false;

    d->intercatorCreators.insert(identifier, creator);
    d->identifierHash.insert(identifier, typeHandled);
    return true;
}

//--------------------------------------------------------------------------
//  get handler

QStringList medLayeredViewFactory::navigatorsAbleToHandle(const QString viewType) const
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

QStringList medLayeredViewFactory::interactorsAbleToHandle(const QString viewType, const QString dataType) const
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

QStringList medLayeredViewFactory::viewsAbleToHandle(const QString dataType) const
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


medLayeredViewFactory *medLayeredViewFactory::s_instance = NULL;



