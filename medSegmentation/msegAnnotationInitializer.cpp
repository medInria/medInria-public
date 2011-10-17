/*
 * msegAnnotationInializer.cpp
 *
 *  Created on: 31 mai 2011 13:38:56
 *      Author: jstark
 */

#include "msegAnnotationInitializer.h"

#include "msegSeedPointAnnotation.h"
#include "msegSeedPointAnnotationData.h"

#include <medAnnotationFactory.h>

#include <dtkCore/dtkAbstractDataFactory.h>

namespace  {
    template< class T > 
        dtkAbstractData * dtkAbstractDataCreateFunc() { return new T; }
}

namespace mseg {

AnnotationInitializer::AnnotationInitializer()
{
    // TODO Auto-generated constructor stub

}

AnnotationInitializer::~AnnotationInitializer()
{
    // TODO Auto-generated destructor stub
}

bool AnnotationInitializer::initialize()
{
    bool ret = true;
    ret &= AnnotationInitializer::initializeAnnotationData();
    ret &= AnnotationInitializer::initializeAnnotations();
    return ret;
}

bool AnnotationInitializer::initializeAnnotations()
{
    medAnnotationFactory * factory = medAnnotationFactory::instance();

    bool ret = factory->registerAnnotation( SeedPointAnnotation::s_identifier(), 
        SeedPointAnnotationData::s_identifier(),
        "v3dView",
        SeedPointAnnotation::s_create );

    return ret;
}

bool AnnotationInitializer::initializeAnnotationData()
{
    dtkAbstractDataFactory * factory = dtkAbstractDataFactory::instance();
    bool ret = factory->registerDataType( SeedPointAnnotationData::s_identifier(), dtkAbstractDataCreateFunc<SeedPointAnnotationData> );
    return ret;
}




}

