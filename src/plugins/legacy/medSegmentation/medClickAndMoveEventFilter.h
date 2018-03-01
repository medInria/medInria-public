/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medViewEventFilter.h>
#include <medAlgorithmPaintToolbox.h>

#include <itkImage.h>
#include <itkLabelObject.h>
#include <itkLabelMap.h>
#include <itkLabelMapToLabelImageFilter.h>

class MEDVIEWSEGMENTATIONPLUGIN_EXPORT medClickAndMoveEventFilter : public medViewEventFilter
{
public:

    typedef itk::Image<unsigned char, 3> MaskType;
    typedef itk::AttributeLabelObject<unsigned char, 3, unsigned char> LabelObjectType;
    typedef itk::LabelMap< LabelObjectType > LabelMapType;
    typedef itk::LabelMapToLabelImageFilter< LabelMapType, MaskType > MapToImageFilter;


    medClickAndMoveEventFilter(medAlgorithmPaintToolbox *cb  = NULL);
    ~medClickAndMoveEventFilter();

    virtual bool mousePressEvent(medAbstractView *view, QMouseEvent *mouseEvent );
    virtual bool mouseMoveEvent( medAbstractView *view, QMouseEvent *mouseEvent );
    virtual bool mouseReleaseEvent( medAbstractView *view, QMouseEvent *mouseEvent );

    const QVector<QVector3D> & points() const { return m_points; }

    void setData( medAbstractData *data );
    void initializeMaskData( medAbstractData * imageData, medAbstractData * maskData );

    template <typename IMAGE> void GenerateMinMaxValuesFromImage ();

    void setColorMap( medImageMaskAnnotationData::ColorMapType colorMap);


    MaskType::Pointer mask(){return m_itkMask;}
    medAbstractView* view(){
        return m_view;
    }

protected:
     void setOutputMetadata(const medAbstractData * inputData, medAbstractData * outputData);

private :
    medAlgorithmPaintToolbox *m_toolbox;
    QVector<QVector3D> m_points;
    PaintState::E m_paintState;
    PaintState::E m_lastPaintState;

    dtkSmartPointer<medImageMaskAnnotationData> m_maskAnnotationData;
    //TODO smartPointing have to be managed only in abstraction -rde

    dtkSmartPointer<medAbstractData> m_maskData;
    medAbstractData* m_imageData;
    medAbstractView *m_view;

    MaskType::Pointer m_itkMask;

    MapToImageFilter::Pointer m_filter;

    double m_MinValueImage;
    double m_MaxValueImage;

    medImageMaskAnnotationData::ColorMapType m_labelColorMap;
};

