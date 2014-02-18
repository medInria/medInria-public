/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include  <medLayeredViewFactory.h>

#include <medCoreExport.h>

#include <QStringList>

#include <medAbstractImageView.h>
#include <medAbstractImageViewNavigator.h>
#include <medAbstractImageViewInteractor.h>


class medImageViewFactoryPrivate;
class MEDCORE_EXPORT medImageViewFactory : public medLayeredViewFactory
{
    Q_OBJECT


public:
    static medImageViewFactory *instance();

    typedef medAbstractImageView *(*viewCreator)(QObject *parent);
    typedef medAbstractImageViewNavigator *(*navigatorCreator)(medAbstractImageView *parent);
    typedef medAbstractImageViewInteractor *(*interactorCreator)(medAbstractImageView *parent);

    virtual medAbstractImageView* createView(QString identifier, QObject *parent = 0);
    virtual medAbstractImageViewNavigator* createNavigator(QString identifier, medAbstractImageView *parent);
    virtual medAbstractImageViewInteractor*  createInteractor(QString identifier, medAbstractImageView *parent);


    template <typename T>
    bool registerView(QString identifier, QStringList typeHandled)
    {
        viewCreator creator = createView<T>;
        return registerView(identifier, typeHandled, creator);
    }
    template <typename T>
    bool registerNavigator(QString identifier, QStringList typeHandled)
    {
        navigatorCreator creator = createNavigator<T>;
        return registerNavigator(identifier, typeHandled, creator);
    }
    template <typename T>
    bool registerInteractor(QString identifier, QStringList typeHandled)
    {
        interactorCreator creator = creatInteractor<T>;
        return registerInteractor(identifier, typeHandled, creator);
    }

    virtual QStringList navigatorsAbleToHandle(const QString viewType) const;
    virtual QStringList interactorsAbleToHandle(const QString viewType, const QString dataType) const;
    virtual QStringList viewsAbleToHandle(const QString dataType) const;


protected:
     medImageViewFactory();
    ~medImageViewFactory();

private:
     virtual bool registerView(QString identifier, QStringList typeHandled, viewCreator creator);
     virtual bool registerNavigator(QString identifier, QStringList typeHandled, navigatorCreator creator);
     virtual bool registerInteractor(QString identifier, QStringList typeHandled, interactorCreator creator);


    /** Singleton holder.*/
    static medImageViewFactory *s_instance;

    template < typename T>
    static medAbstractImageView* createView(QObject* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractImageViewNavigator* createNavigator(medAbstractImageView* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractImageViewInteractor* creatInteractor(medAbstractImageView* parent)
    {
        return (new T(parent));
    }

private:
    medImageViewFactoryPrivate* d;
};


