/*
 * medAnnotationFactory.cpp
 *
 *  Created on: 31 mai 2011
 *      Author: jstark
 */

#include "medAnnotationFactory.h"

#include "medAnnotationData.h"
#include "medAnnotationGraphicsObject.h"


class medAnnotationFactoryPrivate
{
public:
    struct AnnotationInfo {
        medAnnotationFactory::CreatorFunc m_func;
        QStringList m_dataNames;
    };
    typedef QHash< QString , AnnotationInfo > CreatorHashType;
    typedef QHash< QString , QString > DataNameToAnnotationType;

    CreatorHashType creators;
    DataNameToAnnotationType dataNameMap;
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

bool medAnnotationFactory::registerAnnotation( const QString &annotationName, const QStringList & dataNames, CreatorFunc func )
{
    if ( d->creators.contains( annotationName ) ) {
        return false;
    } 

    d->creators[annotationName].m_func = func;
    d->creators[annotationName].m_dataNames = dataNames;
    foreach( const QString name, dataNames ) {
        d->dataNameMap[name] = annotationName;
    }
    return true;
}

bool medAnnotationFactory::registerAnnotation( const QString &annotationName, const QString & dataName, CreatorFunc func )
{
    return registerAnnotation(annotationName, QStringList() << dataName, func );
}

std::auto_ptr<medAnnotationGraphicsObject> medAnnotationFactory::create( const QString &annotationName )
{
    std::auto_ptr<medAnnotationGraphicsObject> ret;
    medAnnotationFactoryPrivate::CreatorHashType::const_iterator it( d->creators.find(annotationName));
    if ( it != d->creators.end() ) {
        CreatorFunc func (it->m_func);
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

QString medAnnotationFactory::annotationForData( const QString & dataName )
{
    if (d->dataNameMap.contains( dataName ) )
        return d->dataNameMap.find(dataName).value();
    else 
        return QString();
}

std::auto_ptr<medAnnotationGraphicsObject> medAnnotationFactory::createAnnotationForData( const medAnnotationData * annData )
{
    std::auto_ptr<medAnnotationGraphicsObject> ret;
    if ( !annData ) {
        return ret;
    }
    ret = this->create(annotationForData(annData->description()));
    if ( !ret.get() )
        return ret;
    ret->setAnnotationData(annData);
    return ret;
}

