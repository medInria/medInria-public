/*
 * msegSeedPointAnnotationData.cpp
 *
 *  Created on: 31 mai 2011 10:36:54
 *      Author: jstark
 */

#include "msegSeedPointAnnotationData.h"

namespace mseg {

SeedPointAnnotationData::SeedPointAnnotationData() :
    m_selectedColor(Qt::red),
    m_color(Qt::cyan),
    m_radiusScene(10),
    m_isSelected(false)
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

void SeedPointAnnotationData::setSelectedColor( QColor val )
{
    if ( m_selectedColor == val )
        return;
    m_selectedColor = val;
    emit dataModified();
}

void SeedPointAnnotationData::setColor( QColor val )
{
    if ( m_color == val )
        return;
     m_color = val;
     emit dataModified();
}

void SeedPointAnnotationData::setRadiusScene( qreal val )
{
    if ( m_radiusScene == val )
        return;
    m_radiusScene = val;
    emit dataModified();
}

QString SeedPointAnnotationData::s_description()
{
    static const QString desc = "mseg::SeedPointAnnotationData";
    return desc;
}

QString SeedPointAnnotationData::description( void ) const 
{
    return SeedPointAnnotationData::s_description();
}

void SeedPointAnnotationData::setSelected( bool value )
{
    if ( m_isSelected != value ) {
        m_isSelected = value;
        emit dataModified();
    }
}









}

