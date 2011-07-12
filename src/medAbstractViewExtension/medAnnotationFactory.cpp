/*
 * medAnnotationFactory.cpp
 *
 *  Created on: 31 mai 2011
 *      Author: jstark
 */

#include "medAnnotationFactory.h"

#include "medAnnotationGraphicsObject.h"


class medAnnotationFactoryPrivate
{
public:
    typedef QHash< QString , medAnnotationFactory::CreatorFunc > CreatorHashType;

    CreatorHashType creators;
    static medAnnotationFactory* instance;

};

medAnnotationFactory* medAnnotationFactoryPrivate::instance = NULL;


medAnnotationFactory::medAnnotationFactory() 
    : d( new medAnnotationFactoryPrivate )
{
}

medAnnotationFactory::~medAnnotationFactory() 
{
    delete d;
    d = NULL;
}

bool medAnnotationFactory::registerAnnotation( const QString &annotationName, CreatorFunc func )
{
    if ( d->creators.contains( annotationName ) ) {
        return false;
    } 

    d->creators[annotationName] = func;
    return true;
}

std::auto_ptr<medAnnotationGraphicsObject> medAnnotationFactory::create( const QString &annotationName )
{
    std::auto_ptr<medAnnotationGraphicsObject> ret;
    medAnnotationFactoryPrivate::CreatorHashType::const_iterator it( d->creators.find(annotationName));
    if ( it != d->creators.end() ) {
        CreatorFunc func (*it);
        ret.reset(func());
    }
    return ret;
}

// static
medAnnotationFactory* medAnnotationFactory::instance()
{
    if ( !medAnnotationFactoryPrivate::instance ) 
        medAnnotationFactoryPrivate::instance = new medAnnotationFactory();

    return medAnnotationFactoryPrivate::instance;
}

// static
void medAnnotationFactory::destroy()
{
    delete medAnnotationFactoryPrivate::instance;
    medAnnotationFactoryPrivate::instance = NULL;
}
