/*
 * msegSeedPointAnnotationData.cpp
 *
 *  Created on: 31 mai 2011 10:36:54
 *      Author: jstark
 */

#include "medImageMaskAnnotationData.h"

#include "medAbstractDataImage.h"

medImageMaskAnnotationData::medImageMaskAnnotationData() :
    medAnnotationData()
{
}

medImageMaskAnnotationData::~medImageMaskAnnotationData()
{
}


QString medImageMaskAnnotationData::s_description()
{
    static const QString desc = "medImageMaskAnnotationData";
    return desc;
}

QString medImageMaskAnnotationData::description() const
{
    return medImageMaskAnnotationData::s_description();
}

QString medImageMaskAnnotationData::s_identifier()
{
    static const QString id = "medImageMaskAnnotationData";
    return id;
}

QString medImageMaskAnnotationData::identifier() const
{
    return medImageMaskAnnotationData::s_identifier();
}

void * medImageMaskAnnotationData::data()
{
    return (QObject *)this->maskData();
}

void medImageMaskAnnotationData::setData( void* data )
{
    // 
    QObject * obj = reinterpret_cast<QObject*>(data);
    this->setMaskData( qobject_cast<medAbstractDataImage *>(obj) );
}

medAbstractDataImage * medImageMaskAnnotationData::maskData()
{
    return m_maskData;
}

void medImageMaskAnnotationData::setMaskData( medAbstractDataImage * data )
{
    if ( m_maskData != data ) {
        m_maskData = data;
        emit dataModified(this);
    }
}

const medImageMaskAnnotationData::ColorMapType & medImageMaskAnnotationData::colorMap()
{
    return m_colorMap;
}

bool operator<( const QColor & lhs, const QColor & rhs ){
    return lhs.rgba() < rhs.rgba();
}


void medImageMaskAnnotationData::setColorMap( const ColorMapType & colorMap )
{
    m_colorMap = colorMap;
    qSort( m_colorMap );
}
