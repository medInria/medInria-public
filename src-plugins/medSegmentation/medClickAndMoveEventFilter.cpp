/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medClickAndMoveEventFilter.h>

#include <medAbstractImageView.h>
#include <medAbstractDataFactory.h>
#include <medPaintCommand.h>
#include <medMagicWandCommand.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>

#include <itkMinimumMaximumImageCalculator.h>
#include <itkLabelMapToLabelImageFilter.h>


medClickAndMoveEventFilter::medClickAndMoveEventFilter(medAlgorithmPaintToolbox *cb) :
    medViewEventFilter(),
    m_toolbox(cb),
    m_paintState(PaintState::None),
    m_lastPaintState(PaintState::None)
{
    m_maskAnnotationData = NULL;
    m_maskData = NULL;
    m_imageData = NULL;
    m_view = NULL;
}

medClickAndMoveEventFilter::~medClickAndMoveEventFilter()
{
}

void medClickAndMoveEventFilter::setColorMap( medImageMaskAnnotationData::ColorMapType colorMap)
{
    m_labelColorMap = colorMap;

    if (m_maskAnnotationData)
    {
        m_maskAnnotationData->setColorMap(m_labelColorMap);
        m_maskAnnotationData->invokeModified();
    }
}


bool medClickAndMoveEventFilter::mousePressEvent(medAbstractView *view, QMouseEvent *mouseEvent )
{
    m_paintState = m_toolbox->paintState();
    m_view = view;

    if ( this->m_paintState == PaintState::DeleteStroke )
    {
        m_toolbox->setPaintState(m_lastPaintState);
        m_paintState = m_lastPaintState;
    }

    if(mouseEvent->button() == Qt::RightButton) // right-click for erasing
    {
        m_lastPaintState = m_toolbox->paintState();
        m_toolbox->setPaintState(PaintState::DeleteStroke);
        m_paintState = m_toolbox->paintState(); //update
    }

    if (m_paintState == PaintState::Stroke && mouseEvent->button() == Qt::LeftButton)
    {
        m_toolbox->setPaintState(PaintState::Stroke);
        m_paintState = m_toolbox->paintState(); //update paintState
    }

    mouseEvent->accept();

    medAbstractImageView * imageView = dynamic_cast<medAbstractImageView *>(view);
    if(!imageView)
        return false;

    // let's take the first non medImageMaskAnnotationData as the reference data
    // TODO: to improve...

    for(unsigned int i=0; i<imageView->layersCount(); i++)
    {

        medAbstractData *data = imageView->layerData(i);
        if (!data)
            continue;

        medImageMaskAnnotationData * existingMaskAnnData = dynamic_cast<medImageMaskAnnotationData *>(data);
        if(!existingMaskAnnData)
        {
            this->setData( data );
            if(!imageView->contains(m_maskAnnotationData))
            {
                imageView->addLayer(m_maskAnnotationData);
                setOutputMetadata(m_imageData, m_maskData);
                medDataManager::instance()->importData(m_maskData);
            }
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
            medPaintCommandOptions *options = new medPaintCommandOptions;
            QVector<QVector3D> vpoints;
            vpoints.append(posImage);

            options->points = vpoints;
            options->view = imageView;
            options->data = m_imageData;
            options->radius = m_toolbox->wandRadius();
            options->itkMask = m_itkMask;
            options->maskData = m_maskData;
            options->maskValue = m_toolbox->strokeLabel();

            medMagicWandCommand *magicWandCommand = new medMagicWandCommand(options, m_toolbox->isWand3D());
            imageView->undoStack()->push(magicWandCommand);

            m_paintState = PaintState::None; //Wand operation is over
        }
    }
    return mouseEvent->isAccepted();
}


bool medClickAndMoveEventFilter::mouseMoveEvent( medAbstractView *view, QMouseEvent *mouseEvent )
{
    m_view = view;

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

        QVector<QVector3D> vpoints;
        vpoints.append(posImage);

        medPaintCommandOptions *options = new medPaintCommandOptions;
        options->points = vpoints;
        options->view = imageView;
        options->data = m_imageData;
        options->radius = m_toolbox->strokeRadius();
        options->itkMask = m_itkMask;
        options->maskData = m_maskData;

        if(this->m_paintState == PaintState::Stroke)
            options->maskValue = m_toolbox->strokeLabel();
        else if(this->m_paintState == PaintState::DeleteStroke)
            options->maskValue = 0;

        medPaintCommand *paintCommand = new medPaintCommand(options);
        paintCommand->paint();

        m_maskAnnotationData->invokeModified();
    }

    return mouseEvent->isAccepted();
}


bool medClickAndMoveEventFilter::mouseReleaseEvent( medAbstractView *view, QMouseEvent *mouseEvent )
{
    m_view = view;

    medAbstractImageView * imageView = dynamic_cast<medAbstractImageView *>(view);
    if(!imageView)
        return false;

    if ( this->m_paintState == PaintState::None )
        return false;

    if (imageView->is2D())
    {
        medPaintCommandOptions *options = new medPaintCommandOptions;
        options->points = m_points;
        options->view = imageView;
        options->data = m_imageData;
        options->radius = m_toolbox->strokeRadius();
        options->itkMask = m_itkMask;
        options->maskData = m_maskData;

        if(this->m_paintState == PaintState::Stroke)
            options->maskValue = m_toolbox->strokeLabel();
        else if(this->m_paintState == PaintState::DeleteStroke)
            options->maskValue = 0;

        medPaintCommand *paintCommand = new medPaintCommand(options);
        imageView->undoStack()->push(paintCommand);

        m_maskAnnotationData->invokeModified();

        m_paintState = PaintState::None; //Painting is done

        this->m_points.clear();
    }
    return true;
}


void medClickAndMoveEventFilter::setData( medAbstractData *medData )
{
    if (!medData)
        return;

    // disconnect existing
    if ( m_imageData )
    {
        // TODO?
    }

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

            m_maskData = medAbstractDataFactory::instance()->createSmartPointer( "itkDataImageUChar3" );

            if ( !m_maskData ) {
                dtkDebug() << DTK_PRETTY_FUNCTION << "Failed to create itkDataImageUChar3";
                return;
            }

            m_maskAnnotationData = new medImageMaskAnnotationData;
            this->initializeMaskData( m_imageData, m_maskData );

            m_maskAnnotationData->setMaskData(qobject_cast<medAbstractImageData*>(m_maskData));

            m_maskAnnotationData->setColorMap( m_labelColorMap );
            m_maskAnnotationData->invokeModified();

            m_imageData->addAttachedData(m_maskAnnotationData);
        }
    }

    if ( m_imageData ) {
        m_itkMask = dynamic_cast<MaskType*>( reinterpret_cast<itk::Object*>(m_maskData->data()) );
        m_toolbox->showButtons(true);
    } else {
        m_itkMask = NULL;
        m_toolbox->showButtons(false);
    }

    m_toolbox->setMask(m_itkMask);
}

void medClickAndMoveEventFilter::initializeMaskData( medAbstractData * imageData, medAbstractData * maskData )
{
    MaskType::Pointer mask = MaskType::New();

    Q_ASSERT(mask->GetImageDimension() == 3);

    medAbstractImageData * mImage = qobject_cast<medAbstractImageData*>(imageData);
    Q_ASSERT(mImage);
    //Q_ASSERT(mask->GetImageDimension() >= mImage->Dimension());

    MaskType::RegionType region;
    region.SetSize(0, ( mImage->dimension() > 0 ? mImage->xDimension() : 1 ) );
    region.SetSize(1, ( mImage->dimension() > 1 ? mImage->yDimension() : 1 ) );
    region.SetSize(2, ( mImage->dimension() > 2 ? mImage->zDimension() : 1 ) );

    MaskType::DirectionType direction;
    MaskType::SpacingType spacing;
    MaskType::PointType origin;

    direction.Fill(0);
    spacing.Fill(0);
    origin.Fill(0);
    for (unsigned int i = 0;i < mask->GetImageDimension();++i)
        direction(i,i) = 1;

    unsigned int maxIndex = std::min<unsigned int>(mask->GetImageDimension(),mImage->dimension());

    switch (mImage->dimension())
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
    mask->FillBuffer( 0 );

    maskData->setData((QObject*)(mask.GetPointer()));
}



template <typename IMAGE>
void
medClickAndMoveEventFilter::GenerateMinMaxValuesFromImage ()
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

    m_toolbox->setMinValue(m_MinValueImage);
    m_toolbox->setMaxValue(m_MaxValueImage);
    m_toolbox->setWandSliderValue(m_toolbox->wandRadius());
}

void medClickAndMoveEventFilter::setOutputMetadata(const medAbstractData * inputData, medAbstractData * outputData)
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
