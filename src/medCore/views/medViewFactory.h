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

class medAbstractView;
class medAbstractViewNavigator;
class medAbstractViewInteractor;

class medAbstractLayeredView;
class medAbstractLayeredViewNavigator;
class medAbstractLayeredViewInteractor;

class medAbstractImageView;
class medAbstractImageViewNavigator;
class medAbstractImageViewInteractor;

class medViewCreator;
class medInteractorCreator;
class medNavigatorCreator;


class medViewFactoryPrivate;

class MEDCORE_EXPORT medViewBoxFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef medAbstractView *(*medViewCreator)(QObject *parent);
    typedef medAbstractViewNavigator *(*medViewCreator)(medAbstractView *parent);
    typedef medAbstractViewInteractor *(*medViewCreator)(medAbstractView *parent);

    typedef medAbstractLayeredView *(*medViewCreator)(QObject *parent);
    typedef medAbstractLayeredViewNavigator *(*medViewCreator)(medAbstractLayeredView *parent);
    typedef medAbstractLayeredViewInteractor *(*medViewCreator)(medAbstractLayeredView *parent);

    typedef medAbstractImageView *(*medViewCreator)(QObject *parent);
    typedef medAbstractImageViewNavigator *(*medViewCreator)(medAbstractImageView *parent);
    typedef medAbstractImageViewInteractor *(*medViewCreator)(medAbstractImageView *parent);

public:
    static medViewBoxFactory *instance();

    template <typename objectType>


    bool registerView(QString& identifier,
                      medViewCreator creator);

    bool registerNavigator(QString& identifier,
                           QString& viewTypeHandled,
                           medNavigatorCreator creator);

    bool registerInteractor(QString& identifier,
                            QString& viewTypeHandled,
                            QString& dataTypeHandled, medInteractorCreator creator);

    template <typename objectType>
    bool registerView(QString& identifier)
    {
        //we must keep the templated part in the .h file for library users
        medViewCreator creator = create<viewType>;
        return registerView(identifier, creator);
    }

    template <typename objectType>
    bool registerNavigator(QString& identifier, QString& viewTypeHandle)
    {
        //we must keep the templated part in the .h file for library users
        medNavigatorCreator creator = create<navigatorType>;
        return registerNavigator(identifier, viewTypeHandle, creator);
    }

    template <typename objectType>
    bool registerInteractor(QString& identifier, QString& viewTypeHandle, QString& dataTypeHandle)
    {
        //we must keep the templated part in the .h file for library users
        medInteractorCreator creator = create<interactorType>;
        return registerInteractor(identifier, viewTypeHandle, dataTypeHandle, creator);
    }

    QList<QString> navigatorAbleToHandle(const QString& viewType) const;
    QList<QString> interactorAbleToHandle(const QString& viewType, const QString& dataType) const;

    QStringList viewHandledbyNavigator (QString& navigatorType) const;
    QStringList viewHandledbyInteractor (QString& navigatorType) const;
    QStringList dataHandledbyInteractor (QString& navigatorType) const;

public slots:
    medAbstractView *createView(QString& identifier, QObject *parent=0);
    medAbstractLayeredView *createLayeredView(QString& identifier, QObject *parent=0);
    medAbstractImageView *createImageView(QString& identifier, QObject *parent=0);

    medAbstractViewNavigator *createNavigator(QString& identifier, medAbstractView *parent=0);
    medAbstractLayeredViewNavigator *createNavigator(QString& identifier, medAbstractLayeredView *parent=0);
    medAbstractImageViewNavigator *createNavigator(QString& identifier, medAbstractImageView *parent=0);

    medAbstractViewInteractor *createInteractor(QString& identifier, medAbstractView *parent=0);
    medAbstractLayeredViewInteractor *createInteractor(QString& identifier, medAbstractLayeredView *parent=0);
    medAbstractImageViewInteractor *createInteractor(QString& identifier, medAbstractImageView *parent=0);


protected:
     medViewBoxFactory();
    ~medViewBoxFactory();

private:
    static medViewBoxFactory *s_instance; /** Singleton holder.*/
    /**
     * @brief Templated method returning a pointer to an allocated toolbox.
     * @see template<class toolboxType> registerToolBox
     * @warning keep it static if you don't want to freeze your brain
     * (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
     */
    template < typename T >
    static medAbstractView* create(QObject* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractLayeredView* create(QObject* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractImageView* create(QObject* parent)
    {
    return (new T(parent));
    }

    template < typename T >
    static medAbstractViewNavigator* create(medAbstractView* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractLayeredViewNavigator* create(medAbstractLayeredView* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractImageViewNavigator* create(medAbstractImageView* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractViewInteractor* create(medAbstractImageView* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractLayeredViewInteractor* create(medAbstractLayeredView* parent)
    {
        return (new T(parent));
    }

    template < typename T >
    static medAbstractImageViewInteractor* create(medAbstractImageView* parent)
    {
        return (new T(parent));
    }


private:
    medViewFactoryPrivate *d;
};


