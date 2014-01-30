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

#include <medAbstractLayeredView.h>
#include <medAbstractLayeredViewNavigator.h>
#include <medAbstractLayeredViewInteractor.h>




class medLayeredViewFactoryPrivate;
class MEDCORE_EXPORT medLayeredViewFactory : public dtkAbstractFactory
{
    Q_OBJECT
public:
    typedef medAbstractLayeredView *(*viewCreator)(QObject *parent);
    typedef medAbstractLayeredViewNavigator *(*navigatorCreator)(medAbstractLayeredView *parent);
    typedef medAbstractLayeredViewInteractor *(*interactorCreator)(medAbstractLayeredView *parent);

    static medLayeredViewFactory *instance();

    medAbstractLayeredView* createView(QString identifier, QObject *parent = 0);
    medAbstractLayeredViewNavigator* createNavigator(QString identifier, medAbstractLayeredView *parent);
    medAbstractLayeredViewInteractor*  createInteractor(QString identifier, medAbstractLayeredView *parent);


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
     medLayeredViewFactory();
    ~medLayeredViewFactory();

private:
     bool registerView(QString identifier, QStringList typeHandled, viewCreator creator);
     bool registerNavigator(QString identifier, QStringList typeHandled, navigatorCreator creator);
     bool registerInteractor(QString identifier, QStringList typeHandled, interactorCreator creator);


    /** Singleton holder.*/
    static medLayeredViewFactory *s_instance;

    /**
     * @brief Templated method returning a pointer to an allocated object.
     * @see template<typename pointerT> registerToolBox
     * @warning keep it static if you don't want to freeze your brain
     * (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
     */
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


