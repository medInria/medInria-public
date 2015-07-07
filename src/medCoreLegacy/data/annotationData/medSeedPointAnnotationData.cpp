/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSeedPointAnnotationData.h>

medSeedPointAnnotationData::medSeedPointAnnotationData() :
    m_color(Qt::cyan),
    m_selectedColor(Qt::red),
    m_radiusScene(10),
    m_selectedSeed(-1)
{
}

medSeedPointAnnotationData::~medSeedPointAnnotationData()
{
}

void medSeedPointAnnotationData::setCenterWorld( int num, const QVector3D & val )
{
    if ( num >= m_centerWorld.size() ) {
        this->setNumberOfSeeds(num+1);
    } else if ( m_centerWorld[num] == val )
        return;
    m_centerWorld[num] = val;
    emit dataModified(this);
}

void medSeedPointAnnotationData::setSelectedColor( const QColor & val )
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

void medSeedPointAnnotationData::setSelectedSeed( int num )
{
    if ( m_selectedSeed != num ) {
        m_selectedSeed = num;
        emit dataModified(this);
    }
}

void medSeedPointAnnotationData::removeSeed( int i )
{
    m_centerWorld.remove(i);
    emit dataModified(this);
}

void medSeedPointAnnotationData::setNumberOfSeeds( int num )
{
    if ( m_centerWorld.size() != num ) {
        m_centerWorld.resize(num);
        emit dataModified(this);
    }
}



