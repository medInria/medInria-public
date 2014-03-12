/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractFactory.h>

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

    virtual medAbstractView* createView(QString identifier, QObject *parent = 0);
    virtual medAbstractViewNavigator* createNavigator(QString identifier, medAbstractView *parent);
    virtual medAbstractViewInteractor*  createInteractor(QString identifier, medAbstractView *parent);
    medAbstractNavigator* createAdditionalNavigator(QString identifier, medAbstractView *parent);
    medAbstractInteractor*  createAdditionalInteractor(QString identifier, medAbstractView *parent);

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
    template <typename T>
    bool registerAdditionalNavigator(QString identifier, QStringList typeHandled)
    {
        addNavigatorCreator creator = createAdditionalNavigator<T>;
        return registerAdditionalNavigator(identifier, typeHandled, creator);
    }
    template <typename T>
    bool registerAdditionalInteractor(QString identifier, QStringList typeHandled)
    {
        addInteractorCreator creator = creatAdditionalInteractor<T>;
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


    /** Singleton holder.*/
    static medViewFactory *s_instance;


    template < typename T>
    static medAbstractView* createView(QObject* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractViewNavigator* createNavigator(medAbstractView* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractViewInteractor* creatInteractor(medAbstractView* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractNavigator* createAdditionalNavigator(medAbstractView* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractInteractor* creatAdditionalInteractor(medAbstractView* parent)
    {
        return (new T(parent));
    }

private:
    medViewFactoryPrivate* d;
};


