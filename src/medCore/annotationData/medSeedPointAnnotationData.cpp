/*
 * msegSeedPointAnnotationData.cpp
 *
 *  Created on: 31 mai 2011 10:36:54
 *      Author: jstark
 */

#include "medSeedPointAnnotationData.h"

medSeedPointAnnotationData::medSeedPointAnnotationData() :
    m_selectedColor(Qt::red),
    m_color(Qt::cyan),
    m_radiusScene(10),
    m_isSelected(false)
{
    // TODO Auto-generated constructor stub

}

medSeedPointAnnotationData::~medSeedPointAnnotationData()
{
    // TODO Auto-generated destructor stub
}

void medSeedPointAnnotationData::setCenterWorld( QVector3D val )
{
    if ( m_centerWorld == val )
        return;
    m_centerWorld = val;
    emit dataModified(this);
}

void medSeedPointAnnotationData::setSelectedColor( QColor val )
{
    if ( m_selectedColor == val )
        return;
    m_selectedColor = val;
    emit dataModified(this);
}

void medSeedPointAnnotationData::setColor( QColor val )
{
    if ( m_color == val )
        return;
     m_color = val;
     emit dataModified(this);
}

void medSeedPointAnnotationData::setRadiusScene( qreal val )
{
    if ( m_radiusScene == val )
        return;
    m_radiusScene = val;
    emit dataModified(this);
}

QString medSeedPointAnnotationData::s_description()
{
    static const QString desc = "mseg::medSeedPointAnnotationData";
    return desc;
}

QString medSeedPointAnnotationData::description( void ) const 
{
    return medSeedPointAnnotationData::s_description();
}

QString medSeedPointAnnotationData::s_identifier()
{
    static const QString id = "mseg::medSeedPointAnnotationData";
    return id;
}

QString medSeedPointAnnotationData::identifier( void ) const 
{
    return medSeedPointAnnotationData::s_identifier();
}

void medSeedPointAnnotationData::setSelected( bool value )
{
    if ( m_isSelected != value ) {
        m_isSelected = value;
        emit dataModified(this);
    }
}

