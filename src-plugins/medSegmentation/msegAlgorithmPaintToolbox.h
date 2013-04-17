/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medSegmentationAbstractToolBox.h"

#include "msegPluginExport.h"

#include "medProcessPaintSegm.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medDataIndex.h>
#include <medViewEventFilter.h>
#include <medImageMaskAnnotationData.h>

#include <QVector3D>
#include <QTextEdit>

#include <vector>

class medAbstractData;
class medAbstractView;
class medAnnotationData;

class dtkAbstractProcessFactory;
class medSeedPointAnnotationData;

namespace mseg {
    class ClickAndMoveEventFilter;
    class ClickEventFilter;
    class SelectDataEventFilter;

//! Segmentation toolbox to allow manual painting of pixels
class MEDVIEWSEGMENTATIONPLUGIN_EXPORT AlgorithmPaintToolbox : public medSegmentationAbstractToolBox
{
    Q_OBJECT;
public:
    struct PaintState {
        enum E{ None, Wand, Stroke, DeleteStroke, BoundaryStroke };
    };

    AlgorithmPaintToolbox( QWidget *parent );
    ~AlgorithmPaintToolbox();

     //! Override dtkAbstractObject
    QString description() const { return s_description(); }
    QString identifier() const { return s_identifier(); }

    static medSegmentationAbstractToolBox * createInstance( QWidget *parent );

    static QString s_description();

    /** Get name to use for this when registering with a factory.*/
    static QString s_identifier();

    //! Get a human readable name for this widget.
    /** \param trObj : Provide an object for the tr() function. If NULL qApp will be used. */
    static QString s_name(const QObject * trObj =  NULL);

public slots:
    void onStrokePressed();
    void onMagicWandPressed();
    void onRemoveStrokePressed();
    void onBoundaryStrokePressed();

    void onSetDataPressed();
    void onApplyButtonPressed();
    void onClearMaskPressed();
    void onResetDataPressed();

    void onLabelChanged(int newVal);
    void onSelectLabelColor();

    void setWandSliderValue(double val);
    void setWandSpinBoxValue(int val);

    void updateStroke(ClickAndMoveEventFilter * filter, medAbstractView * view);
    void updateWandRegion(medAbstractView * view, QVector3D &vec);

protected:
    friend class SelectDataEventFilter;

    void addStroke( medAbstractView *view, const QVector3D &vec );
    void setData( dtkAbstractData *data );

    // update with seed point data.
    void updateTableRow(int row);

    void initializeMaskData( medAbstractData * imageData, medAbstractData * maskData );

    void updateFromGuiItems();

    void enableButtons( bool value);

    void setPaintState( PaintState::E value);

    void generateLabelColorMap(unsigned int numLabels);
private:
    typedef dtkSmartPointer<medSeedPointAnnotationData> SeedPoint;

    QPushButton *m_strokeButton;
    QPushButton *m_removeStrokeButton;
    QPushButton *m_boundaryStrokeButton;
    QPushButton *m_labelColorWidget;

    QSlider *m_brushSizeSlider;
    QSpinBox *m_brushSizeSpinBox;
    QSpinBox *m_strokeLabelSpinBox;

    QPushButton *m_magicWandButton;
    // The slider works on percentages of a linear scale between min and max values, i.e.
    // wandradius = (max - min) * sliderPerc / 2.0
    QSlider *m_wandThresholdSizeSlider;
    QDoubleSpinBox *m_wandThresholdSizeSpinBox;
    QCheckBox *m_wand3DCheckbox;

    double m_MinValueImage;
    double m_MaxValueImage;

    QPushButton *m_applyButton;

    QPushButton *m_selectDataButton;
    QPushButton *m_resetDataButton;
    QPushButton *m_clearMaskButton;
    QTextEdit *m_dataText;

    dtkSmartPointer< medViewEventFilter > m_viewFilter;

    dtkSmartPointer<medImageMaskAnnotationData> m_maskAnnotationData;

    dtkSmartPointer<medAbstractData> m_maskData;
    dtkSmartPointer<medAbstractData> m_imageData;

    QString m_noDataText;

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

} // namespace mseg


