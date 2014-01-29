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


class MEDCORE_EXPORT medViewFactory : public dtkAbstractFactory
{
    Q_OBJECT
public:
    static medViewFactory *instance();

    template <typename pointerT>
    pointerT create(QString& implementationOf, QString& identifier, QObject *parent=0)
    {
        pointerT pointer = NULL;
        if(implementationOf == medAbstractView::implementationOf())
        {
            medAbstractViewCreator c = NULL;
            c = _prvt_creators1.value(identifier);
            if(c)
                pointer = (c)(parent);
        }
        if(implementationOf == medAbstractLayeredView::implementationOf())
        {
            medAbstractLayeredViewCreator c = NULL;
            c = _prvt_creators2.value(identifier);
            if(c)
                pointer = (c)(parent);
        }
        if(implementationOf == medAbstractImageView::implementationOf())
        {
            medAbstractImageViewCreator c = NULL;
            c = _prvt_creators3.value(identifier);
            if(c)
                pointer = (c)(parent);
        }

        return pointer;
    }

    template <typename pointerT>
    pointerT create(QString& implementationOf, QString& identifier, medAbstractView *parent=0)
    {

        pointerT pointer = NULL;
        if(implementationOf == medAbstractNavigator::implementationOf())
        {
            medAbstractNavigatorCreator c = NULL;
            c = _prvt_creators4.value(identifier);
            if(c)
                pointer = (c)(parent);
        }
        if(implementationOf == medAbstractViewNavigator::implementationOf())
        {
            medAbstractViewNavigatorCreator c = NULL;
            c = _prvt_creators5.value(identifier);
            if(c)
                pointer = (c)(parent);
        }
        if(implementationOf == medAbstractInteractor::implementationOf())
        {
            medAbstractInteractorCreator c = NULL;
            c = _prvt_creators8.value(identifier);
            if(c)
                pointer = (_prvt_creators8.value(identifier))(parent);
        }
        if(implementationOf == medAbstractViewInteractor::implementationOf())
        {
            medAbstractViewInteractorCreator c = NULL;
            c = _prvt_creators9.value(identifier);
            if(c)
                pointer = (c)(parent);
        }

        return pointer;
    }

    template <typename pointerT>
    pointerT create(QString& implementationOf, QString& identifier, medAbstractLayeredView *parent=0)
    {
        pointerT pointer = NULL;
        if(implementationOf == medAbstractLayeredViewNavigator::implementationOf())
        {
            medAbstractLayeredViewNavigatorCreator c = NULL;
            c = _prvt_creators6.value(identifier);
            if(c)
                pointer = (c)(parent);
        }
        if(implementationOf == medAbstractLayeredViewInteractor::implementationOf())
        {
            medAbstractLayeredViewInteractorCreator c = NULL;
            c = _prvt_creators10.value(identifier);
            if(c)
                pointer = (c)(parent);
        }

        return pointer;
    }

    template <typename pointerT>
    pointerT create(QString& implementationOf, QString& identifier, medAbstractImageView *parent=0)
    {
        pointerT pointer = NULL;
        if(implementationOf == medAbstractImageViewNavigator::implementationOf())
        {
            medAbstractImageViewNavigatorCreator c = NULL;
            c = _prvt_creators7.value(identifier);
            if(c)
                pointer = (c)(parent);
        }
        if(implementationOf == medAbstractImageViewInteractor::implementationOf())
        {
            medAbstractImageViewInteractorCreator c = NULL;
            c = _prvt_creators11.value(identifier);
            if(c)
                pointer = (c)(parent);
        }

        return pointer;
    }

    template <typename T>
    bool registered(QString& implementationOf,
                            QString& identifier,
                            QStringList& handled)
    {
        //we must keep the templated part in the .h file for library users
        //--------------------------------------------------------------------------
        //  register views
        if(implementationOf == medAbstractView::implementationOf())
        {
            medAbstractViewCreator creator = _prvt_create<medAbstractView*,
                    T,
                    medAbstractView*>;
            return registered(implementationOf, identifier, handled, creator);
        }
        else if(implementationOf == medAbstractLayeredView::implementationOf())
        {
            medAbstractViewCreator creator = _prvt_create<medAbstractLayeredView*,
                    T,
                    medAbstractLayeredView*>;
            return registered(implementationOf, identifier, handled, creator);
        }
        else if(implementationOf == medAbstractImageView::implementationOf())
        {
            medAbstractImageViewCreator creator = _prvt_create<medAbstractImageView*,
                    T,
                    medAbstractImageView*>;
            return registered(implementationOf, identifier, handled, creator);
        }
        //--------------------------------------------------------------------------
        //  register navigators
        else if(implementationOf == medAbstractNavigator::implementationOf())
        {
            medAbstractNavigatorCreator creator = _prvt_create<medAbstractNavigator*,
                    T,
                    medAbstractView*>;
            return registered(implementationOf, identifier, handled, creator);
        }
        else if(implementationOf == medAbstractViewNavigator::implementationOf())
        {
            medAbstractViewNavigatorCreator creator = _prvt_create<medAbstractViewNavigator*,
                    T,
                    medAbstractView*>;
            return registered(implementationOf, identifier, handled, creator);
        }
        else if(implementationOf == medAbstractLayeredViewNavigator::implementationOf())
        {
            medAbstractViewNavigatorCreator creator = _prvt_create<medAbstractLayeredViewNavigator*,
                    T,
                    medAbstractLayeredView*>;
            return registered(implementationOf, identifier, handled, creator);
        }
        else if(implementationOf == medAbstractImageViewNavigator::implementationOf())
        {
            medAbstractImageViewNavigatorCreator creator = _prvt_create<medAbstractImageViewNavigator*,
                    T,
                    medAbstractImageView*>;
            return registered(implementationOf, identifier, handled, creator);
        }
        //--------------------------------------------------------------------------
        //  register interactors
        else if(implementationOf == medAbstractInteractor::implementationOf())
        {
            medAbstractInteractorCreator creator = _prvt_create<medAbstractInteractor*,
                    T,
                    QObject*>;
            return registered(implementationOf, identifier, handled, creator);
        }
        else if(implementationOf == medAbstractViewInteractor::implementationOf())
        {
            medAbstractViewInteractorCreator creator = _prvt_create<medAbstractViewInteractor*,
                    T,
                    medAbstractView*>;
            return registered(implementationOf, identifier, handled, creator);
        }
        else if(implementationOf == medAbstractLayeredViewInteractor::implementationOf())
        {
            medAbstractViewInteractorCreator creator = _prvt_create<medAbstractLayeredViewInteractor*,
                    T,
                    medAbstractLayeredView*>;
            return registered(implementationOf, identifier, handled, creator);
        }
        else if(implementationOf == medAbstractImageViewInteractor::implementationOf())
        {
            medAbstractImageViewInteractorCreator creator = _prvt_create<medAbstractImageViewInteractor*,
                    T,
                    medAbstractImageView*>;
            return registered(implementationOf, identifier, handled, creator);
        }
        else
            return false;
    }

    QList<QString> navigatorsAbleToHandle(const QString& viewType) const;
    QList<QString> interactorsAbleToHandle(const QString& viewType, const QString& dataType) const;


protected:
     medViewFactory(){}
    ~medViewFactory(){}

private:

     bool registered(QString& implementationOf,
                     QString& identifier,
                     QStringList& typeHandled,
                     medAbstractViewCreator creator);
     bool registered(QString& implementationOf,
                     QString& identifier,
                     QStringList& typeHandled,
                     medAbstractLayeredViewCreator creator);
     bool registered(QString& implementationOf,
                     QString& identifier,
                     QStringList& typeHandled,
                     medAbstractImageViewCreator creator);
     bool registered(QString& implementationOf,
                     QString& identifier,
                     QStringList& typeHandled,
                     medAbstractNavigatorCreator creator);
     bool registered(QString& implementationOf,
                     QString& identifier,
                     QStringList& typeHandled,
                     medAbstractViewNavigatorCreator creator);
     bool registered(QString& implementationOf,
                     QString& identifier,
                     QStringList& typeHandled,
                     medAbstractLayeredViewNavigatorCreator creator);
     bool registered(QString& implementationOf,
                     QString& identifier,
                     QStringList& typeHandled,
                     medAbstractImageViewNavigatorCreator creator);
     bool registered(QString& implementationOf,
                     QString& identifier,
                     QStringList& typeHandled,
                     medAbstractInteractorCreator creator);
     bool registered(QString& implementationOf,
                     QString& identifier,
                     QStringList& typeHandled,
                     medAbstractViewInteractorCreator creator);
     bool registered(QString& implementationOf,
                     QString& identifier,
                     QStringList& typeHandled,
                     medAbstractLayeredViewInteractorCreator creator);
     bool registered(QString& implementationOf,
                     QString& identifier,
                     QStringList& typeHandled,
                     medAbstractImageViewInteractorCreator creator);

    /** Singleton holder.*/
    static medViewFactory *s_instance;

    /**
     * @brief Templated method returning a pointer to an allocated object.
     * @see template<typename pointerT> registerToolBox
     * @warning keep it static if you don't want to freeze your brain
     * (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
     */
    template < typename pointerT, typename instanceT, typename parameterT >
    static pointerT _prvt_create(parameterT parent)
    {
        return (new instanceT(parent));
    }

private:
    typedef QHash <QString, medAbstractViewCreator>                     medIdentifierCreatorPair1;
    typedef QHash <QString, medAbstractLayeredViewCreator>              medIdentifierCreatorPair2;
    typedef QHash <QString, medAbstractImageViewCreator>                medIdentifierCreatorPair3;

    typedef QHash <QString, medAbstractNavigatorCreator>                medIdentifierCreatorPair4;
    typedef QHash <QString, medAbstractViewNavigatorCreator>            medIdentifierCreatorPair5;
    typedef QHash <QString, medAbstractLayeredViewNavigatorCreator>     medIdentifierCreatorPair6;
    typedef QHash <QString, medAbstractImageViewNavigatorCreator>       medIdentifierCreatorPair7;

    typedef QHash <QString, medAbstractInteractorCreator>               medIdentifierCreatorPair8;
    typedef QHash <QString, medAbstractViewInteractorCreator>           medIdentifierCreatorPair9;
    typedef QHash <QString, medAbstractLayeredViewInteractorCreator>    medIdentifierCreatorPair10;
    typedef QHash <QString, medAbstractImageViewInteractorCreator>      medIdentifierCreatorPair11;

    medIdentifierCreatorPair1   _prvt_creators1;
    medIdentifierCreatorPair2   _prvt_creators2;
    medIdentifierCreatorPair3   _prvt_creators3;

    medIdentifierCreatorPair4   _prvt_creators4;
    medIdentifierCreatorPair5   _prvt_creators5;
    medIdentifierCreatorPair6   _prvt_creators6;
    medIdentifierCreatorPair7   _prvt_creators7;

    medIdentifierCreatorPair8   _prvt_creators8;
    medIdentifierCreatorPair9   _prvt_creators9;
    medIdentifierCreatorPair10  _prvt_creators10;
    medIdentifierCreatorPair11  _prvt_creators11;


    typedef QHash <QString, QStringList>                                identifierHandledTypeHash;
    typedef QHash <QString, QString>                                    identifierImplementOfHash;

    identifierHandledTypeHash               _prvt_identifierHash;
    identifierImplementOfHash               _prvt_implementOfHash;
};


