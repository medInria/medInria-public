/*
 * msegSeedPointAnnotationData.cpp
 *
 *  Created on: 31 mai 2011 10:36:54
 *      Author: jstark
 */

#include "msegSeedPointAnnotationData.h"

namespace mseg {

SeedPointAnnotationData::SeedPointAnnotationData() :
    m_radiusScene(10)
{
    // TODO Auto-generated constructor stub

}

SeedPointAnnotationData::~SeedPointAnnotationData()
{
    // TODO Auto-generated destructor stub
}

void SeedPointAnnotationData::setCenterWorld( QVector3D val )
{
    m_centerWorld = val;
    emit dataModified();
}

void SeedPointAnnotationData::setColor( QColor val )
{
     m_color = val;
     emit dataModified();
}

void SeedPointAnnotationData::setRadiusScene( qreal val )
{
    m_radiusScene = val;
    emit dataModified();
}

QString SeedPointAnnotationData::s_description()
{
    static const QString desc = "mseg::SeedPointAnnotationData";
    return desc;
}







}

