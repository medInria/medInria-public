/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <medAnnotationData.h>
#include <medAbstractDataImage.h>

#include <dtkCore/dtkSmartPointer.h>

#include <QtCore>

/*
 * Implementation of an overlay image to be used to mark voxels as inside, outside or unknown.
 * Can be added to an image data as an annotation, in which case the size of this mask should
 * match the size of the image.
 * 
 * The class contains a medAbstractDataImage containing the actual mask data. This can be manipulated.
 */
class MEDCORE_EXPORT medImageMaskAnnotationData : public medAnnotationData {
    Q_OBJECT;
public:
    medImageMaskAnnotationData();
    virtual ~medImageMaskAnnotationData();

    //! Implement dtkAbstractObject
    QString description() const;
    QString identifier() const;

    // Implement dtkAbstractData
    virtual void *data();
    virtual void setData(void* data);

    //! Set the color map to be used.
    typedef QList< QPair<double, QColor> > ColorMapType;
    const ColorMapType & colorMap();
    void setColorMap(const ColorMapType & colorMap);

    // static methods returning description and id.
    static QString s_description();
    static QString s_identifier();

    //! Return the actual mask data.
    medAbstractDataImage * maskData();
    void setMaskData( medAbstractDataImage * data );

protected:
    ColorMapType m_colorMap;
    dtkSmartPointer<medAbstractDataImage> m_maskData;
};



