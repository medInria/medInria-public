/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medImageViewFactory.h>



medImageViewFactory *medImageViewFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medImageViewFactory;

    return s_instance;
}





class medImageViewFactoryPrivate
{
    typedef QHash <QString, medImageViewFactory::viewCreator>        viewIdCreatorHash;
    typedef QHash <QString, medImageViewFactory::navigatorCreator>   navigatorIdCreatorHash;
    typedef QHash <QString, medImageViewFactory::interactorCreator>  interactorIdCreatorHash;

    typedef QHash <QString, QStringList>        identifierHandledTypeHash;

    public:

    viewIdCreatorHash         viewCreators;
    navigatorIdCreatorHash    navigatorCreators;
    interactorIdCreatorHash   intercatorCreators;

    identifierHandledTypeHash viewIdentifierHash;
    identifierHandledTypeHash navigatorIdentifierHash;
    identifierHandledTypeHash interactorIdentifierHash;
};

medImageViewFactory::medImageViewFactory()
{
    d = new medImageViewFactoryPrivate;
}
medImageViewFactory::~medImageViewFactory()
{
    delete d;
}

//--------------------------------------------------------------------------
//  create

medAbstractImageView* medImageViewFactory::createView(QString identifier, QObject *parent)
{
    medAbstractImageView* view = NULL;
    viewCreator c = NULL;
    c = d->viewCreators.value(identifier);
    if(c)
        view = (c)(parent);

    return view;
}

medAbstractImageViewNavigator* medImageViewFactory::createNavigator(QString identifier, medAbstractImageView *parent)
{

    medAbstractImageViewNavigator* navigator = NULL;
    navigatorCreator c = NULL;
    c = d->navigatorCreators.value(identifier);
    if(c)
        navigator = (c)(parent);

    return navigator;
}

medAbstractImageViewInteractor*  medImageViewFactory::createInteractor(QString identifier, medAbstractImageView *parent)
{
    medAbstractImageViewInteractor* interactor = NULL;
    interactorCreator c = NULL;
    c = d->intercatorCreators.value(identifier);
    if(c)
        interactor = (c)(parent);

    return interactor;
}


//--------------------------------------------------------------------------
//  register


bool medImageViewFactory::registerView(QString identifier, QStringList typeHandled, viewCreator creator)
{
    if(d->viewCreators.contains(identifier))
        return false;

    d->viewCreators.insert(identifier, creator);
    d->viewIdentifierHash.insert(identifier, typeHandled);
    return true;
}

bool medImageViewFactory::registerNavigator(QString identifier, QStringList typeHandled, navigatorCreator creator)
{
    if(d->navigatorCreators.contains(identifier))
        return false;

    d->navigatorCreators.insert(identifier, creator);
    d->navigatorIdentifierHash.insert(identifier, typeHandled);
    return true;
}

bool medImageViewFactory::registerInteractor(QString identifier, QStringList typeHandled, interactorCreator creator)
{
    if(d->intercatorCreators.contains(identifier))
        return false;

    d->intercatorCreators.insert(identifier, creator);
    d->interactorIdentifierHash.insert(identifier, typeHandled);
    return true;
}

//--------------------------------------------------------------------------
//  get handler

QStringList medImageViewFactory::navigatorsAbleToHandle(const QString viewType) const
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

QStringList medImageViewFactory::interactorsAbleToHandle(const QString viewType, const QString dataType) const
{
    qDebug() << "dataType" << dataType;
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

QStringList medImageViewFactory::viewsAbleToHandle(const QString dataType) const
{
    if(d->viewIdentifierHash.isEmpty())
        return QStringList();

    QHashIterator<QString, QStringList> it(d->interactorIdentifierHash);

    QList<QString> views;

    while(it.hasNext())
    {
        it.next();
        if (it.value().contains(dataType))
            views << it.key();
    }
    return views;
}


//--------------------------------------------------------------------------
//  construc / destruct factory


medImageViewFactory *medImageViewFactory::s_instance = NULL;



