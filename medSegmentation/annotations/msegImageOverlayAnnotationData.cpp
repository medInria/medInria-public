/*
 * msegImageOverlayAnnotationData.cpp
 *
 *  Created on: 31 mai 2011 10:36:54
 *      Author: jstark
 */

#include "msegImageOverlayAnnotationData.h"

namespace mseg {

ImageOverlayAnnotationData::ImageOverlayAnnotationData() :
    m_radiusScene(10)
{
    // TODO Auto-generated constructor stub

}

ImageOverlayAnnotationData::~ImageOverlayAnnotationData()
{
    // TODO Auto-generated destructor stub
}

void ImageOverlayAnnotationData::setRadiusScene( qreal val )
{
    m_radiusScene = val;
    emit dataModified();
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








}

