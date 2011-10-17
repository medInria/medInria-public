/*
 * msegImageOverlayAnnotationData.cpp
 *
 *  Created on: 31 mai 2011 10:36:54
 *      Author: jstark
 */

#include "msegImageOverlayAnnotationData.h"

namespace mseg {

ImageOverlayAnnotationData::ImageOverlayAnnotationData()
{
    // TODO Auto-generated constructor stub
}

ImageOverlayAnnotationData::~ImageOverlayAnnotationData()
{
    // TODO Auto-generated destructor stub
}

QString ImageOverlayAnnotationData::s_description()
{
    static const QString desc = "mseg::ImageOverlayAnnotationData";
    return desc;
}

QString ImageOverlayAnnotationData::description( void ) const
{
    return ImageOverlayAnnotationData::s_description();
}

QString ImageOverlayAnnotationData::s_identifier()
{
    static const QString desc = "mseg::ImageOverlayAnnotationData";
    return desc;
}

QString ImageOverlayAnnotationData::identifier( void ) const
{
    return ImageOverlayAnnotationData::s_identifier();
}










}

