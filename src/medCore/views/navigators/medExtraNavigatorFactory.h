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

#include <medAbstractExtraNavigator.h>



class medExtraNavigatorFactoryPrivate;
class MEDCORE_EXPORT medExtraNavigatorFactory : public dtkAbstractFactory
{
    Q_OBJECT
public:

    typedef medAbstractExtraNavigator *(*navigatorCreator)(medAbstractView *parent);

    static medExtraNavigatorFactory *instance();


    medAbstractExtraNavigator*  createNavigator(QString identifier, medAbstractView *parent);


    template <typename T>
    bool registerNavigator(QString identifier, QStringList typeHandled)
    {
        navigatorCreator creator = _prvt_creatNavigator<T>;
        return registerNavigator(identifier, typeHandled, creator);
    }

    QStringList NavigatorsAbleToHandle(const QString viewType, const QString dataType) const;

protected:
     medExtraNavigatorFactory();
    ~medExtraNavigatorFactory();

private:
     bool registerNavigator(QString identifier, QStringList typeHandled, navigatorCreator creator);


    /** Singleton holder.*/
    static medExtraNavigatorFactory *s_instance;

    /**
     * @brief Templated method returning a pointer to an allocated object.
     * @see template<typename pointerT> registerToolBox
     * @warning keep it static if you don't want to freeze your brain
     * (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
     */

    template < typename T>
    static medAbstractExtraNavigator* _prvt_creatNavigator(medAbstractView* parent)
    {
        return (new T(parent));
    }

private:
    medExtraNavigatorFactoryPrivate* d;
};


