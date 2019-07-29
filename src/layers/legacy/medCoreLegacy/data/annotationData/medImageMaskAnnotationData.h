/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAnnotationData.h>
#include <medAbstractImageData.h>

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medCoreLegacyExport.h>

/**
 * Implementation of an overlay image to be used to mark voxels as inside, outside or unknown.
 * Can be added to an image data as an annotation, in which case the size of this mask should
 * match the size of the image.
 *
 * The class contains a medAbstractImageData containing the actual mask data. This can be manipulated.
 */
class MEDCORELEGACY_EXPORT medImageMaskAnnotationData : public medAnnotationData
{
    Q_OBJECT

public:
    medImageMaskAnnotationData();
    medImageMaskAnnotationData(const medImageMaskAnnotationData &other);
    virtual ~medImageMaskAnnotationData();

    QString description() const;
    QString identifier() const;

    virtual void *data();
    virtual void setData(void* data);

    typedef QList< QPair<double, QColor> > ColorMapType;
    const ColorMapType & colorMap();
    void setColorMap(const ColorMapType & colorMap);

    static QString s_description();
    static QString s_identifier();

    medAbstractImageData * maskData();
    void setMaskData( medAbstractImageData * data );

protected:
    ColorMapType m_colorMap;
    dtkSmartPointer<medAbstractImageData> m_maskData;
};



