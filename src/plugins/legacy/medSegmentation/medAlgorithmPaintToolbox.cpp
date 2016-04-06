/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAlgorithmPaintToolbox.h>

#include <medAbstractData.h>
#include <medAbstractImageData.h>
#include <medAbstractImageView.h>
#include <medDataIndex.h>
#include <medImageMaskAnnotationData.h>
#include <medMetaDataKeys.h>
#include <medMessageController.h>
#include <medSegmentationSelectorToolBox.h>
#include <medPluginManager.h>
#include <medDataManager.h>
#include <medAbstractDataFactory.h>
#include <medClickAndMoveEventFilter.h>
#include <medClearMaskCommand.h>

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkLog/dtkLog.h>
#include <dtkCoreSupport/dtkGlobal.h>

#include <QtCore>
#include <QColorDialog>

#include <algorithm>
#include <set>



medAlgorithmPaintToolbox::medAlgorithmPaintToolbox(QWidget *parent ) :
    medSegmentationAbstractToolBox( parent),
    m_MinValueImage(0),
    m_MaxValueImage(500),
    m_strokeRadius(4),
    m_strokeLabel(1),
    m_paintState(PaintState::None)
{
    QWidget *displayWidget = new QWidget(this);
    this->addWidget(displayWidget);

    this->setTitle(this->name());

    QVBoxLayout * layout = new QVBoxLayout(displayWidget);

    m_strokeButton = new QPushButton( tr("Paint / Erase") , displayWidget);
    m_strokeButton->setToolTip(tr("Left-click: Start painting with specified label.\nRight-click: Erase painted voxels."));
    m_strokeButton->setCheckable(true);

    m_magicWandButton = new QPushButton(tr("Magic Wand"), displayWidget);
    QPixmap pixmap(":medSegmentation/pixmaps/magic_wand.png");
    QIcon buttonIcon(pixmap);
    m_magicWandButton->setIcon(buttonIcon);
    m_magicWandButton->setToolTip(tr("Magic wand to automatically paint similar voxels."));
    m_magicWandButton->setCheckable(true);

    QHBoxLayout * addRemoveButtonLayout = new QHBoxLayout();
    addRemoveButtonLayout->addWidget( m_strokeButton );
    addRemoveButtonLayout->addWidget( m_magicWandButton );
    layout->addLayout( addRemoveButtonLayout );


    QHBoxLayout * brushSizeLayout = new QHBoxLayout();
    m_brushSizeSlider = new QSlider(Qt::Horizontal, displayWidget);
    m_brushSizeSlider->setToolTip(tr("Changes the brush radius."));
    m_brushSizeSlider->setValue(this->m_strokeRadius);
    m_brushSizeSlider->setRange(1, 10);
    m_brushSizeSlider->hide();
    m_brushSizeSpinBox = new QSpinBox(displayWidget);
    m_brushSizeSpinBox->setToolTip(tr("Changes the brush radius."));
    m_brushSizeSpinBox->setValue(this->m_strokeRadius);
    m_brushSizeSpinBox->setMinimum(1);
    m_brushSizeSpinBox->setMaximum(10);
    m_brushSizeSpinBox->hide();
    m_brushRadiusLabel = new QLabel(tr("Brush Radius"), displayWidget);
    m_brushRadiusLabel->hide();

    connect(m_brushSizeSpinBox, SIGNAL(valueChanged(int)),m_brushSizeSlider,SLOT(setValue(int)) );
    connect(m_brushSizeSlider,SIGNAL(valueChanged(int)),m_brushSizeSpinBox,SLOT(setValue(int)) );

    brushSizeLayout->addWidget(m_brushRadiusLabel);
    brushSizeLayout->addWidget( m_brushSizeSlider );
    brushSizeLayout->addWidget( m_brushSizeSpinBox );
    layout->addLayout( brushSizeLayout );

    QHBoxLayout * magicWandLayout = new QHBoxLayout();

    m_wandThresholdSizeSlider = new QSlider(Qt::Horizontal, displayWidget);
    m_wandThresholdSizeSlider->setValue(100);
    m_wandThresholdSizeSlider->setMinimum(0);
    m_wandThresholdSizeSlider->setMaximum(1000);
    m_wandThresholdSizeSlider->hide();

    m_wandThresholdSizeSpinBox = new QDoubleSpinBox(displayWidget);
    m_wandThresholdSizeSpinBox->setMinimum(0);
    m_wandThresholdSizeSpinBox->setMaximum(1000000);
    m_wandThresholdSizeSpinBox->setDecimals(2);
    m_wandThresholdSizeSpinBox->hide();

    this->setWandSpinBoxValue(100);

    connect(m_wandThresholdSizeSpinBox, SIGNAL(valueChanged(double)),this,SLOT(setWandSliderValue(double)) );
    connect(m_wandThresholdSizeSlider,SIGNAL(valueChanged(int)),this,SLOT(setWandSpinBoxValue(int)) );

    m_wand3DCheckbox = new QCheckBox (tr("3D"), displayWidget);
    m_wand3DCheckbox->setCheckState(Qt::Unchecked);
    m_wand3DCheckbox->hide();

    magicWandLayout->addWidget( m_wand3DCheckbox );
    magicWandLayout->addWidget( m_wandThresholdSizeSlider );
    magicWandLayout->addWidget( m_wandThresholdSizeSpinBox );
    layout->addLayout( magicWandLayout );

    this->generateLabelColorMap(24);

    QHBoxLayout * labelSelectionLayout = new QHBoxLayout();

    m_strokeLabelSpinBox = new QSpinBox(displayWidget);
    m_strokeLabelSpinBox->setToolTip(tr("Changes the painted label."));
    m_strokeLabelSpinBox->setValue(this->m_strokeLabel);
    m_strokeLabelSpinBox->setMinimum(1);
    m_strokeLabelSpinBox->setMaximum(24);
    m_strokeLabelSpinBox->hide();
    connect (m_strokeLabelSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setLabel(int)));

    m_labelColorWidget = new QPushButton(displayWidget);
    m_labelColorWidget->setToolTip(tr("Current label color"));
    m_labelColorWidget->setStyleSheet("background-color: rgb(255, 0, 0);border:0;border-radius: 0px;width:20px;height:20px;");
    m_labelColorWidget->setCheckable(false);
    m_labelColorWidget->setText("");
    m_labelColorWidget->hide();
    connect(m_labelColorWidget, SIGNAL(clicked()), this, SLOT(setLabelColor()));

    m_colorLabel = new QLabel(tr("Label:"), displayWidget);
    m_colorLabel->hide();

    labelSelectionLayout->addStretch();
    labelSelectionLayout->addWidget(m_colorLabel );
    labelSelectionLayout->addWidget( m_labelColorWidget );
    labelSelectionLayout->addWidget( m_strokeLabelSpinBox );

    layout->addLayout( labelSelectionLayout );

    m_clearMaskButton = new QPushButton( tr("Clear Mask") , displayWidget);
    m_clearMaskButton->setToolTip(tr("Resets the mask."));
    QHBoxLayout * dataButtonsLayout = new QHBoxLayout();
    dataButtonsLayout->addWidget(m_clearMaskButton);
    layout->addLayout(dataButtonsLayout);

    connect (m_strokeButton, SIGNAL(pressed()), this, SLOT(activateStroke ()));
    connect (m_magicWandButton, SIGNAL(pressed()),this,SLOT(activateMagicWand()));
    connect (m_clearMaskButton, SIGNAL(pressed()), this, SLOT(clearMask()));
    connect(this->segmentationToolBox(), SIGNAL(inputChanged()), this, SLOT(updateMouseInteraction()));

    showButtons(false);
}

medAlgorithmPaintToolbox::~medAlgorithmPaintToolbox()
{
}

medAbstractData* medAlgorithmPaintToolbox::processOutput()
{
    return NULL;
}

void medAlgorithmPaintToolbox::setWandSliderValue(double val)
{
    double perc = 4000.0 * val / (m_MaxValueImage - m_MinValueImage);

    unsigned int percRound = (unsigned int)floor(perc);

    m_wandThresholdSizeSlider->blockSignals(true);
    m_wandThresholdSizeSlider->setValue(percRound);
    m_wandThresholdSizeSlider->blockSignals(false);
}

void medAlgorithmPaintToolbox::setWandSpinBoxValue(int val)
{
    double realValue = val * (m_MaxValueImage - m_MinValueImage) / 4000.0;

    // Determine number of decimals necessary

    double testValue = (m_MaxValueImage - m_MinValueImage) / 4.0;

    unsigned int powTestValue = 1;
    while (testValue < 1)
    {
        ++powTestValue;
        testValue *= 10;
    }

    if (powTestValue < 2)
        powTestValue = 2;

    m_wandThresholdSizeSpinBox->setDecimals(powTestValue);
    m_wandThresholdSizeSpinBox->blockSignals(true);
    m_wandThresholdSizeSpinBox->setValue(realValue);
    m_wandThresholdSizeSpinBox->blockSignals(false);
}

void medAlgorithmPaintToolbox::activateStroke()
{
    if ( this->m_strokeButton->isChecked() ) {
        this->m_viewFilter->removeFromAllViews();
        m_paintState = (PaintState::None);
        updateButtons();
        return;
    }
    setPaintState(PaintState::Stroke);
    updateButtons();
    this->m_magicWandButton->setChecked(false);
    m_viewFilter = ( new medClickAndMoveEventFilter(this) );
    m_viewFilter->setColorMap(m_labelColorMap);
    emit installEventFilterRequest(m_viewFilter);
}

void medAlgorithmPaintToolbox::activateMagicWand()
{
    if ( this->m_magicWandButton->isChecked() ) {
        this->m_viewFilter->removeFromAllViews();
        m_paintState = (PaintState::None);
        updateButtons();
        return;
    }
    setPaintState(PaintState::Wand);
    updateButtons();
    this->m_strokeButton->setChecked(false);
    m_viewFilter = ( new medClickAndMoveEventFilter(this) );
    m_viewFilter->setColorMap(m_labelColorMap);
    emit installEventFilterRequest(m_viewFilter);
}

void medAlgorithmPaintToolbox::setLabel(int newVal)
{
    QColor labelColor = m_labelColorMap[newVal-1].second;
    m_labelColorWidget->setStyleSheet("background-color: " + labelColor.name() + ";border:0;border-radius: 0px;width:20px;height:20px;");
}

void medAlgorithmPaintToolbox::setLabelColor()
{
    QColor currentColor = m_labelColorMap[m_strokeLabelSpinBox->value() - 1].second;
    QColor newColor = QColorDialog::getColor(currentColor,this);

    if (newColor.isValid())
    {
        m_labelColorMap[m_strokeLabelSpinBox->value() - 1].second = newColor;

        m_viewFilter->setColorMap(m_labelColorMap);

        this->setLabel(m_strokeLabelSpinBox->value());
    }
}

void medAlgorithmPaintToolbox::clearMask()
{
    medClearMaskCommand *clearCommand = new medClearMaskCommand(m_viewFilter->view());
    clearCommand->setMaskToClear(m_itkMask);
    m_viewFilter->view()->undoStack()->push(clearCommand);

    m_viewFilter->view()->viewWidget()->setFocus();
}

void medAlgorithmPaintToolbox::generateLabelColorMap(unsigned int numLabels)
{
    medImageMaskAnnotationData::ColorMapType colorMap;
    typedef medImageMaskAnnotationData::ColorMapType::value_type PairType;

    QColor tmpColor;
    double realHueValue = 0;
    double factor = (1.0 + sqrt(5.0)) / 2.0;
    for (unsigned int i = 0;i < numLabels;++i)
    {
        tmpColor.setHsvF(realHueValue,1.0,1.0);
        colorMap.push_back(PairType(i+1 , tmpColor));

        realHueValue += 1.0 / factor;
        if (realHueValue > 1.0)
            realHueValue -= 1.0;
    }

    m_labelColorMap = colorMap;
}


void medAlgorithmPaintToolbox::showButtons( bool value )
{
    if (value)
    {
        m_clearMaskButton->show();
    }
    else
    {
        m_clearMaskButton->hide();
    }
}

void medAlgorithmPaintToolbox::updateButtons()
{
    if ( m_paintState == PaintState::None ) {
        m_wandThresholdSizeSlider->hide();
        m_wandThresholdSizeSpinBox->hide();
        m_wand3DCheckbox->hide();
        m_brushSizeSlider->hide();
        m_brushSizeSpinBox->hide();
        m_brushRadiusLabel->hide();
        m_labelColorWidget->hide();
        m_strokeLabelSpinBox->hide();
        m_colorLabel->hide();
        return;
    }
    else
    {
        m_labelColorWidget->show();
        m_strokeLabelSpinBox->show();
        m_colorLabel->show();

        if ( m_paintState == PaintState::Wand ) {
            m_brushSizeSlider->hide();
            m_brushSizeSpinBox->hide();
            m_brushRadiusLabel->hide();
            m_wandThresholdSizeSlider->show();
            m_wandThresholdSizeSpinBox->show();
            m_wand3DCheckbox->show();
        }
        else if ( m_paintState == PaintState::Stroke ) {
            m_brushSizeSlider->show();
            m_brushSizeSpinBox->show();
            m_brushRadiusLabel->show();
            m_wandThresholdSizeSlider->hide();
            m_wandThresholdSizeSpinBox->hide();
            m_wand3DCheckbox->hide();
        }
    }
}

void medAlgorithmPaintToolbox::updateMouseInteraction() //Apply the current interaction (paint, ...) to a new view
{
    if (m_paintState != PaintState::None)
    {
        m_viewFilter = ( new medClickAndMoveEventFilter(this) );
        m_viewFilter->setColorMap(m_labelColorMap);
        emit installEventFilterRequest(m_viewFilter);
    }
}

dtkPlugin* medAlgorithmPaintToolbox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "segmentationPlugin" );
    return plugin;
}

bool medAlgorithmPaintToolbox::isWand3D() const
{
    return m_wand3DCheckbox->isChecked();
}
