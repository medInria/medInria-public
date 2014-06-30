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
#include <dtkCore/dtkSmartPointer.h>

#include <medDataIndex.h>
#include <medViewEventFilter.h>
#include <medImageMaskAnnotationData.h>

#include <QVector3D>
#include <QTextEdit>

#include <vector>

#include <itkImage.h>

class medAbstractData;
class medAbstractImageView;
class medAnnotationData;

class dtkAbstractProcessFactory;
class medSeedPointAnnotationData;
class ClickAndMoveEventFilter;

struct PaintState {
    enum E{ None, Wand, Stroke, DeleteStroke };
};

//! Segmentation toolbox to allow manual painting of pixels
class MEDVIEWSEGMENTATIONPLUGIN_EXPORT AlgorithmPaintToolbox : public medSegmentationAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Paint Segmentation", "Paint Tool",
                          <<"segmentation")
public:


    AlgorithmPaintToolbox( QWidget *parent );
    ~AlgorithmPaintToolbox();

    inline void setPaintState( PaintState::E value){m_paintState = value;}
    inline PaintState::E paintState(){return m_paintState;}

    dtkPlugin* plugin();

    medAbstractData* processOutput();

public slots:
    void activateStroke();
    void activateMagicWand();

    void import();
    void clearMask();

    void setLabel(int newVal);
    void setLabelColor();

    void setWandSliderValue(double val);
    void setWandSpinBoxValue(int val);

    void updateStroke(ClickAndMoveEventFilter * filter, medAbstractImageView * view);
    void updateWandRegion(medAbstractImageView * view, QVector3D &vec);
    void updateMouseInteraction();

protected:
    friend class ClickAndMoveEventFilter;

    void addStroke( medAbstractImageView *view, const QVector3D &vec );
    void setData( medAbstractData *data );

    // update with seed point data.
    void updateTableRow(int row);

    void initializeMaskData( medAbstractData * imageData, medAbstractData * maskData );

    void setOutputMetadata(const medAbstractData * inputData, medAbstractData * outputData);

    void updateFromGuiItems();

    void showButtons( bool value);

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

    double m_MinValueImage;
    double m_MaxValueImage;

    QPushButton *m_applyButton;

    QPushButton *m_clearMaskButton;

    dtkSmartPointer< medViewEventFilter > m_viewFilter;

    dtkSmartPointer<medImageMaskAnnotationData> m_maskAnnotationData;

    dtkSmartPointer<medAbstractData> m_maskData;
    dtkSmartPointer<medAbstractData> m_imageData;

    medImageMaskAnnotationData::ColorMapType m_labelColorMap;

    typedef itk::Image<unsigned char, 3> MaskType;
    MaskType::Pointer m_itkMask;

    template <typename IMAGE> void RunConnectedFilter (MaskType::IndexType &index, unsigned int planeIndex);
    template <typename IMAGE> void GenerateMinMaxValuesFromImage ();

    QVector3D m_lastVup;
    QVector3D m_lastVpn;
    double m_sampleSpacing[2];

    double m_wandRadius;
    double m_strokeRadius;
    unsigned int m_strokeLabel;

    PaintState::E m_paintState;
};



