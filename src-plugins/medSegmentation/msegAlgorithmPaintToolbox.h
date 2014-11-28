/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medSegmentationAbstractToolBox.h>

#include <msegPluginExport.h>

#include <medAbstractData.h>

#include <medDataIndex.h>
#include <medViewEventFilter.h>
#include <medImageMaskAnnotationData.h>

#include <QVector3D>
#include <QTextEdit>

#include <vector>

#include <itkImage.h>
#include <itkLabelObject.h>
#include <itkLabelMap.h>
#include <itkAttributeLabelObject.h>

class medAbstractData;
class medAbstractImageView;
class medAnnotationData;

class dtkAbstractProcessFactory;
class medSeedPointAnnotationData;

class medClickAndMoveEventFilter;

struct PaintState {
    enum E{ None, Wand, Stroke, DeleteStroke };
};

//! Segmentation toolbox to allow manual painting of pixels
class MEDVIEWSEGMENTATIONPLUGIN_EXPORT medAlgorithmPaintToolbox : public medSegmentationAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Paint Segmentation", "Paint Tool",
                          <<"segmentation")
public:

    typedef itk::Image<unsigned char, 3> MaskType;

    medAlgorithmPaintToolbox( QWidget *parent );
    ~medAlgorithmPaintToolbox();

    inline void setPaintState( PaintState::E value){m_paintState = value;}
    inline PaintState::E paintState(){return m_paintState;}

    dtkPlugin* plugin();

    medAbstractData* processOutput();

    double wandRadius () const  { return m_wandThresholdSizeSpinBox->value(); }
    double strokeRadius( ) const {return m_brushSizeSlider->value(); }
    unsigned int strokeLabel( )  const {return m_strokeLabelSpinBox->value(); }
    bool isWand3D () const;

    void setMask(MaskType::Pointer mask){m_itkMask = mask;}


public slots:
    void activateStroke();
    void activateMagicWand();

    void clearMask();

    void setLabel(int newVal);
    void setLabelColor();

    void setWandSliderValue(double val);
    void setWandSpinBoxValue(int val);

    void setMinValue(double min){m_MinValueImage = min;}
    void setMaxValue(double max){m_MaxValueImage = max;}

    void updateMouseInteraction();

    void showButtons( bool value);

protected:
    friend class ClickAndMoveEventFilter;

    // update with seed point data.
    void updateTableRow(int row);

    void generateLabelColorMap(unsigned int numLabels);

    void updateButtons();

signals:
    void installEventFilterRequest(medViewEventFilter *filter);

private:
    typedef dtkSmartPointer<medSeedPointAnnotationData> SeedPoint;

    QPushButton *m_strokeButton;
    QPushButton *m_labelColorWidget;
    QSpinBox *m_strokeLabelSpinBox;
    QLabel *m_colorLabel;

    QSlider *m_brushSizeSlider;
    QSpinBox *m_brushSizeSpinBox;
    QLabel *m_brushRadiusLabel;

    QPushButton *m_magicWandButton;
    // The slider works on percentages of a linear scale between min and max values, i.e.
    // wandradius = (max - min) * sliderPerc / 2.0
    QSlider *m_wandThresholdSizeSlider;
    QDoubleSpinBox *m_wandThresholdSizeSpinBox;
    QCheckBox *m_wand3DCheckbox;

    QPushButton *m_clearMaskButton;

    dtkSmartPointer< medClickAndMoveEventFilter > m_viewFilter;

    medImageMaskAnnotationData::ColorMapType m_labelColorMap;

    MaskType::Pointer m_itkMask;

    double m_MinValueImage;
    double m_MaxValueImage;

    double m_wandRadius;
    double m_strokeRadius;
    unsigned int m_strokeLabel;

    PaintState::E m_paintState;
};



