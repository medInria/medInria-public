/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDbControllerFactory.h>

#include <medAbstractDbController.h>

class medDbControllerFactoryPrivate
{
public:
    medDbControllerFactory::medDbControllerCreatorHash dbControllerCreators;
};

medDbControllerFactory * medDbControllerFactory::instance( void )
{
    if(!s_instance)
        s_instance = new medDbControllerFactory();

    return s_instance;
}

bool medDbControllerFactory::registerDbController( const QString& type, medDbControllerCreator func )
{
    if(!d->dbControllerCreators.contains(type)) {
        d->dbControllerCreators.insert(type, func);
        //qDebug() << "registering " << type;
        emit dbControllerRegistered(type);
        return true;
    }
    qWarning() << "failed registering" << type;

    return false;
}


medAbstractDbController * medDbControllerFactory::createDbController( QString type )
{
    // if we can't find the concrete controller we use the first we can get
    if(!d->dbControllerCreators.contains(type))
    {
        return NULL;

        // check if we have one at least
//        if (d->dbControllerCreators.isEmpty())
//            return NULL;
        // return the first element of the hash
//       return d->dbControllerCreators.begin().value()();
    }

    medAbstractDbController *controller = d->dbControllerCreators[type]();

    return controller;
}

QList <QString> medDbControllerFactory::controllers()
{
    return d->dbControllerCreators.keys();
}


medDbControllerFactory::medDbControllerFactory( void ): dtkAbstractFactory(), d(new medDbControllerFactoryPrivate)
{

}

medDbControllerFactory::~medDbControllerFactory( void )
{
    delete d;
    d = NULL;
}

medDbControllerFactory *medDbControllerFactory::s_instance = NULL;


