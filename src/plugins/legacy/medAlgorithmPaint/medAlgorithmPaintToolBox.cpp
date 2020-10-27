/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkConnectedThresholdImageFilter.h>
#include <itkDanielssonDistanceMapImageFilter.h>
#include <itkExceptionObject.h>
#include <itkExtractImageFilter.h>
#include <itkImageLinearIteratorWithIndex.h>
#include <itkImageSliceIteratorWithIndex.h>
#include <itkInvertIntensityImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkSubtractImageFilter.h>

#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medAbstractImageView.h>
#include <medAbstractInteractor.h>
#include <medAbstractLayeredView.h>
#include <medAbstractParameterL.h>
#include <medAbstractProcessLegacy.h>
#include <medAlgorithmPaintToolBox.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medMessageController.h>
#include <medPluginManager.h>
#include <medSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medUtilities.h>
#include <medViewContainer.h>
#include <medViewContainerManager.h>

#include <vnl/vnl_cross.h>

namespace med
{

class ClickAndMoveEventFilter : public medViewEventFilter
{
public:
    ClickAndMoveEventFilter(AlgorithmPaintToolBox *cb = nullptr) :
        medViewEventFilter(),
        m_cb(cb),
        m_paintState(PaintState::None),
        m_lastPaintState(PaintState::None),
        timer(){}

    virtual bool mousePressEvent(medAbstractView *view, QMouseEvent *mouseEvent)
    {
        m_paintState = m_cb->paintState();
        m_cb->m_applyButton->setDisabled(false);
        m_cb->m_interpolateButton->setEnabled(true);
        m_cb->m_addButton->setEnabled(true);
        m_cb->m_eraseButton->setEnabled(true);

        if ( this->m_paintState == PaintState::DeleteStroke )
        {
            m_cb->setPaintState(m_lastPaintState);
            m_paintState = m_lastPaintState;
        }

        if(mouseEvent->button() == Qt::RightButton) // right-click for erasing
        {
            m_lastPaintState = m_cb->paintState();
            m_cb->setPaintState(PaintState::DeleteStroke);
            m_paintState = m_cb->paintState(); //update
        }

        if (m_paintState == PaintState::Stroke && mouseEvent->button() == Qt::LeftButton)
        {
            m_cb->setPaintState(PaintState::Stroke);
            m_paintState = m_cb->paintState(); // update paintState
        }

        mouseEvent->accept();

        medAbstractImageView *imageView = dynamic_cast<medAbstractImageView *>(view);
        if(!imageView)
        {
            return false;
        }
        m_cb->setCurrentView(imageView);

        if (imageView->is2D())
        {
            // Convert mouse click to a 3D point in the image.
            int devicePixelRatio = medUtilities::getDevicePixelRatio(mouseEvent);
            QPointF mousePos = mouseEvent->localPos() * devicePixelRatio;
            QVector3D posImage = imageView->mapDisplayToWorldCoordinates( mousePos );

            if (m_paintState != PaintState::Wand)
            {
                // add current state to undo stack
                bool isInside;
                MaskType::IndexType index;
                m_cb->setCurrentPlaneIndex(m_cb->computePlaneIndex(posImage, index, isInside));
                m_cb->setCurrentIdSlice(index[m_cb->currentPlaneIndex]);

                // For undo/redo purposes -------------------------
                QList<unsigned int> listIdSlice;
                listIdSlice.append(m_cb->currentIdSlice);
                m_cb->addSliceToStack(view, m_cb->currentPlaneIndex, listIdSlice);

                // -------------------------------------------------
                this->m_points.push_back(posImage);
            }
            else
            {
                m_cb->newSeed();
                m_cb->wandTimer.start();
                m_cb->setSeed(posImage);
                m_cb->updateWandRegion(imageView, posImage);
                m_paintState = PaintState::None; // Wand operation is over
            }
        }

        return mouseEvent->isAccepted();
    }

    virtual bool mouseMoveEvent( medAbstractView *view, QMouseEvent *mouseEvent )
    {
        medAbstractImageView *imageView = dynamic_cast<medAbstractImageView *>(view);
        if(!imageView)
        {
            return false;
        }

        if (m_paintState == PaintState::None
                && (m_cb->m_paintState == PaintState::Stroke || m_cb->m_paintState == PaintState::DeleteStroke)
                && (m_cb->undoRedoCopyPasteModeOn))
        {
            mouseEvent->accept();

            int elapsed = timer.elapsed();

            qDebug() << elapsed;
            if (elapsed<10) // 1000/24 (24 images per second)
            {
                return false;
            }

            if (imageView->is2D())
            {
                int devicePixelRatio = medUtilities::getDevicePixelRatio(mouseEvent);
                QPointF mousePos = mouseEvent->localPos() * devicePixelRatio;
                QVector3D posImage = imageView->mapDisplayToWorldCoordinates( mousePos );

                bool isInside;
                MaskType::IndexType index;
                unsigned int planeIndex = m_cb->computePlaneIndex(posImage,index,isInside);
                unsigned int idSlice = index[planeIndex];

                if (planeIndex != m_cb->getCurrentPlaneIndex() || idSlice != m_cb->getCurrentIdSlice())
                {
                    m_cb->setCurrentPlaneIndex(planeIndex);
                    m_cb->setCurrentIdSlice(idSlice);
                }

                // Project vector onto plane
                this->m_points.push_back(posImage);
                m_cb->updateStroke( this,imageView );
                timer.start();
            }
            return mouseEvent->isAccepted();
        }

        if ( this->m_paintState == PaintState::None )
        {
            return false;
        }

        mouseEvent->accept();

        if (imageView->is2D())
        {
            int devicePixelRatio = medUtilities::getDevicePixelRatio(mouseEvent);
            QPointF mousePos = mouseEvent->localPos() * devicePixelRatio;
            QVector3D posImage = imageView->mapDisplayToWorldCoordinates( mousePos );
            //Project vector onto plane
            this->m_points.push_back(posImage);
            m_cb->updateStroke( this,imageView );
        }

        return mouseEvent->isAccepted();
    }

    virtual bool mouseReleaseEvent( medAbstractView *view, QMouseEvent *mouseEvent )
    {
        Q_UNUSED(mouseEvent)

        medAbstractImageView *imageView = dynamic_cast<medAbstractImageView *>(view);
        if(imageView)
        {
            if ( this->m_paintState != PaintState::None )
            {
                if (imageView->is2D())
                {
                    m_paintState = PaintState::None; // Painting is done
                    m_cb->updateStroke(this, imageView);
                    this->m_points.clear();
                    timer.start();

                    return true;
                }
            }
        }

        return false;
    }

    virtual bool mouseWheelEvent(medAbstractView *view, QWheelEvent *event)
    {
        medAbstractImageView *imageView = dynamic_cast<medAbstractImageView *>(view);
        if(!imageView)
        {
            return false;
        }

        if ((m_cb->paintState() == PaintState::Stroke || m_cb->paintState() == PaintState::DeleteStroke) && (event->modifiers()==Qt::ControlModifier))
        {
            int numDegrees = event->delta() / 8;
            int numSteps = numDegrees / 15;
            if (event->orientation() == Qt::Horizontal)
            {
                m_cb->addBrushSize(-numSteps);
            }
            else
            {
                m_cb->addBrushSize(numSteps);
            }
            return true;
        }
        return false;
    }

    const std::vector<QVector3D> & points() const { return m_points; }

private :
    AlgorithmPaintToolBox *m_cb;
    std::vector<QVector3D> m_points;
    PaintState::E m_paintState;
    PaintState::E m_lastPaintState;
    QTime timer; // timer used to improve the visualization of the cursor
};

AlgorithmPaintToolBox::AlgorithmPaintToolBox(QWidget *parent ) :
    medAbstractSelectableToolBox(parent),
    m_paintState(PaintState::None)
{
    // Instantiate the event filter only once
    m_viewFilter = new ClickAndMoveEventFilter(this);

    QWidget *displayWidget = new QWidget(this);
    this->addWidget(displayWidget);

    QVBoxLayout *layout = new QVBoxLayout(displayWidget);

    m_strokeButton = new QPushButton( tr("Paint / Erase") );
    m_strokeButton->setToolTip(tr("Left-click: Start painting with specified label.\nRight-click: Erase painted voxels."));
    m_strokeButton->setCheckable(true);
    m_strokeButton->setObjectName("paintButton");

    m_interpolateButton = new QPushButton( tr("Interpolate") );

    m_magicWandButton = new QPushButton(tr("Magic Wand"));
    m_magicWandButton->setObjectName("Magic Wand");
    QPixmap pixmap(":medSegmentation/pixmaps/magic_wand.png");
    QIcon buttonIcon(pixmap);
    m_magicWandButton->setIcon(buttonIcon);
    m_magicWandButton->setToolTip(tr("Magic wand to automatically paint similar voxels."));
    m_magicWandButton->setCheckable(true);

    QHBoxLayout *ButtonLayout = new QHBoxLayout();
    ButtonLayout->addWidget( m_strokeButton );
    ButtonLayout->addWidget( m_magicWandButton );
    ButtonLayout->addWidget( m_interpolateButton );
    layout->addLayout( ButtonLayout );

    // Brush radius label/slider/spinbox
    QHBoxLayout *brushSizeLayout = new QHBoxLayout();
    m_brushSizeSlider = new medIntParameterL("Brush Radius (mm)");
    m_brushSizeSlider->setToolTip(tr("Changes the brush radius."));
    m_brushSizeSlider->setValue(4);
    m_brushSizeSlider->setRange(1, 30);
    m_brushSizeSlider->getSlider()->setPageStep(1);
    m_brushSizeSlider->getSlider()->setOrientation(Qt::Horizontal);
    m_brushSizeSlider->hide();

    connect(m_brushSizeSlider->getSpinBox(),
            SIGNAL(valueChanged(int)),m_brushSizeSlider->getSlider(), SLOT(setValue(int)) );
    connect(m_brushSizeSlider->getSlider(),
            SIGNAL(valueChanged(int)),m_brushSizeSlider->getSpinBox(),SLOT(setValue(int)) );
    connect(m_brushSizeSlider->getSpinBox(),
            SIGNAL(valueChanged(int)),this,SLOT(activateCustomedCursor(void)) );
    connect(m_brushSizeSlider->getSlider(),
            SIGNAL(valueChanged(int)),this,SLOT(activateCustomedCursor(void)) );

    brushSizeLayout->addWidget( m_brushSizeSlider->getLabel() );
    brushSizeLayout->addWidget( m_brushSizeSlider->getSlider() );
    brushSizeLayout->addWidget( m_brushSizeSlider->getSpinBox() );

    layout->addLayout( brushSizeLayout );

    // Magic wand's widgets
    m_wandUpperThresholdSlider = new medDoubleParameterL("Upper Threshold", this);
    m_wandUpperThresholdSlider->setObjectName("Upper Threshold");
    m_wandUpperThresholdSlider->setToolTip(tr("Changes the upper threshold."));
    m_wandUpperThresholdSlider->setValue(0);
    m_wandUpperThresholdSlider->setRange(0, 10000);
    m_wandUpperThresholdSlider->getSlider()->setPageStep(1000);
    m_wandUpperThresholdSlider->getSlider()->setOrientation(Qt::Horizontal);
    m_wandUpperThresholdSlider->getSpinBox()->setKeyboardTracking(false); //prevents undesired emissions of valueChanged()
    m_wandUpperThresholdSlider->getSpinBox()->setSingleStep(1);
    m_wandUpperThresholdSlider->hide();

    m_wandLowerThresholdSlider = new medDoubleParameterL("Lower Threshold", this);
    m_wandLowerThresholdSlider->setObjectName("Lower Threshold");
    m_wandLowerThresholdSlider->setToolTip(tr("Changes the lower threshold."));
    m_wandLowerThresholdSlider->setValue(0);
    m_wandLowerThresholdSlider->setRange(0, 10000);
    m_wandLowerThresholdSlider->getSlider()->setPageStep(1000);
    m_wandLowerThresholdSlider->getSlider()->setOrientation(Qt::Horizontal);
    m_wandLowerThresholdSlider->getSpinBox()->setKeyboardTracking(false);
    m_wandLowerThresholdSlider->getSpinBox()->setSingleStep(1);
    m_wandLowerThresholdSlider->hide();

    // Sliders connects are in updateMagicWandComputationSpeed() and depend on realTime parameter
    connect(m_wandUpperThresholdSlider->getSpinBox(),SIGNAL(valueChanged(double)),this,SLOT(updateMagicWandComputation()),Qt::UniqueConnection);
    connect(m_wandLowerThresholdSlider->getSpinBox(),SIGNAL(valueChanged(double)),this,SLOT(updateMagicWandComputation()),Qt::UniqueConnection);

    wandTimer = QTime();

    // Remove seed button
    m_removeSeedButton = new QPushButton("Remove seed");
    m_removeSeedButton->hide();
    seedPlanted = false;

    connect(m_removeSeedButton,SIGNAL(clicked()),this,SLOT(removeSeed()));

    m_wand3DCheckbox = new QCheckBox (tr("Activate 3D mode"));
    m_wand3DCheckbox->setObjectName("Activate 3D mode");
    m_wand3DCheckbox->setCheckState(Qt::Unchecked);
    m_wand3DCheckbox->hide();

    m_wand3DRealTime = new QCheckBox (tr("RealTime Computation"));
    m_wand3DRealTime->setCheckState(Qt::Unchecked);
    m_wand3DRealTime->hide();

    connect(m_wand3DRealTime,SIGNAL(stateChanged(int)),this,SLOT(updateMagicWandComputationSpeed()));
    updateMagicWandComputationSpeed();

    m_wandInfo = new QLabel("Select a pixel in the image to plant the seed");
    m_wandInfo->hide();

    QHBoxLayout *magicWandCheckboxes = new QHBoxLayout();
    magicWandCheckboxes->addWidget(m_wand3DCheckbox);
    magicWandCheckboxes->addWidget(m_wand3DRealTime);
    QHBoxLayout *magicWandLayout1 = new QHBoxLayout();
    magicWandLayout1->addWidget( m_wandLowerThresholdSlider->getLabel() );
    magicWandLayout1->addWidget( m_wandLowerThresholdSlider->getSlider() );
    magicWandLayout1->addWidget( m_wandLowerThresholdSlider->getSpinBox() );
    QHBoxLayout *magicWandLayout2 = new QHBoxLayout();
    magicWandLayout2->addWidget( m_wandUpperThresholdSlider->getLabel() );
    magicWandLayout2->addWidget( m_wandUpperThresholdSlider->getSlider() );
    magicWandLayout2->addWidget( m_wandUpperThresholdSlider->getSpinBox() );
    QHBoxLayout *magicWandLayout3 = new QHBoxLayout();
    magicWandLayout3->addWidget( m_removeSeedButton );

    magicWandLayout = new QFormLayout();
    magicWandLayout->addRow(m_wandInfo);
    magicWandLayout->addRow(magicWandCheckboxes);
    magicWandLayout->addRow(magicWandLayout1);
    magicWandLayout->addRow(magicWandLayout2);
    magicWandLayout->addRow(magicWandLayout3);

    layout->addLayout(magicWandLayout);

    this->generateLabelColorMap(24);

    QHBoxLayout *labelSelectionLayout = new QHBoxLayout();

    int defaultLabelValue = 1;
    m_strokeLabelSpinBox = new QSpinBox();
    m_strokeLabelSpinBox->setToolTip(tr("Changes the painted label."));
    m_strokeLabelSpinBox->setValue(defaultLabelValue);
    m_strokeLabelSpinBox->setMinimum(1);
    m_strokeLabelSpinBox->setMaximum(23);
    m_strokeLabelSpinBox->hide();
    connect (m_strokeLabelSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setLabel(int)));

    m_labelColorWidget = new QPushButton();
    m_labelColorWidget->setToolTip(tr("Current label color"));
    m_labelColorWidget->setStyleSheet("background-color: rgb(255, 0, 0);border:0;border-radius: 0px;width:20px;height:20px;");
    m_labelColorWidget->setCheckable(false);
    m_labelColorWidget->setText("");
    m_labelColorWidget->hide();
    connect(m_labelColorWidget, SIGNAL(clicked()), this, SLOT(setLabelColor()));
    setLabel(defaultLabelValue);

    m_colorLabel = new QLabel(tr("Label:"));
    m_colorLabel->hide();

    labelSelectionLayout->addStretch();
    labelSelectionLayout->addWidget(m_colorLabel );
    labelSelectionLayout->addWidget( m_labelColorWidget );
    labelSelectionLayout->addWidget( m_strokeLabelSpinBox );

    layout->addLayout( labelSelectionLayout );

    m_addButton = new QPushButton( tr("Add") );
    m_addButton->setToolTip(tr("Append mask to existing mask."));
    m_addButton->setObjectName("addButton");
    m_addButton->setDisabled(true);
    m_addButton->hide();
    m_eraseButton = new QPushButton( tr("Erase") );
    m_eraseButton->setToolTip(tr("Delete mask from existing mask."));
    m_eraseButton->setObjectName("eraseButton");
    m_eraseButton->setDisabled(true);
    m_eraseButton->hide();
    QHBoxLayout * operationsButtonsLayout = new QHBoxLayout();
    operationsButtonsLayout->addWidget(m_addButton);
    operationsButtonsLayout->addWidget(m_eraseButton);
    layout->addLayout(operationsButtonsLayout);

    m_applyButton = new QPushButton( tr("Save") );
    m_applyButton->setToolTip(tr("Save result to the Database"));
    m_applyButton->setObjectName("saveButton");
    m_applyButton->setDisabled(true);

    m_clearMaskButton = new QPushButton( tr("Clear Mask") );
    m_clearMaskButton->setToolTip(tr("Resets the mask."));
    m_clearMaskButton->setObjectName("clearMaskButton");
    QHBoxLayout * dataButtonsLayout = new QHBoxLayout();
    dataButtonsLayout->addWidget(m_applyButton);
    dataButtonsLayout->addWidget(m_clearMaskButton);
    layout->addLayout(dataButtonsLayout);

    connect (m_strokeButton, SIGNAL(pressed()), this, SLOT(activateStroke ()));
    connect (m_magicWandButton, SIGNAL(pressed()),this,SLOT(activateMagicWand()));
    connect (m_clearMaskButton, SIGNAL(pressed()), this, SLOT(clear()));
    connect (m_applyButton, SIGNAL(pressed()),this, SLOT(import()));

    if (this->selectorToolBox())
    {
        connect(this->selectorToolBox(), SIGNAL(inputChanged()),
                this, SLOT(updateMouseInteraction()));
    }
    connect (m_interpolateButton, SIGNAL(clicked()), this, SLOT(interpolate()));

    showButtons(false);

    undo_shortcut = new QShortcut(QKeySequence(tr("Ctrl+z","Undo segmentation")),this);
    redo_shortcut = new QShortcut(QKeySequence(tr("Ctrl+y","Redo segmentation")),this);
    copy_shortcut = new QShortcut(QKeySequence(tr("Ctrl+c","Copy segmentation")),this);
    paste_shortcut = new QShortcut(QKeySequence(tr("Ctrl+v","Paste segmentation")),this);
    removeSeed_shortcut = new QShortcut(QKeySequence(tr("Ctrl+BackSpace","Remove seed")),this);
    addBrushSize_shortcut = new QShortcut(QKeySequence(tr("Ctrl+Up","Add brush size")),this);
    reduceBrushSize_shortcut = new QShortcut(QKeySequence(tr("Ctrl+Down","Reduce brush size")),this);

    removeSeed_shortcut->setEnabled(false);
    addBrushSize_shortcut->setEnabled(false);
    reduceBrushSize_shortcut->setEnabled(false);

    m_copy.first  = 0;
    m_copy.second = -1;
    viewCopied = nullptr;

    m_undoStacks = new QHash<medAbstractView*,QStack<PairListSlicePlaneId>*>();
    m_redoStacks = new QHash<medAbstractView*,QStack<PairListSlicePlaneId>*>();

    currentPlaneIndex = 0;
    currentIdSlice = 0;
    undoRedoCopyPasteModeOn = false;
    currentView = nullptr;
    currentSliceLabel = 0;

    //Shortcuts
    connect(undo_shortcut,SIGNAL(activated()),this,SLOT(undo()));
    connect(redo_shortcut,SIGNAL(activated()),this,SLOT(redo()));
    connect(copy_shortcut,SIGNAL(activated()),this,SLOT(copySliceMask()));
    connect(paste_shortcut,SIGNAL(activated()),this,SLOT(pasteSliceMask()));
    connect(removeSeed_shortcut,SIGNAL(activated()),this,SLOT(removeSeed()));
    connect(addBrushSize_shortcut,SIGNAL(activated()),this,SLOT(increaseBrushSize()));
    connect(reduceBrushSize_shortcut,SIGNAL(activated()),this,SLOT(reduceBrushSize()));

    maskHasBeenSaved = false;
    interpolatedMaskPixelValue = 24;
    setOfPaintBrushRois.clear();
    slicingParameter = nullptr;
}

AlgorithmPaintToolBox::~AlgorithmPaintToolBox()
{
    setOfPaintBrushRois.clear();
}

medAbstractData* AlgorithmPaintToolBox::processOutput()
{
    // Check if painted data on the volume
    if (!m_undoStacks->empty() && !m_undoStacks->value(currentView)->isEmpty())
    {
        updateMaskWithMasterLabel();
        copyMetaData(m_maskData, m_imageData);
        return m_maskData; // return output data
    }
    else
    {
        return nullptr;
    }
}

bool AlgorithmPaintToolBox::registered()
{
    medToolBoxFactory* factory = medToolBoxFactory::instance();
    return factory->registerToolBox<AlgorithmPaintToolBox> ();
}

void AlgorithmPaintToolBox::updateMagicWandComputation()
{
    if (seedPlanted)
    {
        if (m_wand3DCheckbox->isChecked() && wandTimer.elapsed()<600) // 1000/24 (24 images per second)
        {
            return;
        }

        undo();
        updateWandRegion(currentView,m_seed);
        wandTimer.start();
    }
}

void AlgorithmPaintToolBox::activateStroke()
{
    m_wandInfo->hide();

    if ( this->m_strokeButton->isChecked() )
    {
        deactivateCustomedCursor(); // Deactivate painting cursor
        this->m_viewFilter->removeFromAllViews();
        m_paintState = (PaintState::None);
        updateButtons();
        return;
    }
    setPaintState(PaintState::Stroke);
    updateButtons();
    this->m_magicWandButton->setChecked(false);
    addViewEventFilter(m_viewFilter);
    addBrushSize_shortcut->setEnabled(true);
    reduceBrushSize_shortcut->setEnabled(true);

    activateCustomedCursor(); // Add circular cursor for painting
}

void AlgorithmPaintToolBox::activateCustomedCursor()
{
    if (!currentView) // no data
    {
        return;
    }

    if (currentView->orientation() == medImageView::VIEW_ORIENTATION_3D)
    {
        // 3D view
        deactivateCustomedCursor();
        return;
    }

    // Get radius size of the brush in mm
    double radiusSize = (double)(m_brushSizeSlider->value());

    // Adapt to scale of view (zoom, crop, etc)
    double radiusSizeDouble = radiusSize * currentView->scale();

    int radiusSizeInt = floor(radiusSizeDouble + 0.5);

    // Create shape of the new cursor
    QPixmap pix(radiusSizeInt, radiusSizeInt);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setBackground(QColor(255,255,255,255));
    painter.setPen( Qt::white );
    painter.drawEllipse( 0, 0, radiusSizeInt, radiusSizeInt );
    painter.setPen( Qt::cyan );
    painter.drawPoint(floor(radiusSizeDouble/2.0+0.5),     floor(radiusSizeDouble/2.0+0.5));
    painter.drawPoint(floor(radiusSizeDouble/2.0-1.0+0.5), floor(radiusSizeDouble/2.0+0.5));
    painter.drawPoint(floor(radiusSizeDouble/2.0+1.0+0.5), floor(radiusSizeDouble/2.0+0.5));
    painter.drawPoint(floor(radiusSizeDouble/2.0+0.5),     floor(radiusSizeDouble/2.0-1.0+0.5));
    painter.drawPoint(floor(radiusSizeDouble/2.0+0.5),     floor(radiusSizeDouble/2.0+1.0+0.5));

    // Update the cursor
    currentView->viewWidget()->setCursor(QCursor(pix, -1, -1));
}

void AlgorithmPaintToolBox::deactivateCustomedCursor()
{
    if (!currentView) // no data
    {
        return;
    }

    currentView->viewWidget()->setCursor(Qt::CrossCursor);
}

void AlgorithmPaintToolBox::activateMagicWand()
{
    if ( this->m_magicWandButton->isChecked() )
    {
        this->m_viewFilter->removeFromAllViews();
        m_paintState = (PaintState::None);
        newSeed(); // accept the current growth
        updateButtons();
        return;
    }
    setPaintState(PaintState::Wand);
    updateButtons();
    this->m_strokeButton->setChecked(false);
    addViewEventFilter(m_viewFilter);
    deactivateCustomedCursor();
}

void AlgorithmPaintToolBox::updateMagicWandComputationSpeed()
{
    if (m_wand3DRealTime->isChecked())
    {
        connect(m_wandUpperThresholdSlider->getSlider(),SIGNAL(valueChanged(int)),this,SLOT(updateMagicWandComputation()),Qt::UniqueConnection);
        connect(m_wandLowerThresholdSlider->getSlider(),SIGNAL(valueChanged(int)),this,SLOT(updateMagicWandComputation()),Qt::UniqueConnection);

        disconnect(m_wandUpperThresholdSlider->getSlider(),SIGNAL(sliderReleased()),this,SLOT(updateMagicWandComputation()));
        disconnect(m_wandLowerThresholdSlider->getSlider(),SIGNAL(sliderReleased()),this,SLOT(updateMagicWandComputation()));
    }
    else
    {
        disconnect(m_wandUpperThresholdSlider->getSlider(),SIGNAL(valueChanged(int)),this,SLOT(updateMagicWandComputation()));
        disconnect(m_wandLowerThresholdSlider->getSlider(),SIGNAL(valueChanged(int)),this,SLOT(updateMagicWandComputation()));

        connect(m_wandUpperThresholdSlider->getSlider(),SIGNAL(sliderReleased()),this,SLOT(updateMagicWandComputation()),Qt::UniqueConnection);
        connect(m_wandLowerThresholdSlider->getSlider(),SIGNAL(sliderReleased()),this,SLOT(updateMagicWandComputation()),Qt::UniqueConnection);
    }
}

void AlgorithmPaintToolBox::copyMetaData(medAbstractData *output,
                                         medAbstractData *input)
{
    medUtilities::setDerivedMetaData(output, input, "painted");
}

void AlgorithmPaintToolBox::updateMaskWithMasterLabel()
{
    MaskType::RegionType inputRegion = m_itkMask->GetLargestPossibleRegion();
    std::vector<std::pair<unsigned int, int>> masterRois;
    for(auto it = setOfPaintBrushRois.begin(); it != setOfPaintBrushRois.end(); )
    {
        auto& pB = *it;
        if( pB->isMasterRoi() )
        {
            masterRois.push_back(std::pair<unsigned int, int>(pB->getIdSlice(), pB->getLabel()));
        }
        ++it;
    }
    if (!masterRois.empty())
    {
        std::vector<std::tuple<int, unsigned int, unsigned int>> masterLabels;
        unsigned int firstSlice, secondSlice;
        int firstLabel, secondLabel;
        for (auto it = masterRois.begin(); it != masterRois.end() - 1; )
        {
            firstSlice = (*it).first;
            firstLabel = (*it).second;
            advance(it, 1);
            secondSlice = (*it).first;
            secondLabel = (*it).second;
            if (firstLabel == secondLabel)
            {
                masterLabels.push_back(std::make_tuple(firstLabel,firstSlice,secondSlice));
            }
        }

        itk::ImageRegionIterator<MaskType> iterator(m_itkMask, inputRegion);
        typename MaskType::IndexType index;
        while(!iterator.IsAtEnd())
        {
            index = iterator.GetIndex();
            if ( m_itkMask->GetPixel(index) == interpolatedMaskPixelValue )
            {
                for (std::vector<std::tuple<int, unsigned int, unsigned int>>::iterator it=masterLabels.begin(); it!=masterLabels.end(); ++it)
                {
                    if ( index[2] < std::get<2>(*it) && index[2] > std::get<1>(*it) )
                    {
                        m_itkMask->SetPixel(index, std::get<0>(*it));
                    }
                }
            }
            ++iterator;
        }
    }
}

void AlgorithmPaintToolBox::import()
{
    updateMaskWithMasterLabel();

    // Import a copy of the mask
    medAbstractData *output = dynamic_cast<medAbstractData*>(m_maskData->clone());

    copyMetaData(output, m_imageData);
    medDataManager::instance()->importData(output, false);

    maskHasBeenSaved = true;
}

void AlgorithmPaintToolBox::updateView()
{
    setButtonsDisabled(true);
    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();

    if (view)
    {
        medAbstractImageView *v = qobject_cast<medAbstractImageView*>(view);
        if (v)
        {
            // Toolbox does not work with meshes or vector images
            for (unsigned int i=0; i<v->layersCount(); ++i)
            {
                medAbstractData *data = v->layerData(i);
                if(!data || data->identifier().contains("vtkDataMesh")
                        || !data->identifier().contains("itkDataImage") //avoid medVtkFibersData also
                        || data->identifier().contains("itkDataImageVector"))
                {
                    handleDisplayError(medAbstractProcessLegacy::DIMENSION_3D);
                    return;
                }
            }
            setButtonsDisabled(false);
            currentView = v;
            updateMouseInteraction();

            medAbstractData *data = currentView->layerData(0);
            medImageMaskAnnotationData *existingMaskAnnData = dynamic_cast<medImageMaskAnnotationData *>(data);
            if(!existingMaskAnnData)
            {
                setData( data );
            }

            // Update cursor if the orientation change
            connect(currentView, SIGNAL(orientationChanged()), this, SLOT(activateCustomedCursor()), Qt::UniqueConnection);

            connect(currentView,SIGNAL(layerRemoved(unsigned int)),this,SLOT(clear()), Qt::UniqueConnection);

            // Update cursor to new view
            if ( this->m_strokeButton->isChecked() )
            {
                activateCustomedCursor(); // Add circular cursor for painting
            }
            else
            {
                deactivateCustomedCursor(); // Deactivate painting cursor
            }

            slicingParameter = nullptr;
            for(medAbstractInteractor *interactor : qobject_cast<medAbstractLayeredView*>(currentView)->layerInteractors(0))
            {
                if ((interactor->identifier() == "medVtkViewItkDataImageInteractor") ||
                        (interactor->identifier() == "medVtkViewItkDataImage4DInteractor"))
                {
                    for(medAbstractParameterL *parameter : interactor->linkableParameters())
                    {
                        if (parameter->name() == "Slicing")
                        {
                            slicingParameter = qobject_cast<medIntParameterL*>(parameter);
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
}

void AlgorithmPaintToolBox::setButtonsDisabled(bool disable)
{
    m_strokeButton->setDisabled(disable);
    m_magicWandButton->setDisabled(disable);
    m_interpolateButton->setDisabled(disable);

    if (disable)
    {
        currentView = nullptr;
    }
}

void AlgorithmPaintToolBox::setLabel(int newVal)
{
    QColor labelColor = m_labelColorMap[newVal-1].second;
    m_labelColorWidget->setStyleSheet("background-color: " + labelColor.name() + ";border:0;border-radius: 0px;width:20px;height:20px;");
}

void AlgorithmPaintToolBox::setLabelColor()
{
    QColor currentColor = m_labelColorMap[m_strokeLabelSpinBox->value() - 1].second;
    QColor newColor = QColorDialog::getColor(currentColor,this);

    if (newColor.isValid())
    {
        m_labelColorMap[m_strokeLabelSpinBox->value() - 1].second = newColor;
        if (m_maskAnnotationData)
        {
            m_maskAnnotationData->setColorMap(m_labelColorMap);
            m_maskAnnotationData->invokeModified();
        }

        this->setLabel(m_strokeLabelSpinBox->value());
    }
}

void AlgorithmPaintToolBox::setData( medAbstractData *medData )
{
    if (!medData)
    {
        return;
    }

    m_lastVup = QVector3D();
    m_lastVpn = QVector3D();

    m_imageData = medData;

    // Update values of slider

    GenerateMinMaxValuesFromImage < itk::Image <char,3> > ();
    GenerateMinMaxValuesFromImage < itk::Image <unsigned char,3> > ();
    GenerateMinMaxValuesFromImage < itk::Image <short,3> > ();
    GenerateMinMaxValuesFromImage < itk::Image <unsigned short,3> > ();
    GenerateMinMaxValuesFromImage < itk::Image <int,3> > ();
    GenerateMinMaxValuesFromImage < itk::Image <unsigned int,3> > ();
    GenerateMinMaxValuesFromImage < itk::Image <long,3> > ();
    GenerateMinMaxValuesFromImage < itk::Image <unsigned long,3> > ();
    GenerateMinMaxValuesFromImage < itk::Image <float,3> > ();
    GenerateMinMaxValuesFromImage < itk::Image <double,3> > ();

    if ( m_imageData )
    {
        medImageMaskAnnotationData *existingMaskAnnData = nullptr;
        for( medAttachedData *data : m_imageData->attachedData() )
        {

            if ( qobject_cast<medImageMaskAnnotationData*>(data) )
            {
                existingMaskAnnData =  qobject_cast<medImageMaskAnnotationData*>(data);
                break;
            }
        }

        if ( existingMaskAnnData )
        {
            m_maskAnnotationData = existingMaskAnnData;
            m_maskData = existingMaskAnnData->maskData();

        }
        else
        {
            m_maskData = medAbstractDataFactory::instance()->createSmartPointer( "itkDataImageUChar3" );

            if ( !m_maskData )
            {
                qDebug() << DTK_PRETTY_FUNCTION << "Failed to create itkDataImageUChar3";
                return;
            }

            m_maskAnnotationData = new medImageMaskAnnotationData;
            this->initializeMaskData( m_imageData, m_maskData );
            m_maskAnnotationData->setMaskData(qobject_cast<medAbstractImageData*>(m_maskData));

            m_maskAnnotationData->setColorMap( m_labelColorMap );

            m_imageData->addAttachedData(m_maskAnnotationData);
        }
    }

    if ( m_imageData )
    {
        m_itkMask = dynamic_cast<MaskType*>( reinterpret_cast<itk::Object*>(m_maskData->data()) );
        this->showButtons(true);
    }
    else
    {
        m_itkMask = nullptr;
        this->showButtons(false);
    }
}

void AlgorithmPaintToolBox::generateLabelColorMap(unsigned int numLabels)
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

void AlgorithmPaintToolBox::initializeMaskData( medAbstractData *imageData, medAbstractData *maskData )
{
    MaskType::Pointer mask = MaskType::New();

    Q_ASSERT(mask->GetImageDimension() == 3);

    medAbstractImageData * mImage = qobject_cast<medAbstractImageData*>(imageData);
    Q_ASSERT(mImage);

    MaskType::RegionType region;
    region.SetSize(0, ( mImage->Dimension() > 0 ? mImage->xDimension() : 1 ) );
    region.SetSize(1, ( mImage->Dimension() > 1 ? mImage->yDimension() : 1 ) );
    region.SetSize(2, ( mImage->Dimension() > 2 ? mImage->zDimension() : 1 ) );

    MaskType::DirectionType direction;
    MaskType::SpacingType spacing;
    MaskType::PointType origin;

    direction.Fill(0);
    spacing.Fill(0);
    origin.Fill(0);
    for (unsigned int i = 0;i < mask->GetImageDimension();++i)
    {
        direction(i,i) = 1;
    }

    unsigned int maxIndex = std::min<unsigned int>(mask->GetImageDimension(),mImage->Dimension());

    switch (mImage->Dimension())
    {
        case 2:
        {
            itk::ImageBase <2> * imageDataOb = dynamic_cast<itk::ImageBase <2> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

            for (unsigned int i = 0;i < maxIndex;++i)
            {
                for (unsigned int j = 0;j < maxIndex;++j)
                {
                    direction(i,j) = imageDataOb->GetDirection()(i,j);
                }
                spacing[i] = imageDataOb->GetSpacing()[i];
                origin[i] = imageDataOb->GetOrigin()[i];
            }

            break;
        }

        case 4:
        {
            itk::ImageBase <4> * imageDataOb = dynamic_cast<itk::ImageBase <4> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

            for (unsigned int i = 0;i < maxIndex;++i)
            {
                for (unsigned int j = 0;j < maxIndex;++j)
                {
                    direction(i,j) = imageDataOb->GetDirection()(i,j);
                }

                spacing[i] = imageDataOb->GetSpacing()[i];
                origin[i] = imageDataOb->GetOrigin()[i];
            }

            break;
        }

        default:
        {
            itk::ImageBase <3> * imageDataOb = dynamic_cast<itk::ImageBase <3> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

            for (unsigned int i = 0;i < maxIndex;++i)
            {
                for (unsigned int j = 0;j < maxIndex;++j)
                {
                    direction(i,j) = imageDataOb->GetDirection()(i,j);
                }

                spacing[i] = imageDataOb->GetSpacing()[i];
                origin[i] = imageDataOb->GetOrigin()[i];
            }

            break;
        }
    }

    mask->SetOrigin(origin);
    mask->SetDirection(direction);
    mask->SetSpacing(spacing);
    mask->SetLargestPossibleRegion(region);
    mask->SetBufferedRegion(region);
    mask->Allocate();
    mask->FillBuffer( MaskPixelValues::Unset );

    maskData->setData((QObject*)(mask.GetPointer()));
}

void AlgorithmPaintToolBox::updateWandRegion(medAbstractImageView *view, QVector3D &vec)
{
    setCurrentView(view);

    if ( !m_imageData && (m_maskAnnotationData != view->layerData(0)))
    {
        this->setData(view->layerData(0));
    }

    if (!m_imageData)
    {
        displayMessageError("Could not set data");
        return;
    }

    if ((m_imageData->identifier().contains("4"))||
            (m_imageData->identifier().contains("RGB"))||
            (m_imageData->identifier().contains("Vector"))||
            (m_imageData->identifier().contains("2")))
    {
        medMessageController::instance()->showError(tr("Magic wand option is only available for 3D images"),3000);
        return;
    }

    MaskType::IndexType index;

    bool isInside;
    unsigned int planeIndex = computePlaneIndex(vec,index,isInside);
    if (isInside)
    {
        RunConnectedFilter < itk::Image <char,3> > (index,planeIndex);
        RunConnectedFilter < itk::Image <unsigned char,3> > (index,planeIndex);
        RunConnectedFilter < itk::Image <short,3> > (index,planeIndex);
        RunConnectedFilter < itk::Image <unsigned short,3> > (index,planeIndex);
        RunConnectedFilter < itk::Image <int,3> > (index,planeIndex);
        RunConnectedFilter < itk::Image <unsigned int,3> > (index,planeIndex);
        RunConnectedFilter < itk::Image <long,3> > (index,planeIndex);
        RunConnectedFilter < itk::Image <unsigned long,3> > (index,planeIndex);
        RunConnectedFilter < itk::Image <float,3> > (index,planeIndex);
        RunConnectedFilter < itk::Image <double,3> > (index,planeIndex);
    }

    if(!view->contains(m_maskAnnotationData))
    {
        view->addLayer(m_maskAnnotationData);

        // Update Mouse Interaction ToolBox
        view->setCurrentLayer(0);
        getWorkspace()->updateMouseInteractionToolBox();
    }
}

template <typename IMAGE>
void
AlgorithmPaintToolBox::RunConnectedFilter (MaskType::IndexType &index, unsigned int planeIndex)
{
    IMAGE *tmpPtr = dynamic_cast<IMAGE *> ((itk::Object*)(m_imageData->data()));

    MaskType::PixelType pxValue = m_strokeLabelSpinBox->value();

    if (!tmpPtr)
    {
        return;
    }

    typedef itk::ConnectedThresholdImageFilter<IMAGE, MaskType> ConnectedThresholdImageFilterType;
    typename ConnectedThresholdImageFilterType::Pointer ctiFilter = ConnectedThresholdImageFilterType::New();

    double value = tmpPtr->GetPixel(index);
    if (!seedPlanted)
    {
        setSeedPlanted(true,index,planeIndex,value);
    }

    ctiFilter->SetUpper( m_wandUpperThresholdSlider->value() );
    ctiFilter->SetLower( m_wandLowerThresholdSlider->value() );

    MaskType::RegionType regionRequested = tmpPtr->GetLargestPossibleRegion();
    regionRequested.SetIndex(planeIndex, index[planeIndex]);
    regionRequested.SetSize(planeIndex, 1);
    MaskType::RegionType outRegion = regionRequested;
    outRegion.SetIndex(planeIndex,0);

    if (m_wand3DCheckbox->checkState() == Qt::Unchecked)
    {
        typename IMAGE::Pointer workPtr = IMAGE::New();
        workPtr->Initialize();
        workPtr->SetDirection(tmpPtr->GetDirection());
        workPtr->SetSpacing(tmpPtr->GetSpacing());
        workPtr->SetOrigin(tmpPtr->GetOrigin());
        workPtr->SetRegions(outRegion);
        workPtr->Allocate();

        itk::ImageRegionConstIterator < IMAGE > inputItr (tmpPtr, regionRequested);
        itk::ImageRegionIterator < IMAGE > workItr (workPtr, outRegion);

        while (!workItr.IsAtEnd())
        {
            workItr.Set(inputItr.Get());

            ++workItr;
            ++inputItr;
        }

        // For undo/redo purposes -------------------------
        QList<unsigned int> listIdSlice;
        listIdSlice.append(index[planeIndex]);
        addSliceToStack(currentView,planeIndex,listIdSlice);
        // -------------------------------------------------

        ctiFilter->SetInput( workPtr );
        index[planeIndex] = 0;
        ctiFilter->AddSeed( index );

        ctiFilter->Update();

        itk::ImageRegionConstIterator <MaskType> outFilterItr (ctiFilter->GetOutput(), outRegion);
        itk::ImageRegionIterator <MaskType> maskFilterItr (m_itkMask, regionRequested);

        while (!maskFilterItr.IsAtEnd())
        {
            if (outFilterItr.Get() != 0)
            {
                maskFilterItr.Set(pxValue);
            }

            ++outFilterItr;
            ++maskFilterItr;
        }
    }
    else
    {
        ctiFilter->SetInput( tmpPtr );
        ctiFilter->AddSeed( index );

        ctiFilter->Update();

        itk::ImageRegionConstIterator <MaskType> outFilterItr (ctiFilter->GetOutput(), tmpPtr->GetLargestPossibleRegion());
        itk::ImageRegionIterator <MaskType> maskFilterItr (m_itkMask, tmpPtr->GetLargestPossibleRegion());

        // For undo/redo purposes ------------------------- Save the current states of slices that are going to be modified by the segmentation
        unsigned int idSlice = index[planeIndex];
        QList<unsigned int> listIdSlice;
        listIdSlice.append(idSlice);

        while(!outFilterItr.IsAtEnd())
        {
            if (outFilterItr.Get() != 0)
            {
                MaskType::IndexType indexOutFilter = outFilterItr.GetIndex();
                // save the id of slices that are modified by the 3d mode
                if (idSlice!=indexOutFilter[planeIndex])
                {
                    idSlice = indexOutFilter[planeIndex];
                    listIdSlice.append(idSlice);
                }
            }
            ++outFilterItr;
        }
        addSliceToStack(currentView,planeIndex,listIdSlice);
        // -------------------------------------------------

        outFilterItr.GoToBegin();

        while (!maskFilterItr.IsAtEnd())
        {
            if (outFilterItr.Get() != 0)
            {
                maskFilterItr.Set(pxValue);
            }

            ++outFilterItr;
            ++maskFilterItr;
        }
    }

    m_itkMask->Modified();
    m_itkMask->GetPixelContainer()->Modified();
    m_itkMask->SetPipelineMTime(m_itkMask->GetMTime());

    m_maskAnnotationData->invokeModified();
}

template <typename IMAGE>
void
AlgorithmPaintToolBox::GenerateMinMaxValuesFromImage ()
{
    IMAGE *tmpPtr = dynamic_cast<IMAGE *> ((itk::Object*)(m_imageData->data()));

    if (!tmpPtr)
    {
        return;
    }

    typedef typename itk::MinimumMaximumImageCalculator< IMAGE > MinMaxCalculatorType;

    typename MinMaxCalculatorType::Pointer minMaxFilter = MinMaxCalculatorType::New();

    minMaxFilter->SetImage(tmpPtr);
    minMaxFilter->Compute();

    double m_MinValueImage = minMaxFilter->GetMinimum();
    double m_MaxValueImage = minMaxFilter->GetMaximum();

    m_wandLowerThresholdSlider->setRange(m_MinValueImage, m_MaxValueImage);
    m_wandUpperThresholdSlider->setRange(m_MinValueImage, m_MaxValueImage);
    // Try to keep the previously selected range: current min
    // and max values are kept if they both lie in the new min
    // and max.
    double currentMinValue = m_wandLowerThresholdSlider->value();
    double currentMaxValue = m_wandUpperThresholdSlider->value();
    if ((currentMinValue < m_MinValueImage || currentMinValue > m_MaxValueImage) ||
            (currentMaxValue < m_MinValueImage || currentMaxValue > m_MaxValueImage))
    {
        // reset to minimum image value
        m_wandLowerThresholdSlider->setValue(m_MinValueImage);
        m_wandUpperThresholdSlider->setValue(m_MinValueImage);
    }

    // Set step when click on slider
    m_wandLowerThresholdSlider->getSlider()->setPageStep((m_MaxValueImage-m_MinValueImage)/10);
    m_wandUpperThresholdSlider->getSlider()->setPageStep((m_MaxValueImage-m_MinValueImage)/10);
}

void AlgorithmPaintToolBox::updateStroke(ClickAndMoveEventFilter * filter, medAbstractImageView * view)
{
    setCurrentView(currentView);
    if ( !isMask2dOnSlice() )
    {
        return;
    }

    const double radius = m_brushSizeSlider->value(); // in image units.

    if ( !m_imageData )
    {
        this->setData(view->layerData(0));
    }
    if (!m_imageData)
    {
        qWarning() << "Could not set data";
        return;
    }

    QVector3D newPoint = filter->points().back();

    typedef  MaskType::DirectionType::InternalMatrixType::element_type ElemType;
    itk::Point< ElemType > centerPoint;
    centerPoint.SetElement(0, newPoint.x());
    centerPoint.SetElement(1, newPoint.y());
    centerPoint.SetElement(2, newPoint.z());

    const QVector3D vup = view->viewUp();
    const QVector3D vpn = view->viewPlaneNormal();

    vnl_vector_fixed<ElemType, 3> vecVup(vup.x(), vup.y(), vup.z() );
    vnl_vector_fixed<ElemType, 3> vecVpn(vpn.x(), vpn.y(), vpn.z() );
    vnl_vector_fixed<ElemType, 3> vecRight = vnl_cross_3d(vecVup,vecVpn);

    if ( vup != m_lastVup || vpn != m_lastVpn )
    {
        const MaskType::SpacingType & spacing = m_itkMask->GetSpacing();

        // Rows of the direction matrix are the directions of the image i,j,k pixel directions.
        const MaskType::DirectionType & direction = m_itkMask->GetDirection();

        // project spacing onto view.

        vnl_matrix_fixed<ElemType,2,3> projMatrix;
        projMatrix.set_row(0, vecRight );
        projMatrix.set_row(1, vecVup );

        projMatrix = projMatrix * direction.GetVnlMatrix(); // (direction.GetTranspose());

        double sampleSpacing[2];
        // Compute the total projection of each of the spacings onto the view plane x & y.
        for (int i = 0; i < 2; i++) //output axis
        {
            double s = 0;  // sum squares
            double r = 0;
            for (int j = 0; j < 3; j++)
            {
                const double elem = projMatrix.get(i,j);
                const double elem2 = elem*elem;
                s += elem2*(spacing[j] >= 0 ? spacing[j] : -spacing[j]);
                r += elem2;
            }
            s /= r;
            sampleSpacing[i] = s;
        }

        // Store result.
        std::copy( sampleSpacing, sampleSpacing + 2, m_sampleSpacing);

        //Oversample
        m_sampleSpacing[0] *= 0.5;
        m_sampleSpacing[1] *= 0.5;
        m_lastVup = vup;
        m_lastVpn = vpn;
    }

    const double radius2 = radius*radius;

    const int Nx = std::max( 1, (int)std::ceil(radius/m_sampleSpacing[0]) );
    const int Ny = std::max( 1, (int)std::ceil(radius/m_sampleSpacing[1]) );

    MaskType::PixelType pxValue;
    switch ( m_paintState )
    {
        case PaintState::Stroke :
            // color of master roi (defined by user)
            pxValue = m_strokeLabelSpinBox->value();
            break;
        default:
            pxValue = MaskPixelValues::Unset;
            break;
    }

    MaskType::IndexType index;
    itk::Point<ElemType,3> testPt;

    for ( int y(-Ny); y < Ny; ++y )
    {
        double dy = y*m_sampleSpacing[1];
        for ( int x(-Nx); x < Nx; ++x )
        {
            double dx = x*m_sampleSpacing[0];
            if ( dx*dx + dy*dy >= radius2 )
            {
                continue;
            }

            for ( int ic(0); ic<3; ++ic)
            {
                testPt[ic] = centerPoint[ic] + dx * vecRight[ic] + dy * vecVup[ic];
            }

            bool isInside = m_itkMask->TransformPhysicalPointToIndex( testPt, index );
            if ( isInside )
            {
                m_itkMask->SetPixel( index, pxValue );
            }
        }
    }
    m_itkMask->Modified();
    m_itkMask->GetPixelContainer()->Modified();
    m_itkMask->SetPipelineMTime(m_itkMask->GetMTime());

    if(!view->contains(m_maskAnnotationData))
    {
        m_maskAnnotationData->setMetaData("SeriesDescription", "mask");
        view->addLayer(m_maskAnnotationData);
        for(medAbstractInteractor* interactor : view->layerInteractors(getWorkspace()->getSelectedLayerIndices()[0]))
        {
            if (interactor->identifier() == "medAnnotationInteractor")
            {
                for(medAbstractParameterL* parameter : interactor->linkableParameters())
                {
                    if (parameter->name() == "Opacity")
                    {
                        qobject_cast<medDoubleParameterL*>(parameter)->setValue(0.4);
                    }
                }
            }
        }

        // Update Mouse Interaction ToolBox
        view->setCurrentLayer(0);
        getWorkspace()->updateMouseInteractionToolBox();
    }

    if ( slicingParameter )
    {
        slicingParameter->getSlider()->addTick(currentIdSlice);
        slicingParameter->getSlider()->update();
    }

    m_maskAnnotationData->invokeModified();
}

bool AlgorithmPaintToolBox::isMask2dOnSlice()
{
    MaskType::IndexType index3D;
    QVector3D vector = currentView->mapDisplayToWorldCoordinates(QPointF(0,0));
    bool isInside;
    int planeIndex = (int)AlgorithmPaintToolBox::computePlaneIndex(vector,index3D,isInside);
    MaskType::RegionType inputRegion = m_itkMask->GetLargestPossibleRegion();
    MaskType::SizeType   size      = inputRegion.GetSize();
    MaskType::IndexType  start     = inputRegion.GetIndex();

    Mask2dType::Pointer img = Mask2dType::New();
    img = extract2DImageSlice(m_itkMask, planeIndex, currentIdSlice, size, start);
    Mask2dIterator iterator(img,img->GetBufferedRegion());
    iterator.GoToBegin();
    typename Mask2dType::IndexType index2d;

    while(!iterator.IsAtEnd())
    {
        index2d = iterator.GetIndex();
        if ( img->GetPixel(index2d) == m_strokeLabelSpinBox->value() )
        {
            return true;
        }
        ++iterator;
    }

    if (m_paintState==PaintState::DeleteStroke)
    {
        if ( slicingParameter )
        {
            slicingParameter->getSlider()->removeTick(currentIdSlice);
            slicingParameter->getSlider()->update();
        }
        for (auto& pB : setOfPaintBrushRois)
        {
            if (pB->getIdSlice() == currentIdSlice)
            {
                setOfPaintBrushRois.erase(pB);
                break;
            }
        }
        return false;
    }
    return true;
}

void AlgorithmPaintToolBox::showButtons( bool value )
{
    if (value)
    {
        m_applyButton->show();
        m_clearMaskButton->show();
    }
    else
    {
        m_applyButton->hide();
        m_clearMaskButton->hide();
    }
}

void AlgorithmPaintToolBox::updateButtons()
{
    if ( m_paintState == PaintState::None )
    {
        m_wandLowerThresholdSlider->hide();
        m_wandUpperThresholdSlider->hide();
        m_wand3DCheckbox->hide();
        m_wand3DRealTime->hide();
        m_wandInfo->hide();
        m_brushSizeSlider->hide();
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

        if ( m_paintState == PaintState::Wand )
        {
            m_wandLowerThresholdSlider->show();
            m_wandUpperThresholdSlider->show();
            m_wand3DCheckbox->show();
            m_wand3DRealTime->show();
            m_wandInfo->show();
            m_brushSizeSlider->hide();
        }
        else if ( m_paintState == PaintState::Stroke )
        {
            m_brushSizeSlider->show();
            m_wandLowerThresholdSlider->hide();
            m_wandUpperThresholdSlider->hide();
            m_wand3DCheckbox->hide();
            m_wand3DRealTime->hide();
        }
    }
}

/** Apply the current interaction (paint, ...) to a new view */
void AlgorithmPaintToolBox::updateMouseInteraction()
{
    if (m_paintState != PaintState::None)
    {
        addViewEventFilter(m_viewFilter);
    }
}

dtkPlugin* AlgorithmPaintToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "Algorithm Paint" );
    return plugin;
}

void AlgorithmPaintToolBox::setParameter(int channel, int value)
{
    seedPlanted = false; //prevent from crash when data not set

    if(channel == 0)
    {
        m_wandLowerThresholdSlider->setValue(value);
    }
    if (channel == 1)
    {
        m_wandUpperThresholdSlider->setValue(value);
    }
}

void AlgorithmPaintToolBox::undo()
{
    if (!currentView ||  !m_undoStacks || !m_undoStacks->contains(currentView))
    {
        return;
    }
    QStack<PairListSlicePlaneId> * undo_stack = m_undoStacks->value(currentView);

    if (undo_stack->isEmpty())
    {
        return;
    }

    if (seedPlanted)
    {
        this->removeSeed();
        return;
    }

    if (!m_redoStacks->contains(currentView))
    {
        m_redoStacks->insert(currentView,new QStack<PairListSlicePlaneId>());
    }

    QStack<PairListSlicePlaneId> * redo_stack = m_redoStacks->value(currentView);

    PairListSlicePlaneId previousState = undo_stack->pop();
    unsigned char planeIndex = previousState.second;

    MaskType::RegionType requestedRegion = m_itkMask->GetLargestPossibleRegion();
    Mask2dType::IndexType index2d;
    Mask2dType::RegionType region;
    Mask2dType::RegionType::SizeType size;

    unsigned int i, j;
    char direction[2];
    for (i = 0, j = 0; i < 3; ++i )
    {
        if (i != planeIndex)
        {
            direction[j] = i;
            j++;
        }
    }

    index2d[ direction[0] ]    = requestedRegion.GetIndex()[ direction[0] ];
    index2d[ 1- direction[0] ] = requestedRegion.GetIndex()[ direction[1] ];
    size[ direction[0] ]     = requestedRegion.GetSize()[  direction[0] ];
    size[ 1- direction[0] ]  = requestedRegion.GetSize()[  direction[1] ];

    region.SetSize(size);
    region.SetIndex(index2d);

    PaintBrushSet list;
    for(auto& prev : previousState.first )
    {
        unsigned int idSlice = prev->getIdSlice();
        Mask2dType::Pointer currentSlice = Mask2dType::New();
        currentSlice->SetRegions(region);
        currentSlice->Allocate();
        copySliceFromMask3D(currentSlice, planeIndex, direction, idSlice, false);
        pasteSliceToMask3D(prev->getSlice(), planeIndex, direction, idSlice, false);
        list.insert(new medPaintBrush(currentSlice, idSlice, prev->isMasterRoi(), prev->getLabel() ));

        for (auto& pB : setOfPaintBrushRois)
        {
            if (pB->getIdSlice() == idSlice)
            {
                setOfPaintBrushRois.erase(pB);
                break;
            }
        }

        if (slicingParameter)
        {
            slicingParameter->getSlider()->removeTick(idSlice);
            slicingParameter->getSlider()->update();
        }
    }

    PairListSlicePlaneId currentState = PairListSlicePlaneId(list,planeIndex);
    redo_stack->append(currentState);

    m_itkMask->Modified();
    m_itkMask->GetPixelContainer()->Modified();
    m_itkMask->SetPipelineMTime(m_itkMask->GetMTime());
    m_maskAnnotationData->invokeModified();

    // No more painted data
    if (m_undoStacks->value(currentView)->isEmpty())
    {
        m_applyButton->setDisabled(true);
    }
}

void AlgorithmPaintToolBox::redo()
{
    if (!currentView ||  !m_redoStacks || !m_redoStacks->contains(currentView))
    {
        return;
    }

    QStack<PairListSlicePlaneId> *redo_stack = m_redoStacks->value(currentView);
    QStack<PairListSlicePlaneId> *undo_stack = m_undoStacks->value(currentView);

    if (redo_stack->isEmpty())
    {
        return;
    }

    // Save the current state in the undo_stack
    PairListSlicePlaneId nextState = redo_stack->pop();
    unsigned char planeIndex = nextState.second;
    MaskType::RegionType requestedRegion = m_itkMask->GetLargestPossibleRegion();
    Mask2dType::IndexType index2d;
    Mask2dType::RegionType region;
    Mask2dType::RegionType::SizeType size;

    unsigned int i, j;
    char direction[2];
    for (i = 0, j = 0; i < 3; ++i )
    {
        if (i != planeIndex)
        {
            direction[j] = i;
            j++;
        }
    }

    index2d[ direction[0] ]    = requestedRegion.GetIndex()[ direction[0] ];
    index2d[ 1- direction[0] ] = requestedRegion.GetIndex()[ direction[1] ];
    size[ direction[0] ]     = requestedRegion.GetSize()[  direction[0] ];
    size[ 1- direction[0] ]  = requestedRegion.GetSize()[  direction[1] ];

    region.SetSize(size);
    region.SetIndex(index2d);

    PaintBrushSet list;
    for(auto& next : nextState.first )
    {
        unsigned int idSlice = next->getIdSlice();
        Mask2dType::Pointer currentSlice = Mask2dType::New();
        currentSlice->SetRegions(region);
        currentSlice->Allocate();
        copySliceFromMask3D(currentSlice,planeIndex,direction,idSlice, false);
        pasteSliceToMask3D(next->getSlice(),nextState.second,direction,idSlice, false);
        list.insert(new medPaintBrush(currentSlice, idSlice, next->isMasterRoi(), next->getLabel() ));

        if (slicingParameter)
        {
            slicingParameter->getSlider()->addTick(idSlice);
            slicingParameter->getSlider()->update();
        }
    }

    PairListSlicePlaneId currentState = PairListSlicePlaneId(list,planeIndex);
    undo_stack->append(currentState);

    m_itkMask->Modified();
    m_itkMask->GetPixelContainer()->Modified();
    m_itkMask->SetPipelineMTime(m_itkMask->GetMTime());
    m_maskAnnotationData->invokeModified();
}

void AlgorithmPaintToolBox::addSliceToStack(medAbstractView *view, const unsigned char planeIndex, QList<unsigned int> listIdSlice, bool isMaster)
{
    // save the current state
    if (!currentView)
    {
        return;
    }

    if(!m_undoStacks->contains(view))
    {
        m_undoStacks->insert(view, new QStack<PairListSlicePlaneId>());
    }

    // copy code
    MaskType::RegionType requestedRegion = m_itkMask->GetLargestPossibleRegion();
    Mask2dType::IndexType index2d;
    Mask2dType::RegionType region;
    Mask2dType::RegionType::SizeType size;

    unsigned int i, j;
    char direction[2];
    for (i = 0, j = 0; i < 3; ++i )
    {
        if (i != planeIndex)
        {
            direction[j] = i;
            j++;
        }
    }

    index2d[ direction[0] ]    = requestedRegion.GetIndex()[ direction[0] ];
    index2d[ 1- direction[0] ] = requestedRegion.GetIndex()[ direction[1] ];
    size[ direction[0] ]     = requestedRegion.GetSize()[  direction[0] ];
    size[ 1- direction[0] ]  = requestedRegion.GetSize()[  direction[1] ];

    region.SetSize(size);
    region.SetIndex(index2d);

    PaintBrushSet setOfUndoRois;
    setOfUndoRois.clear();
    for(int i = 0; i<listIdSlice.size(); i++)
    {
        unsigned int idSlice = listIdSlice[i];
        for (auto& pB : setOfPaintBrushRois)
        {
            if (pB->getIdSlice() == idSlice)
            {
                setOfPaintBrushRois.erase(pB);
                break;
            }
        }
        Mask2dType::Pointer currentSlice = Mask2dType::New();
        currentSlice->SetRegions(region);
        currentSlice->Allocate();
        copySliceFromMask3D(currentSlice,planeIndex,direction,idSlice);
        setOfUndoRois.insert(new medPaintBrush(currentSlice, idSlice, isMaster, m_strokeLabelSpinBox->value()));

        slicingParameter->getSlider()->addTick(idSlice);
        slicingParameter->getSlider()->update();
    }

    setOfPaintBrushRois.insert(setOfUndoRois.begin(), setOfUndoRois.end());

    m_undoStacks->value(view)->append(PairListSlicePlaneId(setOfUndoRois,planeIndex));

    if (m_redoStacks->contains(view))
    {
        m_redoStacks->value(view)->clear();
    }
}

void AlgorithmPaintToolBox::clear()
{
    if (currentView && (currentView->layersCount() > 0))
    {
        // remove the "no name" mask data in the view
        for(int unsigned i=0; i<currentView->layersCount(); i++)
        {
            if (currentView->layerData(i)->identifier() == "medImageMaskAnnotationData")
            {
                currentView->removeLayer(i); // call clear() again
                break;
            }
        }
    }
    clearMask();
}

void AlgorithmPaintToolBox::clearMask()
{
    setOfPaintBrushRois.clear();
    if ( m_maskData && m_itkMask )
    {
        m_itkMask->FillBuffer( MaskPixelValues::Unset );
        m_itkMask->Modified();
        m_itkMask->GetPixelContainer()->Modified();
        m_itkMask->SetPipelineMTime(m_itkMask->GetMTime());

        m_maskAnnotationData->invokeModified();

        if(maskHasBeenSaved)
        {
            m_imageData->removeAttachedData(m_maskAnnotationData);
            maskHasBeenSaved = false;

            if (currentView && (currentView->layersCount()>0))
            {
                setData(currentView->layerData(0));
            }
        }
        m_applyButton->setDisabled(true);
        m_interpolateButton->setDisabled(true);
    }
    m_imageData = nullptr;

    if (m_undoStacks->value(currentView))
    {
        m_undoStacks->value(currentView)->clear();
        m_undoStacks->remove(currentView);
    }
    if (m_redoStacks->value(currentView))
    {
        m_redoStacks->value(currentView)->clear();
        m_redoStacks->remove(currentView);
    }

    showButtons(false);
    resetToolbox();
}

void AlgorithmPaintToolBox::resetToolbox()
{
    this->m_viewFilter->removeFromAllViews();

    if ( this->m_strokeButton->isChecked() )
    {
        m_paintState = (PaintState::None);
        updateButtons();
        m_strokeButton->setChecked(false);
    }
    else if ( this->m_magicWandButton->isChecked() )
    {
        m_paintState = (PaintState::None);
        newSeed(); // accept the current growth
        updateButtons();
        m_magicWandButton->setChecked(false);
    }

    deactivateCustomedCursor();
}

void AlgorithmPaintToolBox::setCurrentView(medAbstractImageView * view)
{
    currentView = view;

    if (!m_undoStacks->contains(currentView))
    {
        m_redoStacks->insert(currentView,new QStack<PairListSlicePlaneId>());
        m_undoStacks->insert(currentView,new QStack<PairListSlicePlaneId>());
    }
}

void AlgorithmPaintToolBox::addBrushSize(int size)
{
    if (m_paintState==PaintState::Stroke || m_paintState==PaintState::DeleteStroke)
    {
        m_brushSizeSlider->setValue(m_brushSizeSlider->value()+size);
        activateCustomedCursor(); // cursor for painting
    }
}

bool AlgorithmPaintToolBox::getSeedPlanted()
{
    return seedPlanted;
}

void AlgorithmPaintToolBox::setSeed(QVector3D point)
{
    m_seed = point;
}

void AlgorithmPaintToolBox::setSeedPlanted(bool val, MaskType::IndexType index,
                                           unsigned int planeIndex, double value)
{
    seedPlanted = val;
    if (val)
    {
        unsigned int i, j;
        char direction[2];
        for (i = 0, j = 0; i < 3; ++i )
        {
            if (i != planeIndex)
            {
                direction[j] = i;
                j++;
            }
        }

        m_wandInfo->setText("Seed X : " + QString::number(index[direction[0]]) + " Y : " + QString::number(index[direction[1]]) + " Slice : " + QString::number(index[planeIndex]+1) + " Value : " + QString::number(value));
        m_removeSeedButton->show();
        removeSeed_shortcut->setEnabled(true);
    }
}

void AlgorithmPaintToolBox::newSeed()
{
    seedPlanted = false;
    m_wandInfo->setText("Select a pixel in the image to plant the seed");
    m_removeSeedButton->hide();
    removeSeed_shortcut->setEnabled(false);
}

void AlgorithmPaintToolBox::removeSeed()
{
    newSeed(); // Accepting the growth will remove the seed.
    undo();
}

void AlgorithmPaintToolBox::copySliceMask()
{
    if (!currentView)
    {
        return;
    }

    if (m_copy.first)
    {
        m_copy.second = -1;
        m_copy.first = nullptr; // trigger the delete of the previous copy.
    }

    MaskType::IndexType index3D;
    QVector3D vec = currentView->mapDisplayToWorldCoordinates(QPointF(0,0));
    bool isInside;
    char planeIndex = AlgorithmPaintToolBox::computePlaneIndex(vec,index3D,isInside);

    if (planeIndex == -1)
    {
        return;
    }

    int slice = index3D[planeIndex];

    Mask2dType::RegionType region;
    Mask2dType::RegionType::SizeType size;
    Mask2dType::RegionType::IndexType index2d;

    MaskType::RegionType requestedRegion = m_itkMask->GetLargestPossibleRegion();

    unsigned int i, j;
    char direction[2];
    unsigned int planeInd = (unsigned int) planeIndex;
    for (i = 0, j = 0; i < 3; ++i )
    {
        if (i != planeInd)
        {
            direction[j] = i;
            j++;
        }
    }

    index2d[ direction[0] ]    = requestedRegion.GetIndex()[ direction[0] ];
    index2d[ 1- direction[0] ] = requestedRegion.GetIndex()[ direction[1] ];
    size[ direction[0] ]     = requestedRegion.GetSize()[  direction[0] ];
    size[ 1- direction[0] ]  = requestedRegion.GetSize()[  direction[1] ];

    region.SetSize(size);
    region.SetIndex(index2d);

    m_copy.first = Mask2dType::New();

    m_copy.first->SetRegions(region);
    m_copy.first->Allocate();
    copySliceFromMask3D(m_copy.first,planeIndex,direction,slice);

    m_copy.second = planeIndex;
    viewCopied = currentView;
}

void AlgorithmPaintToolBox::pasteSliceMask()
{
    if (!viewCopied || !currentView || currentView!=viewCopied || !m_copy.first || m_copy.second==-1)
    {
        return;
    }
    MaskType::IndexType index3D;
    QVector3D vec = currentView->mapDisplayToWorldCoordinates(QPointF(0,0));
    bool isInside;
    char planeIndex = AlgorithmPaintToolBox::computePlaneIndex(vec,index3D,isInside);

    if (planeIndex == -1 || planeIndex!=m_copy.second)
    {
        return;
    }
    unsigned int slice = index3D[planeIndex];

    unsigned int i, j;
    char direction[2];
    unsigned int planeInd = (unsigned int) planeIndex;
    for (i = 0, j = 0; i < 3; ++i )
    {
        if (i != planeInd)
        {
            direction[j] = i;
            j++;
        }
    }

    for (auto& pB : setOfPaintBrushRois)
    {
        if ( pB->getIdSlice() == slice)
        {
            pB->setMasterRoi(true);
            break;
        }

    }
    // For undo/redo purposes -------------------------
    QList<unsigned int> listIdSlice;
    listIdSlice.append(slice);
    addSliceToStack(currentView,planeIndex,listIdSlice);
    // -------------------------------------------------
    pasteSliceToMask3D(m_copy.first,planeIndex,direction,slice, true);

    m_itkMask->Modified();
    m_itkMask->GetPixelContainer()->Modified();
    m_itkMask->SetPipelineMTime(m_itkMask->GetMTime());
    m_maskAnnotationData->invokeModified();
}

char AlgorithmPaintToolBox::computePlaneIndex(const QVector3D & vec,MaskType::IndexType & index,bool & isInside)
{
    typedef  MaskType::DirectionType::InternalMatrixType::element_type ElemType;

    char planeIndex=-1;

    if (currentView && currentView->is2D())
    {
        const QVector3D vpn = currentView->viewPlaneNormal();

        const MaskType::DirectionType & direction = m_itkMask->GetDirection();
        vnl_vector_fixed<ElemType, 3> vecVpn(vpn.x(), vpn.y(), vpn.z() );

        MaskType::PointType point;

        point[0] = vec.x();
        point[1] = vec.y();
        point[2] = vec.z();

        isInside = m_itkMask->TransformPhysicalPointToIndex(point,index);

        double absDotProductMax = 0;
        planeIndex = 0;
        for (unsigned int i = 0;i < 3;++i)
        {
            double dotProduct = 0;
            for (unsigned int j = 0;j < 3;++j)
            {
                dotProduct += direction(j,i) * vecVpn[j];
            }

            if (fabs(dotProduct) > absDotProductMax)
            {
                planeIndex = i;
                absDotProductMax = fabs(dotProduct);
            }
        }
    }
    return planeIndex;
}

void AlgorithmPaintToolBox::copySliceFromMask3D(itk::Image<unsigned char,2>::Pointer copy, const char planeIndex,
                                                const char *direction, const unsigned int slice, bool becomesAMasterOne)
{
    typedef itk::ImageLinearIteratorWithIndex< itk::Image<unsigned char,2> > LinearIteratorType;
    typedef itk::ImageSliceIteratorWithIndex< MaskType> SliceIteratorType;

    SliceIteratorType  It3d( m_itkMask, m_itkMask->GetLargestPossibleRegion() );
    LinearIteratorType It2d( copy,copy->GetRequestedRegion() );

    It3d.SetFirstDirection( direction[1]);
    It3d.SetSecondDirection( direction[0] );

    It2d.SetDirection(1-direction[0]);

    It3d.GoToBegin();
    It2d.GoToBegin();

    while ( !It3d.IsAtEndOfSlice() )
    {
        while(It3d.GetIndex()[planeIndex] != slice && !It3d.IsAtEndOfSlice())
        {
            It3d.NextSlice();
        }

        while ( !It3d.IsAtEndOfLine() )
        {
            if (It3d.Get() == interpolatedMaskPixelValue && becomesAMasterOne)
            {
                It3d.Set(m_strokeLabelSpinBox->value());

            }
            It2d.Set(It3d.Get());
            ++It3d;
            ++It2d;
        }
        It2d.NextLine();
        It3d.NextLine();
    }
}

void AlgorithmPaintToolBox::pasteSliceToMask3D(const itk::Image<unsigned char,2>::Pointer image2D,
                                               const char planeIndex,const char * direction,
                                               const unsigned int slice, bool becomesAMasterOne)
{
    typedef itk::ImageLinearIteratorWithIndex< itk::Image<unsigned char,2> > LinearIteratorType;
    typedef itk::ImageSliceIteratorWithIndex< MaskType> SliceIteratorType;

    SliceIteratorType  It3d( m_itkMask, m_itkMask->GetLargestPossibleRegion() );
    LinearIteratorType It2d( image2D,image2D->GetRequestedRegion() );

    It3d.SetFirstDirection( direction[1]);
    It3d.SetSecondDirection( direction[0] );

    It2d.SetDirection(1-direction[0]);

    It3d.GoToBegin();
    It2d.GoToBegin();

    while ( !It3d.IsAtEndOfSlice() )
    {
        while(It3d.GetIndex()[planeIndex] != slice && !It3d.IsAtEndOfSlice())
        {
            It3d.NextSlice();
        }

        while ( !It3d.IsAtEndOfLine() )
        {
            if ( It2d.Get() == interpolatedMaskPixelValue && becomesAMasterOne)
            {
                It2d.Set(m_strokeLabelSpinBox->value());
            }
            It3d.Set(It2d.Get());
            ++It3d;
            ++It2d;
        }
        It2d.NextLine();
        It3d.NextLine();
    }
}

void AlgorithmPaintToolBox::deleteSliceFromMask3D(unsigned int sliceIndex)
{
    typedef itk::ImageSliceIteratorWithIndex< MaskType> SliceIteratorType;
    SliceIteratorType  It3d( m_itkMask, m_itkMask->GetLargestPossibleRegion() );
    char direction[2];
    unsigned int i, j;
    for ( i = 0, j = 0; i < 3; ++i )
    {
        if (i != currentPlaneIndex )
        {
            direction[j] = i;
            j++;
        }
    }
    It3d.SetFirstDirection( direction[1]);
    It3d.SetSecondDirection( direction[0] );

    It3d.GoToBegin();

    while ( !It3d.IsAtEndOfSlice() )
    {
        while(It3d.GetIndex()[currentPlaneIndex]!=sliceIndex && !It3d.IsAtEndOfSlice())
        {
            It3d.NextSlice();
        }

        while ( !It3d.IsAtEndOfLine() )
        {
            It3d.Set(MaskPixelValues::Unset);
            ++It3d;
        }
        It3d.NextLine();
    }
    if ( slicingParameter )
    {
        slicingParameter->getSlider()->removeTick(sliceIndex);
        slicingParameter->getSlider()->update();
    }
}

void AlgorithmPaintToolBox::increaseBrushSize()
{
    addBrushSize(1);
    if (!currentView)
    {
        return;
    }
}

void AlgorithmPaintToolBox::reduceBrushSize()
{
    addBrushSize(-1);
    if (!currentView)
    {
        return;
    }
}

void AlgorithmPaintToolBox::interpolate()
{
    if(!m_itkMask)
    {
        return;
    }
    this->setToolBoxOnWaitStatusForNonRunnableProcess();

    std::vector<std::pair<unsigned int, int>> masterRois;
    for(auto it = setOfPaintBrushRois.begin(); it != setOfPaintBrushRois.end(); )
    {
        auto& pB = *it;
        if( pB->isMasterRoi() == false )
        {
            deleteSliceFromMask3D(pB->getIdSlice());
            it = setOfPaintBrushRois.erase(it);
        }
        else
        {
            masterRois.push_back(std::pair<unsigned int, int>(pB->getIdSlice(), pB->getLabel()));
            ++it;
        }
    }
    if (!masterRois.empty())
    {
        unsigned int firstSlice, secondSlice;
        int firstLabel, secondLabel;
        for (auto it = masterRois.begin(); it != masterRois.end() - 1; )
        {
            firstSlice = (*it).first;
            firstLabel = (*it).second;
            advance(it, 1);
            secondSlice = (*it).first;
            secondLabel = (*it).second;
            if (firstLabel == secondLabel)
            {
                currentSliceLabel = firstLabel;
                interpolateBetween2PaintBrush(firstSlice, secondSlice);
            }
        }
    }
    this->setToolBoxOnReadyToUse();
}

void AlgorithmPaintToolBox::interpolateBetween2PaintBrush(unsigned int firstSlice, unsigned int secondSlice)
{
    MaskType::IndexType index3D;
    QVector3D vector = currentView->mapDisplayToWorldCoordinates(QPointF(0,0));
    bool isInside;
    char planeIndex = AlgorithmPaintToolBox::computePlaneIndex(vector,index3D,isInside);

    if (planeIndex != 2)
    {
        qDebug()<<"Interpolation is only available in sagittal view";
        return;
    }

    MaskType::RegionType inputRegion = m_itkMask->GetLargestPossibleRegion();
    MaskType::SizeType   size      = inputRegion.GetSize();
    MaskType::IndexType  start     = inputRegion.GetIndex();

    MaskFloatType::Pointer volumOut = MaskFloatType::New();
    MaskType::RegionType region;
    region.SetSize( size );
    region.SetIndex( start );
    volumOut->SetRegions( region );
    volumOut->Allocate();

    //Create output iterator
    MaskFloatIterator itVolumOut(volumOut, volumOut->GetBufferedRegion());
    itVolumOut.GoToBegin();

    //Create image iterator
    MaskIterator itMask(m_itkMask, m_itkMask->GetBufferedRegion());
    itMask.GoToBegin();

    Mask2dType::Pointer      img0              = Mask2dType::New();
    Mask2dType::Pointer      img1              = Mask2dType::New();
    Mask2dType::Pointer      img0tr            = Mask2dType::New();
    Mask2dType::Pointer      img1tr            = Mask2dType::New();
    Mask2dFloatType::Pointer distanceMapImg0   = Mask2dFloatType::New();
    Mask2dFloatType::Pointer distanceMapImg1   = Mask2dFloatType::New();

    unsigned int center[2]={(unsigned int)size[0]/2u, (unsigned int)size[1]/2u};
    img1 = extract2DImageSlice(m_itkMask, 2, secondSlice, size, start);
    img0 = extract2DImageSlice(m_itkMask, 2, firstSlice, size, start);
    unsigned int coord0[2], coord1[2];
    double vec[2];

    if(secondSlice - firstSlice > 1)
    {
        Mask2dIterator iterator0(img0,img0->GetBufferedRegion()); //Create image iterator
        iterator0.GoToBegin();
        Mask2dIterator iterator1(img1,img1->GetBufferedRegion()); //Create image iterator
        iterator1.GoToBegin();

        computeCentroid(iterator0, coord0);
        computeCentroid(iterator1, coord1);

        int C0C1[2]     = {(int)(coord1[0]-coord0[0]), (int)(coord1[1]-coord0[1])};
        int C0center[2] = {(int)(center[0]-coord0[0]), (int)(center[1]-coord0[1])};
        int C1center[2] = {(int)(center[0]-coord1[0]), (int)(center[1]-coord1[1])};

        img0tr = translateImageByVec(img0, C0center);
        img1tr = translateImageByVec(img1, C1center);

        distanceMapImg0 = computeDistanceMap(img0tr);
        distanceMapImg1 = computeDistanceMap(img1tr);

        // For undo/redo purposes
        QList<unsigned int> listIdSlice;
        for (unsigned int j=firstSlice+1; j<secondSlice; ++j) // for each intermediate slice
        {
            listIdSlice.append(j);
        }
        addSliceToStack(currentView, planeIndex, listIdSlice, false);

        for (unsigned int j=firstSlice+1; j<secondSlice; ++j) // for each intermediate slice
        {
            // Interpolate the "j" intermediate slice and copy into output volume
            vec[0]= (((j-firstSlice)*(C0C1[0]/(float)(secondSlice-firstSlice))+coord0[0])-center[0]);
            vec[1]= (((j-firstSlice)*(C0C1[1]/(float)(secondSlice-firstSlice))+coord0[1])-center[1]);
            computeIntermediateSlice(distanceMapImg0, distanceMapImg1,firstSlice,secondSlice, j,itVolumOut,itMask,vec);
        }
    }
}

Mask2dType::Pointer AlgorithmPaintToolBox::extract2DImageSlice(MaskType::Pointer input,
                                                               int plane,
                                                               int slice,
                                                               MaskType::SizeType size,
                                                               MaskType::IndexType start
                                                               )
{
    size[plane] = 0;
    const unsigned int sliceNumber = slice;
    start[plane] = sliceNumber;

    // create the desired region
    MaskType::RegionType desiredRegion;
    desiredRegion.SetSize(size);
    desiredRegion.SetIndex(start);
    typedef itk::ExtractImageFilter < MaskType, Mask2dType > Extract2DType;

    // associate previous sequence and desired region
    Extract2DType::Pointer filter = Extract2DType::New();
    filter->SetExtractionRegion(desiredRegion);
    filter->SetInput(input);
    filter->SetDirectionCollapseToGuess();
    filter->Update();

    // extract the image
    return filter->GetOutput();
}

Mask2dFloatType::Pointer AlgorithmPaintToolBox::computeDistanceMap(Mask2dType::Pointer img)
{
    typedef itk::DanielssonDistanceMapImageFilter<Mask2dType, Mask2dFloatType> DistanceMapImageFilterType;
    typedef itk::SubtractImageFilter<Mask2dFloatType, Mask2dFloatType> SubstractImageFilterType;

    DistanceMapImageFilterType::Pointer distMapFilter1 = DistanceMapImageFilterType::New();
    DistanceMapImageFilterType::Pointer distMapFilter2 = DistanceMapImageFilterType::New();

    SubstractImageFilterType::Pointer substractImageFilter = SubstractImageFilterType::New();

    typedef itk::InvertIntensityImageFilter<Mask2dType,Mask2dType> invertFilterType;
    invertFilterType::Pointer invertFilter = invertFilterType::New();

    invertFilter->SetMaximum(currentSliceLabel);
    invertFilter->SetInput(img);
    invertFilter->Update();

    distMapFilter1->SetInput(invertFilter->GetOutput());
    distMapFilter1->Update();
    substractImageFilter->SetInput1(distMapFilter1->GetOutput());

    distMapFilter2->SetInput(img);
    distMapFilter2->Update();

    substractImageFilter->SetInput2(distMapFilter2->GetOutput());
    substractImageFilter->Update();
    return substractImageFilter->GetOutput();
}

void AlgorithmPaintToolBox::computeCentroid(Mask2dIterator itmask, unsigned int *coord)
{
    itmask.GoToBegin();

    coord[0] = 0;
    coord[1] = 0;
    unsigned cpt = 0;

    while(!itmask.IsAtEnd())
    {
        if (itmask.Get() == currentSliceLabel)
        {
            coord[0] += itmask.GetIndex()[0];
            coord[1] += itmask.GetIndex()[1];
            cpt++;
        }
        ++itmask;
    }
    if (cpt>0)
    {
        coord[0] /= cpt;
        coord[1] /= cpt;
    }
}

Mask2dType::Pointer AlgorithmPaintToolBox::translateImageByVec(Mask2dType::Pointer img, int *vec)
{
    Mask2dType::RegionType inputRegion = img->GetLargestPossibleRegion();
    Mask2dType::SizeType   size        = inputRegion.GetSize();
    Mask2dType::IndexType  ind         = inputRegion.GetIndex();

    Mask2dType::Pointer imgOut = Mask2dType::New();
    Mask2dType::RegionType region;
    region.SetSize( size );
    region.SetIndex( ind );
    imgOut->SetRegions( region );
    imgOut->Allocate();

    Mask2dIterator it1(img, img->GetBufferedRegion()); // volume in 0
    it1.GoToBegin();
    Mask2dIterator it2(imgOut, imgOut->GetBufferedRegion()); // volume in 0
    it2.GoToBegin();

    while(!it2.IsAtEnd())
    {
        it2.Set(0);
        ++it2;
    }

    while(!it1.IsAtEnd())
    {
        if (it1.Get() == currentSliceLabel)
        {
            ind = it1.GetIndex();
            ind[0] = ind[0]+floor(vec[0]+0.5);
            ind[1] = ind[1]+floor(vec[1]+0.5);
            it2.SetIndex(ind);
            it2.Set(currentSliceLabel);
        }
        ++it1;
    }
    return imgOut;
}

// Compute the interpolated slice between two distance maps
void AlgorithmPaintToolBox::computeIntermediateSlice(Mask2dFloatType::Pointer distanceMapImg0,
                                                     Mask2dFloatType::Pointer distanceMapImg1,
                                                     unsigned int slice0,
                                                     unsigned int slice1,
                                                     int j,
                                                     MaskFloatIterator ito,
                                                     MaskIterator itmask,
                                                     double *vec)
{
    // iterators
    Mask2dFloatIterator iti0(distanceMapImg0, distanceMapImg0->GetBufferedRegion()); // volume in 0
    iti0.GoToBegin();

    Mask2dFloatIterator iti1(distanceMapImg1, distanceMapImg1->GetBufferedRegion()); // volume in 1
    iti1.GoToBegin();

    // In order to copy the connected component map distance of this slice into the volume
    MaskType::IndexType start;
    MaskType::IndexType other;
    start[0] = 0;
    start[1] = 0;
    start[2] = j;
    ito.SetIndex(start);
    itmask.SetIndex(start);

    // For each pixel of the InterpolatedSlice image, compute the value
    float interpolatVal;
    int nbinterslice = slice1-slice0;

    while(!iti0.IsAtEnd())
    {
        interpolatVal = ((slice1-j)*iti0.Get()+(j-slice0)*iti1.Get())/(float)(nbinterslice);

        ito.Set(interpolatVal);

        if (ito.Get()>=0)
        {
            start = itmask.GetIndex();
            other = itmask.GetIndex();
            other[0]=other[0]+floor(vec[0]+0.5);
            other[1]=other[1]+floor(vec[1]+0.5);
            other[2]=j;
            itmask.SetIndex(other);
            itmask.Set(interpolatedMaskPixelValue);
            itmask.SetIndex(start);
        }

        ++iti0;
        ++iti1;
        ++ito;
        ++itmask;
    }
}

void AlgorithmPaintToolBox::addViewEventFilter( medViewEventFilter *filter)
{
    for(QUuid uuid : this->getWorkspace()->tabbedViewContainers()->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        if(container)
        {
            filter->installOnView(container->view());
        }
    }
}

}
