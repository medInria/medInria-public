/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medViewFactory.h"


#include <medAbstractView.h>
#include <medAbstractViewNavigator.h>
#include <medAbstractViewInteractor.h>

#include <medAbstractLayeredView.h>
#include <medAbstractLayeredViewNavigator.h>
#include <medAbstractLayeredViewInteractor.h>

#include <medAbstractImageView.h>
#include <medAbstractImageViewNavigator.h>
#include <medAbstractImageViewInteractor.h>



medViewFactory *medViewFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medViewFactory;

    return s_instance;
}


//--------------------------------------------------------------------------
//  register

bool medViewFactory::registered(QString &implementationOf,
                                QString& identifier,
                                QStringList& typeHandled,
                                medAbstractViewCreator creator)
{
    if(_prvt_creators1.contains(identifier))
        return false;

    _prvt_creators1.insert(identifier, creator);
    _prvt_identifierHash.insert(identifier, typeHandled);
    _prvt_implementOfHash.insert(identifier, implementationOf);
    return true;    
}

bool medViewFactory::registered(QString &implementationOf,
                                QString& identifier,
                                QStringList& typeHandled,
                                medAbstractLayeredViewCreator creator)
{
    if(_prvt_creators2.contains(identifier))
        return false;

    _prvt_creators2.insert(identifier, creator);
    _prvt_identifierHash.insert(identifier, typeHandled);
    _prvt_implementOfHash.insert(identifier, implementationOf);
    return true;
}

bool medViewFactory::registered(QString &implementationOf,
                                QString& identifier,
                                QStringList& typeHandled,
                                medAbstractImageViewCreator creator)
{
    if(_prvt_creators3.contains(identifier))
        return false;

    _prvt_creators3.insert(identifier, creator);
    _prvt_identifierHash.insert(identifier, typeHandled);
    _prvt_implementOfHash.insert(identifier, implementationOf);
    return true;
}


bool medViewFactory::registered(QString &implementationOf,
                                QString& identifier,
                                QStringList& typeHandled,
                                medAbstractNavigatorCreator creator)
{
    if(_prvt_creators4.contains(identifier))
        return false;

    _prvt_creators4.insert(identifier, creator);
    _prvt_identifierHash.insert(identifier, typeHandled);
    _prvt_implementOfHash.insert(identifier, implementationOf);
    return true;
}

bool medViewFactory::registered(QString &implementationOf,
                                QString& identifier,
                                QStringList& typeHandled,
                                medAbstractViewNavigatorCreator creator)
{
    if(_prvt_creators5.contains(identifier))
        return false;

    _prvt_creators5.insert(identifier, creator);
    _prvt_identifierHash.insert(identifier, typeHandled);
    _prvt_implementOfHash.insert(identifier, implementationOf);
    return true;
}

bool medViewFactory::registered(QString &implementationOf,
                                QString& identifier,
                                QStringList& typeHandled,
                                medAbstractLayeredViewNavigatorCreator creator)
{
    if(_prvt_creators6.contains(identifier))
        return false;

    _prvt_creators6.insert(identifier, creator);
    _prvt_identifierHash.insert(identifier, typeHandled);
    _prvt_implementOfHash.insert(identifier, implementationOf);
    return true;
}

bool medViewFactory::registered(QString &implementationOf,
                                QString& identifier,
                                QStringList& typeHandled,
                                medAbstractImageViewNavigatorCreator creator)
{
    if(_prvt_creators7.contains(identifier))
        return false;

    _prvt_creators7.insert(identifier, creator);
    _prvt_identifierHash.insert(identifier, typeHandled);
    _prvt_implementOfHash.insert(identifier, implementationOf);
    return true;
}


bool medViewFactory::registered(QString &implementationOf,
                                QString& identifier,
                                QStringList& typeHandled,
                                medAbstractInteractorCreator creator)
{
    if(_prvt_creators8.contains(identifier))
        return false;

    _prvt_creators8.insert(identifier, creator);
    _prvt_identifierHash.insert(identifier, typeHandled);
    _prvt_implementOfHash.insert(identifier, implementationOf);
    return true;
}

bool medViewFactory::registered(QString &implementationOf,
                                QString& identifier,
                                QStringList& typeHandled,
                                medAbstractViewInteractorCreator creator)
{
    if(_prvt_creators9.contains(identifier))
        return false;

    _prvt_creators9.insert(identifier, creator);
    _prvt_identifierHash.insert(identifier, typeHandled);
    _prvt_implementOfHash.insert(identifier, implementationOf);
    return true;
}

bool medViewFactory::registered(QString &implementationOf,
                                QString& identifier,
                                QStringList& typeHandled,
                                medAbstractLayeredViewInteractorCreator creator)
{
    if(_prvt_creators10.contains(identifier))
        return false;

    _prvt_creators10.insert(identifier, creator);
    _prvt_identifierHash.insert(identifier, typeHandled);
    _prvt_implementOfHash.insert(identifier, implementationOf);
    return true;
}

bool medViewFactory::registered(QString &implementationOf,
                                QString& identifier,
                                QStringList& typeHandled,
                                medAbstractImageViewInteractorCreator creator)
{
    if(_prvt_creators11.contains(identifier))
        return false;

    _prvt_creators11.insert(identifier, creator);
    _prvt_identifierHash.insert(identifier, typeHandled);
    _prvt_implementOfHash.insert(identifier, implementationOf);
    return true;
}


//--------------------------------------------------------------------------
//  get handler

QList<QString> medViewFactory::navigatorsAbleToHandle(const QString& viewType) const
{
    QHashIterator<QString, QStringList> it(_prvt_identifierHash);

    QList<QString> navigators;

    for(it.toFront(); it.hasNext(); it.next())
    {
        if (_prvt_implementOfHash.value(it.key()).contains("Navigator") &&
                it.value().contains(viewType))
                navigators << it.key();
    }

    return navigators;
}

QList<QString> medViewFactory::interactorsAbleToHandle(const QString& viewType, const QString& dataType) const
{
    QHashIterator<QString, QStringList> it(_prvt_identifierHash);

    QList<QString> interactors;

    for(it.toFront(); it.hasNext(); it.next())
    {
        if (_prvt_implementOfHash.value(it.key()).contains("Interactor") &&
                it.value().contains(viewType) &&
                it.value().contains(dataType))
                interactors << it.key();
    }

    return interactors;
}


//--------------------------------------------------------------------------
//  construc / destruct factory


medViewFactory *medViewFactory::s_instance = NULL;



