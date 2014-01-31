/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medViewFactory.h>

#include <medCoreExport.h>

#include <QStringList>

#include <medAbstractLayeredView.h>
#include <medAbstractLayeredViewNavigator.h>
#include <medAbstractLayeredViewInteractor.h>




class medLayeredViewFactoryPrivate;
class MEDCORE_EXPORT medLayeredViewFactory : public medViewFactory
{
    Q_OBJECT
public:
    typedef medAbstractLayeredView *(*viewCreator)(QObject *parent);
    typedef medAbstractLayeredViewNavigator *(*navigatorCreator)(medAbstractLayeredView *parent);
    typedef medAbstractLayeredViewInteractor *(*interactorCreator)(medAbstractLayeredView *parent);

    static medLayeredViewFactory *instance();

    virtual medAbstractLayeredView* createView(QString identifier, QObject *parent = 0);
    virtual medAbstractLayeredViewNavigator* createNavigator(QString identifier, medAbstractLayeredView *parent);
    virtual medAbstractLayeredViewInteractor*  createInteractor(QString identifier, medAbstractLayeredView *parent);


    template <typename T>
    bool registerView(QString identifier, QStringList typeHandled)
    {
        viewCreator creator = _prvt_createView<T>;
        return registerView(identifier, typeHandled, creator);
    }
    template <typename T>
    bool registerNavigator(QString identifier, QStringList typeHandled)
    {
        navigatorCreator creator = _prvt_createNavigator<T>;
        return registerNavigator(identifier, typeHandled, creator);
    }
    template <typename T>
    bool registerInteractor(QString identifier, QStringList typeHandled)
    {
        interactorCreator creator = _prvt_creatInteractor<T>;
        return registerInteractor(identifier, typeHandled, creator);
    }

    virtual QStringList navigatorsAbleToHandle(const QString viewType) const;
    virtual QStringList interactorsAbleToHandle(const QString viewType, const QString dataType) const;
    virtual QStringList viewsAbleToHandle(const QString dataType) const;


protected:
     medLayeredViewFactory();
    ~medLayeredViewFactory();

private:
     virtual bool registerView(QString identifier, QStringList typeHandled, viewCreator creator);
     virtual bool registerNavigator(QString identifier, QStringList typeHandled, navigatorCreator creator);
     virtual bool registerInteractor(QString identifier, QStringList typeHandled, interactorCreator creator);


    /** Singleton holder.*/
    static medLayeredViewFactory *s_instance;


    template < typename T>
    static medAbstractLayeredView* _prvt_createView(QObject* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractLayeredViewNavigator* _prvt_createNavigator(medAbstractLayeredView* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractLayeredViewInteractor* _prvt_creatInteractor(medAbstractLayeredView* parent)
    {
        return (new T(parent));
    }

private:
    medLayeredViewFactoryPrivate* d;
};


