/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <msegAlgorithmPaintToolbox.h>

#include <medAbstractData.h>
#include <medAbstractImageData.h>
#include <medAbstractImageView.h>
#include <medDataIndex.h>
#include <medImageMaskAnnotationData.h>
#include <medMetaDataKeys.h>
#include <medMessageController.h>
#include <medSegmentationSelectorToolBox.h>
#include <medViewManager.h>
#include <medPluginManager.h>
#include <medDataManager.h>

#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkLog/dtkLog.h>
#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkGlobal.h>

#include <vnl/vnl_cross.h>
#include <vnl/vnl_vector.h>

#include <itkImageRegionIterator.h>
#include <itkConnectedThresholdImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>

#include <QtCore>
#include <QColorDialog>

#include <algorithm>
#include <set>


class ClickAndMoveEventFilter : public medViewEventFilter
{
public:
    ClickAndMoveEventFilter(AlgorithmPaintToolbox *cb  = NULL) :
        medViewEventFilter(),
        m_cb(cb),
        m_paintState(PaintState::None),
        m_lastPaintState(PaintState::None)
    {}

    virtual bool mousePressEvent(medAbstractView *view, QMouseEvent *mouseEvent )
    {
        m_paintState = m_cb->paintState();

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
            m_paintState = m_cb->paintState(); //update paintState
        }

        mouseEvent->accept();

        medAbstractImageView * imageView = dynamic_cast<medAbstractImageView *>(view);
        if(!imageView)
            return false;

        // let's take the first non medImageMaskAnnotationData as the reference data
        // TODO: to improve...
        foreach(medAbstractData * data, imageView->dataList())
        {
            medImageMaskAnnotationData * existingMaskAnnData = dynamic_cast<medImageMaskAnnotationData *>(data);
            if(!existingMaskAnnData)
            {
                m_cb->setData( data );
                break;
            }
        }

        if (imageView->is2D())
        {
            // Convert mouse click to a 3D point in the image.
            QVector3D posImage = imageView->mapDisplayToWorldCoordinates( mouseEvent->posF() );

            if (m_paintState != PaintState::Wand)
            {
                this->m_points.push_back(posImage);
            }
            else
            {
                m_cb->updateWandRegion(imageView, posImage);
                m_paintState = PaintState::None; //Wand operation is over
            }
        }
        return mouseEvent->isAccepted();
    }

    virtual bool mouseMoveEvent( medAbstractView *view, QMouseEvent *mouseEvent )
    {
        medAbstractImageView * imageView = dynamic_cast<medAbstractImageView *>(view);
        if(!imageView)
            return false;

        if ( this->m_paintState == PaintState::None )
            return false;

        mouseEvent->accept();

        if (imageView->is2D())
        {
            QVector3D posImage = imageView->mapDisplayToWorldCoordinates( mouseEvent->posF() );
            //Project vector onto plane
            this->m_points.push_back(posImage);
            m_cb->updateStroke( this,imageView );
        }
        return mouseEvent->isAccepted();
    }

    virtual bool mouseReleaseEvent( medAbstractView *view, QMouseEvent *mouseEvent )
    {
        medAbstractImageView * imageView = dynamic_cast<medAbstractImageView *>(view);
        if(!imageView)
            return false;

        if ( this->m_paintState == PaintState::None )
            return false;
        m_paintState = PaintState::None; //Painting is done
        m_cb->updateStroke(this, imageView);
        this->m_points.clear();
        return true;
    }

    const std::vector<QVector3D> & points() const { return m_points; }

private :
    AlgorithmPaintToolbox *m_cb;
    std::vector<QVector3D> m_points;
    PaintState::E m_paintState;
    PaintState::E m_lastPaintState;
};




AlgorithmPaintToolbox::AlgorithmPaintToolbox(QWidget *parent ) :
    medSegmentationAbstractToolBox( parent),
    m_MinValueImage(0),
    m_MaxValueImage(500),
    m_strokeRadius(4),
    m_strokeLabel(1),
    m_paintState(PaintState::None)
{
    QWidget *displayWidget = new QWidget(this);
    this->addWidget(displayWidget);

    this->setTitle(this->s_name(this));

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

    m_applyButton = new QPushButton( tr("Save") , displayWidget);
    m_applyButton->setToolTip(tr("Save result to the Database"));

    m_clearMaskButton = new QPushButton( tr("Clear Mask") , displayWidget);
    m_clearMaskButton->setToolTip(tr("Resets the mask."));
    QHBoxLayout * dataButtonsLayout = new QHBoxLayout();
    dataButtonsLayout->addWidget(m_applyButton);
    dataButtonsLayout->addWidget(m_clearMaskButton);
    layout->addLayout(dataButtonsLayout);

    connect (m_strokeButton,     SIGNAL(pressed()),
             this, SLOT(activateStroke ()));

    connect (m_magicWandButton, SIGNAL(pressed()),
             this,SLOT(activateMagicWand()));

    connect (m_clearMaskButton,     SIGNAL(pressed()),
             this, SLOT(clearMask()));

    connect (m_applyButton,     SIGNAL(pressed()),
             this, SLOT(import()));

    connect (medViewManager::instance(), SIGNAL(viewOpened()),
             this, SLOT(updateMouseInteraction()));

    showButtons(false);
}

AlgorithmPaintToolbox::~AlgorithmPaintToolbox()
{
}

void AlgorithmPaintToolbox::setWandSliderValue(double val)
{
    double perc = 4000.0 * val / (m_MaxValueImage - m_MinValueImage);

    unsigned int percRound = (unsigned int)floor(perc);

    m_wandThresholdSizeSlider->blockSignals(true);
    m_wandThresholdSizeSlider->setValue(percRound);
    m_wandThresholdSizeSlider->blockSignals(false);
}

void AlgorithmPaintToolbox::setWandSpinBoxValue(int val)
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

void AlgorithmPaintToolbox::activateStroke()
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
    m_viewFilter = ( new ClickAndMoveEventFilter(this) );
    emit installEventFilterRequest(m_viewFilter);
}

void AlgorithmPaintToolbox::activateMagicWand()
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
    m_viewFilter = ( new ClickAndMoveEventFilter(this) );
    emit installEventFilterRequest(m_viewFilter);
}

void AlgorithmPaintToolbox::import()
{
    setOutputMetadata(m_imageData, m_maskData);
    medDataManager::instance()->import(m_maskData);
}

void AlgorithmPaintToolbox::setLabel(int newVal)
{
    QColor labelColor = m_labelColorMap[newVal-1].second;
    m_labelColorWidget->setStyleSheet("background-color: " + labelColor.name() + ";border:0;border-radius: 0px;width:20px;height:20px;");
}

void AlgorithmPaintToolbox::setLabelColor()
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

void AlgorithmPaintToolbox::clearMask()
{
    if ( m_maskData && m_itkMask ){
        m_itkMask->FillBuffer( medSegmentationSelectorToolBox::MaskPixelValues::Unset );
        m_itkMask->Modified();
        m_itkMask->GetPixelContainer()->Modified();
        m_itkMask->SetPipelineMTime(m_itkMask->GetMTime());

        m_maskAnnotationData->invokeModified();
    }
}

void AlgorithmPaintToolbox::setData( medAbstractData *medData )
{
    if (!medData)
        return;

    // disconnect existing
    if ( m_imageData )
    {
        // TODO?
    }

    m_lastVup = QVector3D();
    m_lastVpn = QVector3D();

    m_imageData = dtkSmartPointer<medAbstractData>(medData);

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

    if ( m_imageData ) {
        medImageMaskAnnotationData * existingMaskAnnData = NULL;
        foreach( medAttachedData * data, m_imageData->attachedData() ) {

            if ( qobject_cast<medImageMaskAnnotationData*>(data) ) {
                existingMaskAnnData =  qobject_cast<medImageMaskAnnotationData*>(data);
                break;
            }
        }

        if ( existingMaskAnnData ) {

            m_maskAnnotationData = existingMaskAnnData;
            m_maskData = existingMaskAnnData->maskData();

        } else {

            m_maskData =
                    medAbstractDataFactory::instance()->createSmartPointer( "itkDataImageUChar3" );

            if ( !m_maskData ) {
                dtkDebug() << DTK_PRETTY_FUNCTION << "Failed to create itkDataImageUChar3";
                return;
            }

            m_maskAnnotationData = new medImageMaskAnnotationData;
            this->initializeMaskData( m_imageData, m_maskData );
            m_maskAnnotationData->setMaskData(qobject_cast<medAbstractImageData*>(m_maskData));

            m_maskAnnotationData->setColorMap( m_labelColorMap );

            m_imageData->addAttachedData(m_maskAnnotationData);
        }
    }

    if ( m_imageData ) {
        m_itkMask = dynamic_cast<MaskType*>( reinterpret_cast<itk::Object*>(m_maskData->data()) );
        this->showButtons(true);
    } else {
        m_itkMask = NULL;
        this->showButtons(false);
    }
}

void AlgorithmPaintToolbox::generateLabelColorMap(unsigned int numLabels)
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

//static
medSegmentationAbstractToolBox *
AlgorithmPaintToolbox::createInstance(QWidget *parent )
{
    return new AlgorithmPaintToolbox( parent );
}

QString AlgorithmPaintToolbox::s_description()
{
    static const QString desc = "Paint Tool";
    return desc;
}

QString AlgorithmPaintToolbox::s_identifier()
{
    static const QString id = "mseg::AlgorithmPaintToolbox";
    return id;
}

QString AlgorithmPaintToolbox::s_name(const QObject * trObj)
{
    if (!trObj)
        trObj = qApp;

    return trObj->tr( "Paint Segmentation" );
}


void AlgorithmPaintToolbox::initializeMaskData( medAbstractData * imageData, medAbstractData * maskData )
{
    MaskType::Pointer mask = MaskType::New();

    Q_ASSERT(mask->GetImageDimension() == 3);

    medAbstractImageData * mImage = qobject_cast<medAbstractImageData*>(imageData);
    Q_ASSERT(mImage);
    //Q_ASSERT(mask->GetImageDimension() >= mImage->Dimension());

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
        direction(i,i) = 1;

    unsigned int maxIndex = std::min<unsigned int>(mask->GetImageDimension(),mImage->Dimension());

    switch (mImage->Dimension())
    {
    case 2:
    {
        itk::ImageBase <2> * imageDataOb = dynamic_cast<itk::ImageBase <2> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

        for (unsigned int i = 0;i < maxIndex;++i)
        {
            for (unsigned int j = 0;j < maxIndex;++j)
                direction(i,j) = imageDataOb->GetDirection()(i,j);

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
                direction(i,j) = imageDataOb->GetDirection()(i,j);

            spacing[i] = imageDataOb->GetSpacing()[i];
            origin[i] = imageDataOb->GetOrigin()[i];
        }

        break;
    }

    case 3:
    default:
    {
        itk::ImageBase <3> * imageDataOb = dynamic_cast<itk::ImageBase <3> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

        for (unsigned int i = 0;i < maxIndex;++i)
        {
            for (unsigned int j = 0;j < maxIndex;++j)
                direction(i,j) = imageDataOb->GetDirection()(i,j);

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
    mask->FillBuffer( medSegmentationSelectorToolBox::MaskPixelValues::Unset );

    maskData->setData((QObject*)(mask.GetPointer()));
}

void AlgorithmPaintToolbox::updateWandRegion(medAbstractImageView * view, QVector3D &vec)
{
    this->updateFromGuiItems();

    if ( !m_imageData )
    {
        this->setData(view->layerData(view->currentLayer()));
    }
    if (!m_imageData) {
        dtkWarn() << "Could not set data";
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

    const QVector3D vpn = view->viewPlaneNormal();

    const MaskType::DirectionType & direction = m_itkMask->GetDirection();

    typedef  MaskType::DirectionType::InternalMatrixType::element_type ElemType;
    vnl_vector_fixed<ElemType, 3> vecVpn(vpn.x(), vpn.y(), vpn.z() );

    double absDotProductMax = 0;
    unsigned int planeIndex = 0;
    for (unsigned int i = 0;i < 3;++i)
    {
        double dotProduct = 0;
        for (unsigned int j = 0;j < 3;++j)
            dotProduct += direction(j,i) * vecVpn[j];

        if (fabs(dotProduct) > absDotProductMax)
        {
            planeIndex = i;
            absDotProductMax = fabs(dotProduct);
        }
    }

    MaskType::PointType point;
    MaskType::IndexType index;

    point[0] = vec.x();
    point[1] = vec.y();
    point[2] = vec.z();

    bool isInside = m_itkMask->TransformPhysicalPointToIndex (point, index);

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
}

template <typename IMAGE>
void
AlgorithmPaintToolbox::RunConnectedFilter (MaskType::IndexType &index, unsigned int planeIndex)
{
    IMAGE *tmpPtr = dynamic_cast<IMAGE *> ((itk::Object*)(m_imageData->data()));

    MaskType::PixelType pxValue = m_strokeLabel;

    if (!tmpPtr)
        return;

    typedef itk::ConnectedThresholdImageFilter<IMAGE, MaskType> ConnectedThresholdImageFilterType;
    typename ConnectedThresholdImageFilterType::Pointer ctiFilter = ConnectedThresholdImageFilterType::New();

    double value = tmpPtr->GetPixel(index);

    ctiFilter->SetUpper( value + m_wandRadius );
    ctiFilter->SetLower( value - m_wandRadius );

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

        ctiFilter->SetInput( workPtr );
        index[planeIndex] = 0;
        ctiFilter->AddSeed( index );

        ctiFilter->Update();

        itk::ImageRegionConstIterator <MaskType> outFilterItr (ctiFilter->GetOutput(), outRegion);
        itk::ImageRegionIterator <MaskType> maskFilterItr (m_itkMask, regionRequested);
        while (!maskFilterItr.IsAtEnd())
        {
            if (outFilterItr.Get() != 0)
                maskFilterItr.Set(pxValue);

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
        while (!maskFilterItr.IsAtEnd())
        {
            if (outFilterItr.Get() != 0)
                maskFilterItr.Set(pxValue);

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
AlgorithmPaintToolbox::GenerateMinMaxValuesFromImage ()
{
    IMAGE *tmpPtr = dynamic_cast<IMAGE *> ((itk::Object*)(m_imageData->data()));

    if (!tmpPtr)
        return;

    typedef typename itk::MinimumMaximumImageCalculator< IMAGE > MinMaxCalculatorType;

    typename MinMaxCalculatorType::Pointer minMaxFilter = MinMaxCalculatorType::New();

    minMaxFilter->SetImage(tmpPtr);
    minMaxFilter->Compute();

    m_MinValueImage = minMaxFilter->GetMinimum();
    m_MaxValueImage = minMaxFilter->GetMaximum();

    this->setWandSpinBoxValue(m_wandThresholdSizeSlider->value());
}

void AlgorithmPaintToolbox::updateStroke(ClickAndMoveEventFilter * filter, medAbstractImageView * view)
{
    this->updateFromGuiItems();

    const double radius = m_strokeRadius; // in image units.

    if ( !m_imageData ) {
        this->setData(view->layerData(view->currentLayer()));
    }
    if (!m_imageData) {
        dtkWarn() << "Could not set data";
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

    if ( vup != m_lastVup || vpn != m_lastVpn ) {
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
        pxValue = m_strokeLabel;
        break;
    default:
        pxValue = medSegmentationSelectorToolBox::MaskPixelValues::Unset;
        break;
    }

    MaskType::IndexType index;
    itk::Point<ElemType,3> testPt;

    for ( int y(-Ny); y < Ny; ++y ) {
        double dy = y*m_sampleSpacing[1];
        for ( int x(-Nx); x < Nx; ++x ) {
            double dx = x*m_sampleSpacing[0];
            if ( dx*dx + dy*dy > radius2 )
                continue;

            for ( int ic(0); ic<3; ++ic) {
                testPt[ic] = centerPoint[ic] + dx * vecRight[ic] + dy * vecVup[ic];
            }

            bool isInside = m_itkMask->TransformPhysicalPointToIndex( testPt, index );
            if ( isInside ) {
                m_itkMask->SetPixel( index, pxValue );
            }
        }
    }
    m_itkMask->Modified();
    m_itkMask->GetPixelContainer()->Modified();
    m_itkMask->SetPipelineMTime(m_itkMask->GetMTime());

    if(!view->contains(m_maskAnnotationData))
    {
        view->addLayer(m_maskAnnotationData);
        setOutputMetadata(m_imageData, m_maskData);
        medDataManager::instance()->importNonPersistent(m_maskData);
    }

    m_maskAnnotationData->invokeModified();
}

void AlgorithmPaintToolbox::updateFromGuiItems()
{
    this->m_strokeRadius = m_brushSizeSlider->value();
    this->m_strokeLabel = m_strokeLabelSpinBox->value();
    this->m_wandRadius = m_wandThresholdSizeSpinBox->value();
}

void AlgorithmPaintToolbox::showButtons( bool value )
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

void AlgorithmPaintToolbox::updateButtons()
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


void AlgorithmPaintToolbox::updateMouseInteraction() //Apply the current interaction (paint, ...) to a new view
{
    if (m_paintState != PaintState::None)
    {
        m_viewFilter = ( new ClickAndMoveEventFilter(this) );
        emit installEventFilterRequest(m_viewFilter);
    }
}


dtkPlugin* AlgorithmPaintToolbox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "segmentationPlugin" );
    return plugin;
}

void AlgorithmPaintToolbox::setOutputMetadata(const medAbstractData * inputData, medAbstractData * outputData)
{
    Q_ASSERT(outputData && inputData);

    QStringList metaDataToCopy;
    metaDataToCopy
            << medMetaDataKeys::PatientName.key()
            << medMetaDataKeys::StudyDescription.key();

    foreach( const QString & key, metaDataToCopy ) {
        outputData->setMetaData(key, inputData->metadatas(key));
    }

    QString seriesDesc;
    seriesDesc = tr("Segmented from ") + medMetaDataKeys::SeriesDescription.getFirstValue( inputData );

    medMetaDataKeys::SeriesDescription.set(outputData,seriesDesc);
}


