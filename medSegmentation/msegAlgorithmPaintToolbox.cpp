#include "msegAlgorithmPaintToolbox.h"

#include <medAbstractData.h>
#include <medAbstractDataImage.h>
#include <medAbstractView.h>
#include <medAbstractViewCoordinates.h>
#include <medDataIndex.h>
#include <medImageMaskAnnotationData.h>
#include <medMetaDataKeys.h>
#include <medToolBoxSegmentation.h>
#include <medMessageController.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkLog.h>
#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkGlobal.h>

#include <vnl/vnl_cross.h>
#include <vnl/vnl_vector.h>

#include <QtCore>

#include <algorithm>
#include <set>

namespace mseg {

class SelectDataEventFilter : public medViewEventFilter
{
public:
    SelectDataEventFilter(medToolBoxSegmentation * controller, AlgorithmPaintToolbox *cb ) :
        medViewEventFilter(),
            m_cb(cb)
        {}
        virtual bool mousePressEvent( medAbstractView *view, QMouseEvent *mouseEvent )
        {
            mouseEvent->accept();

            dtkAbstractData * viewData = medToolBoxSegmentation::viewData( view );
            if (viewData) {
                m_cb->setData( viewData );
                this->removeFromAllViews();
            }
            return mouseEvent->isAccepted();
        }

private :
    AlgorithmPaintToolbox *m_cb;
};

class ClickAndMoveEventFilter : public medViewEventFilter
{
public:
    ClickAndMoveEventFilter(medToolBoxSegmentation * controller, AlgorithmPaintToolbox *cb ) :
        medViewEventFilter(),
        m_cb(cb)
        {}

    virtual bool mousePressEvent( medAbstractView *view, QMouseEvent *mouseEvent )
    {
        medAbstractViewCoordinates * coords = view->coordinates();

        mouseEvent->accept();

        if (coords->is2D()) {
            // Convert mouse click to a 3D point in the image.

            QVector3D posImage = coords->displayToWorld( mouseEvent->posF() );
            this->m_state = State::Painting;

            //Project vector onto plane
            dtkAbstractData * viewData = medToolBoxSegmentation::viewData( view );
            this->m_points.push_back(posImage);

            m_cb->updateStroke( this,view );
        }
        return mouseEvent->isAccepted();
    }

    virtual bool mouseMoveEvent( medAbstractView *view, QMouseEvent *mouseEvent )
    {
        if ( this->m_state != State::Painting )
            return false;

        medAbstractViewCoordinates * coords = view->coordinates();
        mouseEvent->accept();

        if (coords->is2D()) {
            // Convert mouse click to a 3D point in the image.

            QVector3D posImage = coords->displayToWorld( mouseEvent->posF() );
            //Project vector onto plane
            this->m_points.push_back(posImage);
            m_cb->updateStroke( this,view );
        }
        return mouseEvent->isAccepted();
    }

    virtual bool mouseReleaseEvent( medAbstractView *view, QMouseEvent *mouseEvent )
    {
        if ( this->m_state == State::Painting )
        {
            this->m_state = State::Done;
            m_cb->updateStroke(this,view);
            this->m_points.clear();
            return true;
        }
        return false;
    }
    struct State {
        enum E { Start, Painting, Done };
    };

    State::E state() const { return m_state; }

    const std::vector<QVector3D> & points() const { return m_points; }

private :
    AlgorithmPaintToolbox *m_cb;
    std::vector<QVector3D> m_points;
    State::E m_state;
};

AlgorithmPaintToolbox::AlgorithmPaintToolbox(QWidget *parent ) :
    medToolBoxSegmentationCustom( parent),
    m_noDataText( tr("[No input data]") ),
    m_strokeRadius(4),
    m_paintState(PaintState::None)
{
    QWidget *displayWidget = new QWidget(this);
    this->addWidget(displayWidget);

    this->setTitle(this->s_name(this));

    QVBoxLayout * layout = new QVBoxLayout(displayWidget);

    QHBoxLayout * dataButtonsLayout = new QHBoxLayout();
    m_selectDataButton = new QPushButton( tr("Select Data") , displayWidget);
    m_selectDataButton->setToolTip(tr("To select data, click on this button, "
                                      "and then on a view."));
    m_resetDataButton = new QPushButton( tr("Reset Data") , displayWidget);
    m_resetDataButton->setToolTip(tr("Choose an other view to segment."));
    m_clearMaskButton = new QPushButton( tr("Clear Mask") , displayWidget);
    m_clearMaskButton->setToolTip(tr("Resets the mask."));
    m_selectDataButton->setCheckable(true);
    dataButtonsLayout->addWidget( m_selectDataButton );
    dataButtonsLayout->addWidget( m_resetDataButton );
    dataButtonsLayout->addWidget( m_clearMaskButton );
    layout->addLayout(dataButtonsLayout);

    m_insideStrokeButton = new QPushButton( tr("Inside") , displayWidget);
    m_insideStrokeButton->setToolTip(tr("Start painting the inside of the ROI."));
    m_outsideStrokeButton = new QPushButton( tr("Outside") , displayWidget);
    m_outsideStrokeButton->setToolTip(tr("Start painting the outside of the ROI."));
    m_removeStrokeButton = new QPushButton( tr("Erase") , displayWidget);
    m_removeStrokeButton->setToolTip(tr("Use an eraser on painted voxels."));
    m_boundaryStrokeButton = new QPushButton( tr("Boundary") , displayWidget);
    m_boundaryStrokeButton->setToolTip(tr("Select a Brush that paints bounderies between in and out"));

    m_insideStrokeButton->setCheckable(true);
    m_outsideStrokeButton->setCheckable(true);
    m_removeStrokeButton->setCheckable(true);
    m_boundaryStrokeButton->setCheckable(true);

    QHBoxLayout * addRemoveButtonLayout = new QHBoxLayout();
    addRemoveButtonLayout->addWidget( m_insideStrokeButton );
    addRemoveButtonLayout->addWidget( m_outsideStrokeButton );
    addRemoveButtonLayout->addWidget( m_removeStrokeButton );
    addRemoveButtonLayout->addWidget( m_boundaryStrokeButton );
    layout->addLayout( addRemoveButtonLayout );

    QHBoxLayout * brushSizeLayout = new QHBoxLayout();
    m_brushSizeSlider = new QSlider(Qt::Horizontal, displayWidget);
    m_brushSizeSlider->setToolTip(tr("Changes the brush radius."));
    m_brushSizeSlider->setValue(this->m_strokeRadius);
    m_brushSizeSlider->setMinimum(0);
    m_brushSizeSpinBox = new QSpinBox(displayWidget);
    m_brushSizeSpinBox->setToolTip(tr("Changes the brush radius."));
    m_brushSizeSpinBox->setValue(this->m_strokeRadius);
    m_brushSizeSpinBox->setMinimum(0);
    connect(m_brushSizeSpinBox, SIGNAL(valueChanged(int)),m_brushSizeSlider,SLOT(setValue(int)) );
    connect(m_brushSizeSlider,SIGNAL(valueChanged(int)),m_brushSizeSpinBox,SLOT(setValue(int)) );

    brushSizeLayout->addWidget(new QLabel(tr("Brush Radius"), displayWidget));
    brushSizeLayout->addWidget( m_brushSizeSlider );
    brushSizeLayout->addWidget( m_brushSizeSpinBox );
    layout->addLayout( brushSizeLayout );

    m_dataText = new QTextEdit( m_noDataText );
    {
        QFont font = m_dataText->currentFont();
        QFontMetrics fm( font );
        int textHeightInPixels = fm.height();
        m_dataText->setFixedHeight( textHeightInPixels );
        m_dataText->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    }
    layout->addWidget( m_dataText );

    m_applyButton = new QPushButton( tr("Create Database Item") , displayWidget);
    m_applyButton->setToolTip(tr("Save result to the Temporary Database"));
    layout->addWidget( m_applyButton );

    enableButtons(false);

    connect (m_insideStrokeButton,     SIGNAL(pressed()),
        this, SLOT(onInsideStrokePressed ()));
    connect (m_outsideStrokeButton,     SIGNAL(pressed()),
        this, SLOT(onOutsideStrokePressed ()));
    connect (m_removeStrokeButton,     SIGNAL(pressed()),
        this, SLOT(onRemoveStrokePressed ()));
    connect (m_boundaryStrokeButton,     SIGNAL(pressed()),
        this, SLOT(onBoundaryStrokePressed ()));

    connect (m_selectDataButton,     SIGNAL(pressed()),
        this, SLOT(onSetDataPressed ()));
    connect (m_resetDataButton,     SIGNAL(pressed()),
        this, SLOT(onResetDataPressed ()));
    connect (m_clearMaskButton,     SIGNAL(pressed()),
        this, SLOT(onClearMaskPressed ()));

    connect (m_applyButton,     SIGNAL(pressed()),
        this, SLOT(onApplyButtonPressed()));
}

AlgorithmPaintToolbox::~AlgorithmPaintToolbox()
{
}

void AlgorithmPaintToolbox::onInsideStrokePressed()
{
    if ( this->m_insideStrokeButton->isChecked() ) {
        this->m_viewFilter->removeFromAllViews();
        m_paintState = (PaintState::None);
        return;
    }
    setPaintState(PaintState::InsideStroke);
    m_viewFilter = ( new ClickAndMoveEventFilter(this->segmentationToolBox(), this) );
    this->segmentationToolBox()->addViewEventFilter( m_viewFilter );
}

void AlgorithmPaintToolbox::onOutsideStrokePressed()
{
    if ( this->m_outsideStrokeButton->isChecked() ) {
        this->m_viewFilter->removeFromAllViews();
        m_paintState = (PaintState::None);
        return;
    }
    setPaintState(PaintState::OutsideStroke);
    m_viewFilter = ( new ClickAndMoveEventFilter(this->segmentationToolBox(), this) );
    this->segmentationToolBox()->addViewEventFilter( m_viewFilter );
}
void AlgorithmPaintToolbox::onRemoveStrokePressed()
{
    if ( this->m_removeStrokeButton->isChecked() ) {
        this->m_viewFilter->removeFromAllViews();
        m_paintState = (PaintState::None);
        return;
    }
    setPaintState(PaintState::DeleteStroke);
    m_viewFilter = ( new ClickAndMoveEventFilter(this->segmentationToolBox(), this) );
    this->segmentationToolBox()->addViewEventFilter( m_viewFilter );
}
void AlgorithmPaintToolbox::onBoundaryStrokePressed()
{
    if ( this->m_boundaryStrokeButton->isChecked() ) {
        this->m_viewFilter->removeFromAllViews();
        m_paintState = (PaintState::None);
        return;
    }
    setPaintState(PaintState::BoundaryStroke);
    m_viewFilter = ( new ClickAndMoveEventFilter(this->segmentationToolBox(), this) );
    this->segmentationToolBox()->addViewEventFilter( m_viewFilter );
}


void AlgorithmPaintToolbox::onApplyButtonPressed()
{
    dtkAbstractProcessFactory *factory = dtkAbstractProcessFactory::instance();

    dtkSmartPointer <medProcessPaintSegm> alg =
            factory->createSmartPointer( medProcessPaintSegm::s_identifier() );

    alg->setInput( this->m_maskData, medProcessPaintSegm::MaskChannel );
    alg->setInput( this->m_imageData, medProcessPaintSegm::ImageChannel );

    this->segmentationToolBox()->run( alg );
}

void AlgorithmPaintToolbox::onSetDataPressed()
{
    setPaintState(PaintState::None);

    if ( this->m_selectDataButton->isChecked() ) {
        this->m_selectDataButton->setChecked(false);
        this->m_viewFilter->removeFromAllViews();
        return;
    }
    this->m_selectDataButton->setChecked(true);
    m_viewFilter = ( new SelectDataEventFilter(this->segmentationToolBox(), this) );
    this->segmentationToolBox()->addViewEventFilter( m_viewFilter );
}

void AlgorithmPaintToolbox::onResetDataPressed()
{
    setPaintState(PaintState::None);
    this->setData(NULL);
}

void AlgorithmPaintToolbox::onClearMaskPressed()
{
    if ( m_maskData && m_itkMask ){
        m_itkMask->FillBuffer( medToolBoxSegmentation::MaskPixelValues::Unset );
        m_itkMask->Modified();
        m_itkMask->GetPixelContainer()->Modified();
        m_itkMask->SetPipelineMTime(m_itkMask->GetMTime());

        m_maskAnnotationData->invokeModified();
    }
}

void AlgorithmPaintToolbox::setData( dtkAbstractData *dtkdata )
{
    // disconnect existing
    if ( m_imageData ) {
        // TODO?
    }

    m_lastVup = QVector3D();
    m_lastVpn = QVector3D();

    m_imageData = dtkSmartPointer<dtkAbstractData>(dtkdata);

    QString dataText;
    if ( m_imageData ) {
        QString patientName;
        QString studyName;
        QString seriesName;

        if ( m_imageData->hasMetaData( medMetaDataKeys::PatientName.key() ) ){
            patientName = dtkdata->metaDataValues(medMetaDataKeys::PatientName.key())[0];
        }
        if ( m_imageData->hasMetaData( medMetaDataKeys::StudyDescription.key() ) ){
            studyName = dtkdata->metaDataValues(medMetaDataKeys::StudyDescription.key())[0];
        }
        if ( m_imageData->hasMetaData( medMetaDataKeys::SeriesDescription.key() ) ){
            seriesName = dtkdata->metaDataValues(medMetaDataKeys::SeriesDescription.key())[0];
        }

        dataText = patientName + '/' + studyName + '/' +seriesName;
    } else {
        // data is NULL
        dataText = m_noDataText;
    }
    m_dataText->setText( dataText );

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
                    dtkAbstractDataFactory::instance()->createSmartPointer( medProcessPaintSegm::MaskImageTypeIdentifier() );

            if ( !m_maskData ) {
                dtkDebug() << DTK_PRETTY_FUNCTION << "Failed to create " << medProcessPaintSegm::MaskImageTypeIdentifier();
                return;
            }

        //    if ( this->m_maskAnnotationData ) {
        //        m_maskAnnotationData->parentData()->removeAttachedData(m_maskAnnotationData);
        //    }
            
            m_maskAnnotationData = new medImageMaskAnnotationData;
            this->initializeMaskData( m_imageData, m_maskData );
            m_maskAnnotationData->setMaskData(qobject_cast<medAbstractDataImage*>(m_maskData));

            medImageMaskAnnotationData::ColorMapType colorMap;
            typedef medImageMaskAnnotationData::ColorMapType::value_type PairType;
            const qreal fgVal = medToolBoxSegmentation::MaskPixelValues::Foreground;
            const qreal bgVal = medToolBoxSegmentation::MaskPixelValues::Background;
            colorMap.push_back( PairType( fgVal - 0.4, QColor(0,0,0,0.1) ) );
            colorMap.push_back( PairType( fgVal      , QColor(Qt::green) ) );
            colorMap.push_back( PairType( fgVal + 0.4, QColor(0,0,0,0.1) ) );
            colorMap.push_back( PairType( bgVal - 0.4, QColor(0,0,0,0.1) ) );
            colorMap.push_back( PairType( bgVal      , QColor(Qt::red) ) );
            colorMap.push_back( PairType( bgVal + 0.4, QColor(0,0,0,0.1) ) );
            m_maskAnnotationData->setColorMap( colorMap );

            m_imageData->addAttachedData(m_maskAnnotationData);
        }
    }

    if ( m_imageData ) {
        m_itkMask = dynamic_cast<MaskType*>( reinterpret_cast<itk::Object*>(m_maskData->data()) );
        this->enableButtons(true);
    } else {
        m_itkMask = NULL;
        this->enableButtons(false);
    }
}

//static
medToolBoxSegmentationCustom *
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

    medAbstractDataImage * mImage = qobject_cast<medAbstractDataImage*>(imageData);
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
    mask->FillBuffer( medToolBoxSegmentation::MaskPixelValues::Unset );

    maskData->setData((QObject*)(mask.GetPointer()));
}

void AlgorithmPaintToolbox::updateStroke( ClickAndMoveEventFilter * filter, medAbstractView * view )
{
    this->updateFromGuiItems();

    const double radius = m_strokeRadius; // in image units.

    if ( !m_imageData ) {
        this->setData(this->segmentationToolBox()->viewData(view));
    }
    if (!m_imageData) {
        dtkWarning() << "Could not set data";
        return;
    }

    QVector3D newPoint = filter->points().back();

    typedef  MaskType::DirectionType::InternalMatrixType::element_type ElemType;
    itk::Point< ElemType > centerPoint;
    centerPoint.SetElement(0, newPoint.x());
    centerPoint.SetElement(1, newPoint.y());
    centerPoint.SetElement(2, newPoint.z());

    const medAbstractViewCoordinates * coords = view->coordinates();
    const QVector3D vup = coords->viewUp();
    const QVector3D vpn = coords->viewPlaneNormal();

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
    switch ( m_paintState ) {
    case PaintState::InsideStroke :
        pxValue = medToolBoxSegmentation::MaskPixelValues::Foreground;
        break;
    case PaintState::OutsideStroke :
        pxValue = medToolBoxSegmentation::MaskPixelValues::Background;
        break;
    default:
        pxValue = medToolBoxSegmentation::MaskPixelValues::Unset;
        break;
    }

    vnl_vector_fixed<ElemType, 3> lrPlane;
    if ( m_paintState == PaintState::BoundaryStroke ) {
        if ( filter->points().size() <= 1 ) {
            return;
        }
        QVector3D prevPoint = *( ++(filter->points().rbegin() ) );
        vnl_vector_fixed<ElemType, 3> vecPrev(prevPoint.x(), prevPoint.y(), prevPoint.z() );//vnl_cross_3d(vecVup,vecVpn);
        vnl_vector_fixed<ElemType, 3> diffPrev = centerPoint.GetVnlVector() - vecPrev;
        lrPlane = vnl_cross_3d(vecVpn, diffPrev);
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

            if ( m_paintState == PaintState::BoundaryStroke ) {
                vnl_vector_fixed<ElemType, 3> offset = testPt.GetVnlVector() - centerPoint.GetVnlVector();
                double proj = dot_product( offset, lrPlane );
                pxValue = (proj >= 0) ?  medToolBoxSegmentation::MaskPixelValues::Foreground :
                     medToolBoxSegmentation::MaskPixelValues::Background;
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

    m_maskAnnotationData->invokeModified();
}

void AlgorithmPaintToolbox::updateFromGuiItems()
{
    this->m_strokeRadius = m_brushSizeSlider->value();
}

void AlgorithmPaintToolbox::enableButtons( bool value )
{
    m_insideStrokeButton->setEnabled(value);
    m_outsideStrokeButton->setEnabled(value);
    m_removeStrokeButton->setEnabled(value);
    m_boundaryStrokeButton->setEnabled(value);
    m_applyButton->setEnabled(value);
    m_clearMaskButton->setEnabled(value);
    m_resetDataButton->setEnabled(value);
}

void AlgorithmPaintToolbox::setPaintState( PaintState::E value )
{
    if ( m_paintState == value )
        return;

    switch( m_paintState ){
    case PaintState::InsideStroke:
        m_insideStrokeButton->setChecked(false); break;
    case PaintState::OutsideStroke:
        m_outsideStrokeButton->setChecked(false); break;
    case PaintState::DeleteStroke:
        m_removeStrokeButton->setChecked(false); break;
    case PaintState::BoundaryStroke:
        m_boundaryStrokeButton->setChecked(false); break;
    default:
        break;
    }

    m_paintState = value;
}








} // namespace mseg

