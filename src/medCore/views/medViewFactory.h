/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCoreSupport/dtkAbstractFactory.h>

#include <medCoreExport.h>

#include <QStringList>

#include <medAbstractView.h>
#include <medAbstractViewNavigator.h>
#include <medAbstractViewInteractor.h>
#include <medAbstractNavigator.h>
#include <medAbstractInteractor.h>


typedef medAbstractView *(*viewCreator)(QObject *parent);
typedef medAbstractViewNavigator *(*navigatorCreator)(medAbstractView *parent);
typedef medAbstractViewInteractor *(*interactorCreator)(medAbstractView *parent);
typedef medAbstractNavigator *(*addNavigatorCreator)(medAbstractView *parent);
typedef medAbstractInteractor *(*addInteractorCreator)(medAbstractView *parent);

class medViewFactoryPrivate;
class MEDCORE_EXPORT medViewFactory : public dtkAbstractFactory
{
    Q_OBJECT
public:
    static medViewFactory *instance();

    template <typename T>
    T* createView(QString identifier, QObject *parent = 0);

    template <typename T>
    T* createNavigator(QString identifier, medAbstractView *parent);

    template <typename T>
    T* createInteractor(QString identifier, medAbstractView *parent);

    medAbstractNavigator* createAdditionalNavigator(QString identifier, medAbstractView *parent);
    medAbstractInteractor*  createAdditionalInteractor(QString identifier, medAbstractView *parent);

    template <typename T>
    bool registerView(QString identifier, QStringList typeHandled)
    {
        viewCreator creator = newView<T>;
        return registerView(identifier, typeHandled, creator);
    }
    template <typename T>
    bool registerNavigator(QString identifier, QStringList typeHandled)
    {
        navigatorCreator creator = newNavigator<T>;
        return registerNavigator(identifier, typeHandled, creator);
    }
    template <typename T>
    bool registerInteractor(QString identifier, QStringList typeHandled)
    {
        interactorCreator creator = newInteractor<T>;
        return registerInteractor(identifier, typeHandled, creator);
    }
    template <typename T>
    bool registerAdditionalNavigator(QString identifier, QStringList typeHandled)
    {
        addNavigatorCreator creator = newAdditionalNavigator<T>;
        return registerAdditionalNavigator(identifier, typeHandled, creator);
    }
    template <typename T>
    bool registerAdditionalInteractor(QString identifier, QStringList typeHandled)
    {
        addInteractorCreator creator = newAdditionalInteractor<T>;
        return registerAdditionalInteractor(identifier, typeHandled, creator);
    }

    virtual QStringList viewsAbleToHandle(const QString dataType) const;
    virtual QStringList navigatorsAbleToHandle(const QString viewType) const;
    virtual QStringList interactorsAbleToHandle(const QString viewType, const QString dataType) const;
    QStringList additionalNavigatorsAbleToHandle(const QString dataType) const;
    QStringList additionalInteractorsAbleToHandle(const QString viewType, const QString dataType) const;


protected:
     medViewFactory();
    ~medViewFactory();

private:
     virtual bool registerView(QString identifier, QStringList typeHandled, viewCreator creator);
     virtual bool registerNavigator(QString identifier, QStringList typeHandled, navigatorCreator creator);
     virtual bool registerInteractor(QString identifier, QStringList typeHandled, interactorCreator creator);
     bool registerAdditionalNavigator(QString identifier, QStringList typeHandled, addNavigatorCreator creator);
     bool registerAdditionalInteractor(QString identifier, QStringList typeHandled, addInteractorCreator creator);

     viewCreator       getViewCreator(QString identifier);
     interactorCreator getInteractorCreator(QString identifier);
     navigatorCreator  getNavigatorCreator(QString identifier);


    /** Singleton holder.*/
    static medViewFactory *s_instance;


    template < typename T>
    static medAbstractView* newView(QObject* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractViewNavigator* newNavigator(medAbstractView* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractViewInteractor* newInteractor(medAbstractView* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractNavigator* newAdditionalNavigator(medAbstractView* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractInteractor* newAdditionalInteractor(medAbstractView* parent)
    {
        return (new T(parent));
    }

private:
    medViewFactoryPrivate* d;
};

// ----------------- template definitions -----------------

template <typename T>
T* medViewFactory::createView(QString identifier, QObject *parent)
{
    medAbstractView* view = NULL;
    viewCreator c = NULL;
    c = this->getViewCreator(identifier);
    if(c)
        view = (c)(parent);

    T* viewCast = dynamic_cast<T*>(view);
    if (!viewCast)
        delete view;

    return viewCast;
}

template <typename T>
T* medViewFactory::createNavigator(QString identifier, medAbstractView *parent)
{

    medAbstractViewNavigator* navigator = NULL;
    navigatorCreator c = NULL;
    c = this->getNavigatorCreator(identifier);
    if(c)
        navigator = (c)(parent);

    T* navigatorCast = dynamic_cast<T*>(navigator);
    if (!navigatorCast)
        delete navigator;

    return navigatorCast;
}

template <typename T>
T*  medViewFactory::createInteractor(QString identifier, medAbstractView *parent)
{
    medAbstractViewInteractor* interactor = NULL;
    interactorCreator c = NULL;
    c = this->getInteractorCreator(identifier);
    if(c)
        interactor = (c)(parent);

    T* interactorCast = dynamic_cast<T*>(interactor);
    if (!interactorCast)
        delete interactor;

    return interactorCast;
}
