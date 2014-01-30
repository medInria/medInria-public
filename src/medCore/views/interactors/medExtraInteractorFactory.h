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

#include <medAbstractExtraInteractor.h>



class medExtraInteractorFactoryPrivate;
class MEDCORE_EXPORT medExtraInteractorFactory : public dtkAbstractFactory
{
    Q_OBJECT
public:
    typedef medAbstractExtraInteractor *(*interactorCreator)(medAbstractView *parent);


    static medExtraInteractorFactory *instance();


    medAbstractExtraInteractor*  createInteractor(QString identifier, medAbstractView *parent);


    template <typename T>
    bool registerInteractor(QString identifier, QStringList typeHandled)
    {
        interactorCreator creator = _prvt_createInteractor<T>;
        return registerInteractor(identifier, typeHandled, creator);
    }

    QStringList interactorsAbleToHandle(const QString viewType, const QString dataType) const;

protected:
     medExtraInteractorFactory();
    ~medExtraInteractorFactory();

private:
     bool registerInteractor(QString identifier, QStringList typeHandled, interactorCreator creator);


    /** Singleton holder.*/
    static medExtraInteractorFactory *s_instance;

    /**
     * @brief Templated method returning a pointer to an allocated object.
     * @see template<typename pointerT> registerToolBox
     * @warning keep it static if you don't want to freeze your brain
     * (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
     */

    template < typename T>
    static medAbstractExtraInteractor* _prvt_createInteractor(medAbstractView* parent)
    {
        return (new T(parent));
    }

private:
    medExtraInteractorFactoryPrivate* d;
};


