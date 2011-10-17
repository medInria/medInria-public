/*
 * medAnnotationFactory.cpp
 *
 *  Created on: 31 mai 2011
 *      Author: jstark
 */

#include "medAnnotationFactory.h"

#include "medAnnotationData.h"
#include "medAbstractAnnotationRepresentation.h"


class medAnnotationFactoryPrivate
{
public:
    struct AnnotationInfo {
        medAnnotationFactory::CreatorFunc m_func;
        QStringList m_dataNames;
        QStringList m_viewNames;
    };
    typedef QHash< QString , AnnotationInfo > CreatorHashType;
    typedef QHash< QString , QStringList > DataNameToAnnotationType;

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

bool medAnnotationFactory::registerAnnotation( const QString &annotationName, const QStringList & dataNames, const QStringList & viewNames, CreatorFunc func )
{
    if ( d->creators.contains( annotationName ) ) {
        return false;
    } 

    d->creators[annotationName].m_func = func;
    d->creators[annotationName].m_dataNames = dataNames;
    d->creators[annotationName].m_viewNames = viewNames;
    foreach( const QString name, dataNames ) {
        d->dataNameMap[name] << annotationName;
    }
    return true;
}

bool medAnnotationFactory::registerAnnotation( const QString &annotationName, const QString & dataName, const QString & viewName, CreatorFunc func )
{
    return registerAnnotation(annotationName, 
        QStringList() << dataName, 
        QStringList() << viewName, func );
}

QObject * medAnnotationFactory::create( const QString &annotationName )
{
    QObject * ret;
    medAnnotationFactoryPrivate::CreatorHashType::const_iterator it( d->creators.find(annotationName));
    if ( it != d->creators.end() ) {
        CreatorFunc func (it->m_func);
        ret = func();
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

QString medAnnotationFactory::annotationForData( const QString & dataName, const QString & viewName )
{
    QString ret;
    if (d->dataNameMap.contains( dataName ) ) {

        foreach ( const QString & annotationName, d->dataNameMap.find(dataName).value() ) 
        {
            if ( d->creators.find(annotationName)->m_viewNames.contains(viewName) ) 
            {
                ret = annotationName;
                break;
            }
        }
    }
    return ret;
}

QObject * medAnnotationFactory::createAnnotationForData( const medAnnotationData * annData, const QString & viewName )
{
    QObject * ret = NULL;
    if ( !annData ) {
        return ret;
    }
    ret = this->create(annotationForData(annData->description(), viewName));
    //if ( ret.isNull() )
    //    return ret;
//    ret->setAnnotationData(annData);
    return ret;
}

