/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractImageData.h>
#include <medImageMaskAnnotationData.h>
#include <QColor>

medImageMaskAnnotationData::medImageMaskAnnotationData() :
    medAnnotationData()
{
}

medImageMaskAnnotationData::medImageMaskAnnotationData(const medImageMaskAnnotationData &other)
    : medAnnotationData(other)
{
    this->m_maskData = dynamic_cast<medAbstractImageData*>(other.m_maskData->clone());
}

medImageMaskAnnotationData::~medImageMaskAnnotationData()
{
}


QString medImageMaskAnnotationData::s_description()
{
    static const QString desc = "medImageMaskAnnotationData";
    return desc;
}

QString medImageMaskAnnotationData::description( void ) const
{
    return medImageMaskAnnotationData::s_description();
}

QString medImageMaskAnnotationData::s_identifier()
{
    static const QString id = "medImageMaskAnnotationData";
    return id;
}

QString medImageMaskAnnotationData::identifier( void ) const
{
    return medImageMaskAnnotationData::s_identifier();
}

void * medImageMaskAnnotationData::data( void )
{
    return (QObject *)this->maskData();
}

void medImageMaskAnnotationData::setData( void* data )
{
    QObject * obj = reinterpret_cast<QObject*>(data);
    this->setMaskData( qobject_cast<medAbstractImageData *>(obj) );
}

//! Return the actual mask data.
medAbstractImageData * medImageMaskAnnotationData::maskData()
{
    return m_maskData;
}

void medImageMaskAnnotationData::setMaskData( medAbstractImageData * data )
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

//! Set the color map to be used.
void medImageMaskAnnotationData::setColorMap( const ColorMapType & colorMap )
{
    m_colorMap = colorMap;
    std::sort(m_colorMap.begin(), m_colorMap.end());
}
