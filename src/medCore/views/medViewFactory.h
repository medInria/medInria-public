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

#include <medAbstractView.h>
#include <medAbstractViewNavigator.h>
#include <medAbstractViewInteractor.h>

#include <medAbstractLayeredView.h>
#include <medAbstractLayeredViewNavigator.h>
#include <medAbstractLayeredViewInteractor.h>

#include <medAbstractImageView.h>
#include <medAbstractImageViewNavigator.h>
#include <medAbstractImageViewInteractor.h>


typedef medAbstractView *(*medAbstractViewCreator)(QObject *parent);
typedef medAbstractNavigator *(*medAbstractNavigatorCreator)(medAbstractView *parent);
typedef medAbstractInteractor *(*medAbstractInteractorCreator)(medAbstractView *parent);
typedef medAbstractViewNavigator *(*medAbstractViewNavigatorCreator)(medAbstractView *parent);
typedef medAbstractViewInteractor *(*medAbstractViewInteractorCreator)(medAbstractView *parent);

typedef medAbstractLayeredView *(*medAbstractLayeredViewCreator)(QObject *parent);
typedef medAbstractLayeredViewNavigator *(*medAbstractLayeredViewNavigatorCreator)(medAbstractLayeredView *parent);
typedef medAbstractLayeredViewInteractor *(*medAbstractLayeredViewInteractorCreator)(medAbstractLayeredView *parent);

typedef medAbstractImageView *(*medAbstractImageViewCreator)(QObject *parent);
typedef medAbstractImageViewNavigator *(*medAbstractImageViewNavigatorCreator)(medAbstractImageView *parent);
typedef medAbstractImageViewInteractor *(*medAbstractImageViewInteractorCreator)(medAbstractImageView *parent);


class medViewFactoryPrivate;
class MEDCORE_EXPORT medViewFactory : public dtkAbstractFactory
{
    Q_OBJECT
public:
    static medViewFactory *instance();

    template <typename T>
    bool registerView(QString& implementationOf, QString& identifier)
    {
        //we must keep the templated part in the .h file for library users
        if(implementationOf == medAbstractImageView::implementationOf())
        {
            medAbstractViewCreator creator = abstractView_c<T>;
            return registerView(identifier, creator);
        }
        else if(implementationOf == medAbstractLayeredView::implementationOf())
        {
            medAbstractLayeredViewCreator creator = abstractLayerdView_c<T>;
            return registerView(identifier, creator);
        }
        else if(implementationOf == medAbstractImageView::implementationOf())
        {
            medAbstractImageViewCreator creator = abstractImageView_c<T>;
            return registerView(identifier, creator);
        }
        else
            return false;
    }

    template <typename T>
    bool registerNavigator(QString& implementationOf,
                           QString& identifier,
                           QString& viewTypeHandle)
    {
        //we must keep the templated part in the .h file for library users
        if(implementationOf == medAbstractViewNavigator::implementationOf())
        {
            medAbstractNavigatorCreator creator = abstractViewNavigator_c<T>;
            return registerNavigator(identifier, viewTypeHandle, creator);
        }
        else if(implementationOf == medAbstractLayeredViewNavigator::implementationOf())
        {
            medAbstractViewNavigatorCreator creator = abstractLayeredViewNavigator_c<T>;
            return registerNavigator(identifier, viewTypeHandle, creator);
        }
        else if(implementationOf == medAbstractImageViewNavigator::implementationOf())
        {
            medAbstractLayeredViewNavigatorCreator creator = abstractImageViewNavigator_c<T>;
            return registerNavigator(identifier, viewTypeHandle, creator);
        }
        else if(implementationOf == medAbstractImageViewNavigator::implementationOf())
        {
            medAbstractImageViewNavigatorCreator creator = abstractNavigator_c<T>;
            return registerNavigator(identifier, viewTypeHandle, creator);
        }
        else
            return false;
    }

    template <typename T>
    bool registerInteractor(QString& implementationOf,
                            QString& identifier,
                            QString& viewTypeHandle,
                            QString& dataTypeHandle)
    {
        //we must keep the templated part in the .h file for library users
        if(implementationOf == medAbstractInteractor::implementationOf())
        {
            medAbstractInteractorCreator creator = abstractViewInteractor_c<T>;
            return registerInteractor(identifier, viewTypeHandle, dataTypeHandle, creator);
        }
        else if(implementationOf == medAbstractLayeredViewInteractor::implementationOf())
        {
            medAbstractViewInteractorCreator creator = abstractLayeredViewInteractor_c<T>;
            return registerInteractor(identifier, viewTypeHandle, dataTypeHandle, creator);
        }
        else if(implementationOf == medAbstractImageViewInteractor::implementationOf())
        {
            medAbstractLayeredViewInteractorCreator creator = abstractImageViewInteractor_c<T>;
            return registerInteractor(identifier, viewTypeHandle, dataTypeHandle, creator);
        }
        else if(implementationOf == medAbstractImageViewInteractor::implementationOf())
        {
            medAbstractImageViewInteractorCreator creator = abstractInteractor_c<T>;
            return registerInteractor(identifier, viewTypeHandle, dataTypeHandle, creator);
        }
        else
            return false;
    }

    QList<QString> navigatorsAbleToHandle(const QString& viewType) const;
    QList<QString> interactorsAbleToHandle(const QString& viewType, const QString& dataType) const;

public slots:
    medAbstractView *createAbstractView(QString& identifier, QObject *parent=0);
    medAbstractLayeredView *createAbstractLayeredView(QString& identifier, QObject *parent=0);
    medAbstractImageView *createAbstractImageView(QString& identifier, QObject *parent=0);

    medAbstractNavigator *createAbstractNavigator(QString& identifier, medAbstractView *parent=0);
    medAbstractViewNavigator *createAbstractViewNavigator(QString& identifier, medAbstractView *parent=0);
    medAbstractLayeredViewNavigator *createAbstractLayeredViewNavigator(QString& identifier, medAbstractLayeredView *parent=0);
    medAbstractImageViewNavigator *createAbstractImageViewNavigator(QString& identifier, medAbstractImageView *parent=0);

    medAbstractInteractor *createAbstractInteractor(QString& identifier, medAbstractView *parent=0);
    medAbstractViewInteractor* createAbstractViewInteractor(QString& identifier, medAbstractView *parent=0);
    medAbstractLayeredViewInteractor* createAbstractLayeredViewInteractor(QString& identifier, medAbstractLayeredView *parent=0);
    medAbstractImageViewInteractor* createAbstractImageViewInteractor(QString& identifier, medAbstractImageView *parent=0);


protected:
     medViewFactory();
    ~medViewFactory();

private:

     bool registerView(QString& identifier,
                       medAbstractViewCreator creator);
     bool registerView(QString& identifier,
                       medAbstractLayeredViewCreator creator);
     bool registerView(QString& identifier,
                       medAbstractImageViewCreator creator);

     bool registerNavigator(QString& identifier,
                            QString& viewTypeHandled,
                            medAbstractNavigatorCreator creator);
     bool registerNavigator(QString& identifier,
                            QString& viewTypeHandled,
                            medAbstractViewNavigatorCreator creator);
     bool registerNavigator(QString& identifier,
                            QString& viewTypeHandled,
                            medAbstractLayeredViewNavigatorCreator creator);
     bool registerNavigator(QString& identifier,
                            QString& viewTypeHandled,
                            medAbstractImageViewNavigatorCreator creator);

     bool registerInteractor(QString& identifier,
                             QString& viewTypeHandled,
                             QString& dataTypeHandled,
                             medAbstractInteractorCreator creator);
     bool registerInteractor(QString& identifier,
                             QString& viewTypeHandled,
                             QString& dataTypeHandled,
                             medAbstractViewInteractorCreator creator);
     bool registerInteractor(QString& identifier,
                             QString& viewTypeHandled,
                             QString& dataTypeHandled,
                             medAbstractLayeredViewInteractorCreator creator);
     bool registerInteractor(QString& identifier,
                             QString& viewTypeHandled,
                             QString& dataTypeHandled,
                             medAbstractImageViewInteractorCreator creator);


    static medViewFactory *s_instance; /** Singleton holder.*/
    /**
     * @brief Templated method returning a pointer to an allocated toolbox.
     * @see template<class toolboxType> registerToolBox
     * @warning keep it static if you don't want to freeze your brain
     * (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
     */
    template < typename T >
    static medAbstractView* abstractView_c(QObject* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractLayeredView* abstractLayerdView_c(QObject* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractImageView* abstractImageView_c(QObject* parent)
    {
    return (new T(parent));
    }

    template < typename T >
    static medAbstractViewNavigator* abstractNavigator_c(medAbstractView* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractLayeredViewNavigator* abstractViewNavigator_c(medAbstractLayeredView* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractImageViewNavigator* abstractLayeredViewNavigator_c(medAbstractImageView* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractImageViewNavigator* abstractImageViewNavigator_c(medAbstractImageView* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractViewNavigator* abstractInteractor_c(medAbstractView* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractLayeredViewNavigator* abstractViewInteractor_c(medAbstractLayeredView* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractImageViewNavigator* abstractLayeredViewInteractor_c(medAbstractImageView* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractImageViewNavigator* abstractImageViewInteractor_c(medAbstractImageView* parent)
    {
        return (new T(parent));
    }

private:
    medViewFactoryPrivate *d;
};


