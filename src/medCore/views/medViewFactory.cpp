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

typedef QPair <QString, medAbstractViewCreator>                     medIdentifierCreatorPair1;
typedef QPair <QString, medAbstractLayeredViewCreator>                  medIdentifierCreatorPair2;
typedef QPair <QString, medAbstractImageViewCreator>                medIdentifierCreatorPair3;

typedef QPair <QString, medAbstractNavigatorCreator>                medIdentifierCreatorPair4;
typedef QPair <QString, medAbstractViewNavigatorCreator>            medIdentifierCreatorPair5;
typedef QPair <QString, medAbstractLayeredViewNavigatorCreator>     medIdentifierCreatorPair6;
typedef QPair <QString, medAbstractImageViewNavigatorCreator>       medIdentifierCreatorPair7;

typedef QPair <QString, medAbstractInteractorCreator>               medIdentifierCreatorPair8;
typedef QPair <QString, medAbstractViewInteractorCreator>           medIdentifierCreatorPair9;
typedef QPair <QString, medAbstractLayeredViewInteractorCreator>    medIdentifierCreatorPair10;
typedef QPair <QString, medAbstractImageViewInteractorCreator>      medIdentifierCreatorPair11;

typedef QPair <QString, QString>                                    medViewTypeDataTypePair;
typedef QString                                                     medViewTypeString;


typedef QList <medIdentifierCreatorPair1>                           medViewCreatorList1;
typedef QList <medIdentifierCreatorPair2>                           medViewCreatorList2;
typedef QList <medIdentifierCreatorPair3>                           medViewCreatorList3;

typedef QHash <medIdentifierCreatorPair4, medViewTypeString>        medNavigatorCreatorHash4;
typedef QHash <medIdentifierCreatorPair5, medViewTypeString>        medNavigatorCreatorHash5;
typedef QHash <medIdentifierCreatorPair6, medViewTypeString>        medNavigatorCreatorHash6;
typedef QHash <medIdentifierCreatorPair7, medViewTypeString>        medNavigatorCreatorHash7;

typedef QHash <medIdentifierCreatorPair8, medViewTypeDataTypePair>  medInteractorCreatorHash8;
typedef QHash <medIdentifierCreatorPair9, medViewTypeDataTypePair>  medInteractorCreatorHash9;
typedef QHash <medIdentifierCreatorPair10, medViewTypeDataTypePair> medInteractorCreatorHash10;
typedef QHash <medIdentifierCreatorPair11, medViewTypeDataTypePair> medInteractorCreatorHash11;


class medViewFactoryPrivate
{
public:
    medViewCreatorList1         viewCreators1;
    medViewCreatorList2         viewCreators2;
    medViewCreatorList3         viewCreators3;

    medNavigatorCreatorHash4    navigatorCreators4;
    medNavigatorCreatorHash5    navigatorCreators5;
    medNavigatorCreatorHash6    navigatorCreators6;
    medNavigatorCreatorHash7    navigatorCreators7;

    medInteractorCreatorHash8   interactorCreators8;
    medInteractorCreatorHash9   interactorCreators9;
    medInteractorCreatorHash10  interactorCreators10;
    medInteractorCreatorHash11  interactorCreators11;
};

medViewFactory *medViewFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medViewFactory;

    return s_instance;
}


//--------------------------------------------------------------------------
//  register

bool medViewFactory::registerView(QString& identifier,
                                  medAbstractViewCreator creator)
{
    foreach (medIdentifierCreatorPair1 vc, d->viewCreators1)
    {
        if(vc.first == identifier)
            return false;
    }

    d->viewCreators1.append(medIdentifierCreatorPair1(identifier, creator));
    return true;
}

bool medViewFactory::registerView(QString& identifier,
                                  medAbstractLayeredViewCreator creator)
{
    foreach (medIdentifierCreatorPair2 vc, d->viewCreators2)
    {
        if(vc.first == identifier)
            return false;
    }

    d->viewCreators2.append(medIdentifierCreatorPair2(identifier, creator));
    return true;
}

bool medViewFactory::registerView(QString& identifier,
                                  medAbstractImageViewCreator creator)
{
    foreach (medIdentifierCreatorPair3 vc, d->viewCreators3)
    {
        if(vc.first == identifier)
            return false;
    }

    d->viewCreators3.append(medIdentifierCreatorPair3(identifier, creator));
    return true;
}


bool medViewFactory::registerNavigator(QString& identifier,
                                       QString& viewTypeHandled,
                                       medAbstractNavigatorCreator creator)
{
    foreach(medIdentifierCreatorPair4 nc, d->navigatorCreators4.keys())
    {
        if(nc.first == identifier)
            return false;
    }

    d->navigatorCreators4.insert(medIdentifierCreatorPair4(identifier, creator), viewTypeHandled);
    return true;
}

bool medViewFactory::registerNavigator(QString& identifier,
                                       QString& viewTypeHandled,
                                       medAbstractViewNavigatorCreator creator)
{
    foreach(medIdentifierCreatorPair5 nc, d->navigatorCreators5.keys())
    {
        if(nc.first == identifier)
            return false;
    }

    d->navigatorCreators5.insert(medIdentifierCreatorPair5(identifier, creator), viewTypeHandled);
    return true;
}

bool medViewFactory::registerNavigator(QString& identifier,
                                       QString& viewTypeHandled,
                                       medAbstractLayeredViewNavigatorCreator creator)
{
    foreach(medIdentifierCreatorPair6 nc, d->navigatorCreators6.keys())
    {
        if(nc.first == identifier)
            return false;
    }

    d->navigatorCreators6.insert(medIdentifierCreatorPair6(identifier, creator), viewTypeHandled);
    return true;
}

bool medViewFactory::registerNavigator(QString& identifier,
                                       QString& viewTypeHandled,
                                       medAbstractImageViewNavigatorCreator creator)
{
    foreach(medIdentifierCreatorPair7 nc, d->navigatorCreators7.keys())
    {
        if(nc.first == identifier)
            return false;
    }

    d->navigatorCreators7.insert(medIdentifierCreatorPair7(identifier, creator), viewTypeHandled);
    return true;
}


bool medViewFactory::registerInteractor(QString& identifier,
                                        QString& viewTypeHandled,
                                        QString& dataTypeHandled,
                                        medAbstractInteractorCreator creator)
{
    foreach(medIdentifierCreatorPair8 ic, d->interactorCreators8.keys())
    {
        if(ic.first == identifier)
            return false;
    }

    d->interactorCreators8.insert(medIdentifierCreatorPair8(identifier, creator),
                                medViewTypeDataTypePair(viewTypeHandled, dataTypeHandled));
    return true;
}

bool medViewFactory::registerInteractor(QString& identifier,
                                        QString& viewTypeHandled,
                                        QString& dataTypeHandled,
                                        medAbstractViewInteractorCreator creator)
{
    foreach(medIdentifierCreatorPair9 ic, d->interactorCreators9.keys())
    {
        if(ic.first == identifier)
            return false;
    }

    d->interactorCreators9.insert(medIdentifierCreatorPair9(identifier, creator),
                                medViewTypeDataTypePair(viewTypeHandled, dataTypeHandled));
    return true;
}

bool medViewFactory::registerInteractor(QString& identifier,
                                        QString& viewTypeHandled,
                                        QString& dataTypeHandled,
                                        medAbstractLayeredViewInteractorCreator creator)
{
    foreach(medIdentifierCreatorPair10 ic, d->interactorCreators10.keys())
    {
        if(ic.first == identifier)
            return false;
    }

    d->interactorCreators10.insert(medIdentifierCreatorPair10(identifier, creator),
                                medViewTypeDataTypePair(viewTypeHandled, dataTypeHandled));
    return true;
}

bool medViewFactory::registerInteractor(QString& identifier,
                                        QString& viewTypeHandled,
                                        QString& dataTypeHandled,
                                        medAbstractImageViewInteractorCreator creator)
{
    foreach(medIdentifierCreatorPair11 ic, d->interactorCreators11.keys())
    {
        if(ic.first == identifier)
            return false;
    }

    d->interactorCreators11.insert(medIdentifierCreatorPair11(identifier, creator),
                                medViewTypeDataTypePair(viewTypeHandled, dataTypeHandled));
    return true;
}



//--------------------------------------------------------------------------
//  get handler

QList<QString> medViewFactory::navigatorsAbleToHandle(const QString& viewType) const
{
    QList<QString> navigators;
    medNavigatorCreatorHash4::Iterator it4;
    for(it4 != d->navigatorCreators4.begin(); it4 != d->navigatorCreators4.end(); ++it4)
    {
        if(it4.value() == viewType)
            navigators << it4.key().first;
    }
    medNavigatorCreatorHash5::Iterator it5;
    for(it5 = d->navigatorCreators5.begin(); it5 != d->navigatorCreators5.end(); ++it5)
    {
        if(it5.value() == viewType)
            navigators << it5.key().first;
    }
    medNavigatorCreatorHash6::Iterator it6;
    for(it6 = d->navigatorCreators6.begin(); it6 != d->navigatorCreators6.end(); ++it6)
    {
        if(it6.value() == viewType)
            navigators << it6.key().first;
    }
    medNavigatorCreatorHash7::Iterator it7;
    for(it7 = d->navigatorCreators7.begin(); it7 != d->navigatorCreators7.end(); ++it7)
    {
        if(it7.value() == viewType)
            navigators << it7.key().first;
    }
    return navigators;
}

QList<QString> medViewFactory::interactorsAbleToHandle(const QString& viewType, const QString& dataType) const
{
    QList<QString> interactors;
    medInteractorCreatorHash8::Iterator it8;
    for(it8 = d->interactorCreators8.begin(); it8 != d->interactorCreators8.end(); ++it8)
    {
        if(it8.value() == medViewTypeDataTypePair(viewType, dataType))
            interactors << it8.key().first;
    }
    medInteractorCreatorHash9::Iterator it9;
    for(it9 = d->interactorCreators9.begin(); it9 != d->interactorCreators9.end(); ++it9)
    {
        if(it9.value() == medViewTypeDataTypePair(viewType, dataType))
            interactors << it9.key().first;
    }
    medInteractorCreatorHash10::Iterator it10;
    for(it10 = d->interactorCreators10.begin(); it10 != d->interactorCreators10.end(); ++it10)
    {
        if(it10.value() == medViewTypeDataTypePair(viewType, dataType))
            interactors << it10.key().first;
    }
    medInteractorCreatorHash11::Iterator it11;
    for(it11 = d->interactorCreators11.begin(); it11 != d->interactorCreators11.end(); ++it11)
    {
        if(it11.value() == medViewTypeDataTypePair(viewType, dataType))
            interactors << it11.key().first;
    }
    return interactors;
}



//--------------------------------------------------------------------------
//  create views

medAbstractView* medViewFactory::createAbstractView(QString &identifier,
                                                    QObject *parent)
{
    medAbstractViewCreator creator = 0;
    foreach (medIdentifierCreatorPair1 vc, d->viewCreators1)
    {
        if(vc.first == identifier)
        {
            creator = vc.second;
            break;
        }
    }

    if(!creator)
        return NULL;

    medAbstractView *abstractView = (creator(parent));
    return abstractView;
}

medAbstractLayeredView* medViewFactory::createAbstractLayeredView(QString &identifier,
                                                                  QObject *parent)
{
    medAbstractLayeredViewCreator creator = 0;
    foreach (medIdentifierCreatorPair2 vc, d->viewCreators2)
    {
        if(vc.first == identifier)
        {
            creator = vc.second;
            break;
        }
    }

    if(!creator)
        return NULL;

    medAbstractLayeredView *abstractLayerdView = (creator(parent));
    return abstractLayerdView;
}


medAbstractImageView* medViewFactory::createAbstractImageView(QString &identifier,
                                                               QObject *parent)
{
    medAbstractImageViewCreator creator = 0;
    foreach (medIdentifierCreatorPair3 vc, d->viewCreators3)
    {
        if(vc.first == identifier)
        {
            creator = vc.second;
            break;
        }
    }

    if(!creator)
        return NULL;

    medAbstractImageView *abstractImageView = (creator(parent));
    return abstractImageView;
}


//--------------------------------------------------------------------------
//  create navigators


medAbstractNavigator* medViewFactory::createAbstractNavigator(QString &identifier,
                                                              medAbstractView *parent)
{
    medAbstractNavigatorCreator creator = 0;
    foreach (medIdentifierCreatorPair4 nc, d->navigatorCreators4.keys())
    {
        if(nc.first == identifier)
        {
            creator = nc.second;
            break;
        }
    }

    if(!creator)
        return NULL;

    medAbstractNavigator *abstractNavigator = (creator(parent));
    return abstractNavigator;
}

medAbstractViewNavigator* medViewFactory::createAbstractViewNavigator(QString &identifier,
                                                                      medAbstractView *parent)
{
    medAbstractViewNavigatorCreator creator = 0;
    foreach (medIdentifierCreatorPair5 nc, d->navigatorCreators5.keys())
    {
        if(nc.first == identifier)
        {
            creator = nc.second;
            break;
        }
    }

    if(!creator)
        return NULL;

    medAbstractViewNavigator *abstractViewNavigator = (creator(parent));
    return abstractViewNavigator;
}

medAbstractLayeredViewNavigator* medViewFactory::createAbstractLayeredViewNavigator(QString &identifier,
                                                                                    medAbstractLayeredView *parent)
{
    medAbstractLayeredViewNavigatorCreator creator = 0;
    foreach (medIdentifierCreatorPair6 nc, d->navigatorCreators6.keys())
    {
        if(nc.first == identifier)
        {
            creator = nc.second;
            break;
        }
    }

    if(!creator)
        return NULL;

    medAbstractLayeredViewNavigator *abstractLayerdViewNavigator = (creator(parent));
    return abstractLayerdViewNavigator;
}


medAbstractImageViewNavigator* medViewFactory::createAbstractImageViewNavigator(QString &identifier,
                                                                                 medAbstractImageView *parent)
{
    medAbstractImageViewNavigatorCreator creator = 0;
    foreach (medIdentifierCreatorPair7 nc, d->navigatorCreators7.keys())
    {
        if(nc.first == identifier)
        {
            creator = nc.second;
            break;
        }
    }

    if(!creator)
        return NULL;

    medAbstractImageViewNavigator *abstractImageViewNavigator = (creator(parent));
    return abstractImageViewNavigator;
}


//--------------------------------------------------------------------------
//  create interactors

medAbstractInteractor* medViewFactory::createAbstractInteractor(QString &identifier,
                                                               medAbstractView *parent)
{
    medAbstractInteractorCreator creator = 0;
    foreach (medIdentifierCreatorPair8 ic, d->interactorCreators8.keys())
    {
        if(ic.first == identifier)
        {
            creator = ic.second;
            break;
        }
    }

    if(!creator)
        return NULL;

    medAbstractInteractor* abstractNavigator = (creator(parent));
    return abstractNavigator;
}

medAbstractViewInteractor* medViewFactory::createAbstractViewInteractor(QString &identifier,
                                                                       medAbstractView* parent)
{
    medAbstractViewInteractorCreator creator = 0;
    foreach (medIdentifierCreatorPair9 ic, d->interactorCreators9.keys())
    {
        if(ic.first == identifier)
        {
            creator = ic.second;
            break;
        }
    }
    if(!creator)
        return NULL;

    medAbstractViewInteractor *abstractViewInteractor = (creator(parent));
    return abstractViewInteractor;
}

medAbstractLayeredViewInteractor* medViewFactory::createAbstractLayeredViewInteractor(QString &identifier,
                                                                                     medAbstractLayeredView *parent)
{
    medAbstractLayeredViewInteractorCreator creator = 0;
    foreach (medIdentifierCreatorPair10 ic, d->interactorCreators10.keys())
    {
        if(ic.first == identifier)
        {
            creator = ic.second;
            break;
        }
    }

    if(!creator)
        return NULL;

    medAbstractLayeredViewInteractor* abstractLayerdViewInteractor = (creator(parent));
    return abstractLayerdViewInteractor;
}


medAbstractImageViewInteractor* medViewFactory::createAbstractImageViewInteractor(QString &identifier,
                                                                                  medAbstractImageView *parent)
{
    medAbstractImageViewInteractorCreator creator = 0;
    foreach (medIdentifierCreatorPair11 ic, d->interactorCreators11.keys())
    {
        if(ic.first == identifier)
        {
            creator = ic.second;
            break;
        }
    }

    if(!creator)
        return NULL;

    medAbstractImageViewInteractor* abstractImageViewInteractor = (creator(parent));
    return abstractImageViewInteractor;
}


//--------------------------------------------------------------------------
//  construc / destruct factory

medViewFactory::medViewFactory(void) : dtkAbstractFactory(), d(new medViewFactoryPrivate)
{

}

medViewFactory::~medViewFactory(void)
{
    delete d;
}

medViewFactory *medViewFactory::s_instance = NULL;



