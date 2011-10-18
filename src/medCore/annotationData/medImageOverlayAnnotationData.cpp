/*
 * msegmedImageOverlayAnnotationData.cpp
 *
 *  Created on: 31 mai 2011 10:36:54
 *      Author: jstark
 */

#include "medImageOverlayAnnotationData.h"

medImageOverlayAnnotationData::medImageOverlayAnnotationData()
{
    // TODO Auto-generated constructor stub
}

medImageOverlayAnnotationData::~medImageOverlayAnnotationData()
{
    // TODO Auto-generated destructor stub
}

QString medImageOverlayAnnotationData::s_description()
{
    static const QString desc = "mseg::medImageOverlayAnnotationData";
    return desc;
}

QString medImageOverlayAnnotationData::description( void ) const
{
    return medImageOverlayAnnotationData::s_description();
}

QString medImageOverlayAnnotationData::s_identifier()
{
    static const QString desc = "mseg::medImageOverlayAnnotationData";
    return desc;
}

QString medImageOverlayAnnotationData::identifier( void ) const
{
    return medImageOverlayAnnotationData::s_identifier();
}


