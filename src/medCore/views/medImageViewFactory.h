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

#include "medCoreExport.h"

#include <QStringList>

#include <medAbstractImageView.h>
#include <medAbstractImageViewNavigator.h>
#include <medAbstractImageViewInteractor.h>


class medImageViewFactoryPrivate;
class MEDCORE_EXPORT medImageViewFactory : public dtkAbstractFactory
{
    Q_OBJECT


public:
    static medImageViewFactory *instance();

    typedef medAbstractImageView *(*viewCreator)(QObject *parent);
    typedef medAbstractImageViewNavigator *(*navigatorCreator)(medAbstractImageView *parent);
    typedef medAbstractImageViewInteractor *(*interactorCreator)(medAbstractImageView *parent);

    medAbstractImageView* createView(QString identifier, QObject *parent = 0);
    medAbstractImageViewNavigator* createNavigator(QString identifier, medAbstractImageView *parent);
    medAbstractImageViewInteractor*  createInteractor(QString identifier, medAbstractImageView *parent);


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
     medImageViewFactory();
    ~medImageViewFactory();

private:
     bool registerView(QString identifier, QStringList typeHandled, viewCreator creator);
     bool registerNavigator(QString identifier, QStringList typeHandled, navigatorCreator creator);
     bool registerInteractor(QString identifier, QStringList typeHandled, interactorCreator creator);


    /** Singleton holder.*/
    static medImageViewFactory *s_instance;

    /**
     * @brief Templated method returning a pointer to an allocated object.
     * @see template<typename pointerT> registerToolBox
     * @warning keep it static if you don't want to freeze your brain
     * (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
     */
    template < typename T>
    static medAbstractImageView* _prvt_createView(QObject* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractImageViewNavigator* _prvt_createNavigator(medAbstractImageView* parent)
    {
        return (new T(parent));
    }
    template < typename T>
    static medAbstractImageViewInteractor* _prvt_creatInteractor(medAbstractImageView* parent)
    {
        return (new T(parent));
    }

private:
    medImageViewFactoryPrivate* d;
};


