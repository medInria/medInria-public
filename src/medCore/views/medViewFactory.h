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


typedef medAbstractView *(*viewCreator)(QObject *parent);
typedef medAbstractViewNavigator *(*navigatorCreator)(medAbstractView *parent);
typedef medAbstractViewInteractor *(*interactorCreator)(medAbstractView *parent);

class medViewFactoryPrivate;
class MEDCORE_EXPORT medViewFactory : public dtkAbstractFactory
{
    Q_OBJECT
public:
    static medViewFactory *instance();

    medAbstractView* createView(QString identifier, QObject *parent = 0);
    medAbstractViewNavigator* createNavigator(QString identifier, medAbstractView *parent);
    medAbstractViewInteractor*  createInteractor(QString identifier, medAbstractView *parent);


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

    QStringList navigatorsAbleToHandle(const QString viewType) const;
    QStringList interactorsAbleToHandle(const QString viewType, const QString dataType) const;
    QStringList viewsAbleToHandle(const QString dataType) const;


protected:
     medViewFactory();
    ~medViewFactory();

private:
     bool registerView(QString identifier, QStringList typeHandled, viewCreator creator);
     bool registerNavigator(QString identifier, QStringList typeHandled, navigatorCreator creator);
     bool registerInteractor(QString identifier, QStringList typeHandled, interactorCreator creator);


    /** Singleton holder.*/
    static medViewFactory *s_instance;

    /**
     * @brief Templated method returning a pointer to an allocated object.
     * @see template<typename pointerT> registerToolBox
     * @warning keep it static if you don't want to freeze your brain
     * (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
     */
    template < typename T>
    static medAbstractView* _prvt_createView(QObject* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractViewNavigator* _prvt_createNavigator(medAbstractView* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractViewInteractor* _prvt_creatInteractor(medAbstractView* parent)
    {
        return (new T(parent));
    }

private:
    medViewFactoryPrivate* d;
};


