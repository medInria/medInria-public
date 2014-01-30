/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medExtraInteractorFactory.h>


medExtraInteractorFactory *medExtraInteractorFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medExtraInteractorFactory;

    return s_instance;
}



class medExtraInteractorFactoryPrivate
{
    typedef QHash <QString, medExtraInteractorFactory::interactorCreator>  interactorIdCreatorHash;
    typedef QHash <QString, QStringList>        identifierHandledTypeHash;
    public:


    interactorIdCreatorHash   intercatorCreators;
    identifierHandledTypeHash identifierHash;
};

medExtraInteractorFactory::medExtraInteractorFactory()
{
    d = new medExtraInteractorFactoryPrivate;
}
medExtraInteractorFactory::~medExtraInteractorFactory()
{
    delete d;
}

//--------------------------------------------------------------------------
//  create

medAbstractExtraInteractor*  medExtraInteractorFactory::createInteractor(QString identifier, medAbstractView *parent)
{
    medAbstractExtraInteractor* interactor = NULL;
    interactorCreator c = NULL;
    c = d->intercatorCreators.value(identifier);
    if(c)
        interactor = (c)(parent);

    return interactor;
}


//--------------------------------------------------------------------------
//  register


bool medExtraInteractorFactory::registerInteractor(QString identifier, QStringList typeHandled, interactorCreator creator)
{
    if(d->intercatorCreators.contains(identifier))
        return false;

    d->intercatorCreators.insert(identifier, creator);
    d->identifierHash.insert(identifier, typeHandled);
    return true;
}

//--------------------------------------------------------------------------
//  get handler


QStringList medExtraInteractorFactory::interactorsAbleToHandle(const QString viewType, const QString dataType) const
{
    if(d->identifierHash.isEmpty())
        return QStringList();

    QHashIterator<QString, QStringList> it(d->identifierHash);

    QList<QString> interactors;

    for(it.toFront(); it.hasNext(); it.next())
    {
        if (it.value().contains(viewType) && it.value().contains(dataType))
            interactors << it.key();
    }

    return interactors;
}

medExtraInteractorFactory *medExtraInteractorFactory::s_instance = NULL;



