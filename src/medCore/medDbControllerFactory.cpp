#include "medDbControllerFactory.h"

#include "medAbstractDbController.h"

class medDbControllerFactoryPrivate
{
public:
    medDbControllerFactory::medDbControllerCreatorHash dbController_creators;
};

medDbControllerFactory * medDbControllerFactory::instance( void )
{
    if(!s_instance)
        s_instance = new medDbControllerFactory();

    return s_instance;
}

bool medDbControllerFactory::registerDbController( const QString& type, medDbControllerCreator func )
{
    if(!d->dbController_creators.contains(type)) {
        d->dbController_creators.insert(type, func);

        emit dbControllerRegistered(type);
        return true;
    }
    qWarning() << "failed registering" << type;

    return false;
}


medAbstractDbController * medDbControllerFactory::createDbController( QString type )
{
    // if we can't find the concrete controller we use the first we can get
    if(!d->dbController_creators.contains(type))
    {
        return NULL;

        // check if we have one at least
//        if (d->dbController_creators.isEmpty())
//            return NULL;
        // return the first element of the hash
//       return d->dbController_creators.begin().value()();
    }
        
    medAbstractDbController *controller = d->dbController_creators[type]();

    return controller;
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