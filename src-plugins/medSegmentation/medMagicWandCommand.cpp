/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMagicWandCommand.h>

#include <medAbstractImageView.h>

#include <itkConnectedThresholdImageFilter.h>
#include <itkImageRegionIterator.h>

#include <QVector3D>

class medMagicWandCommandPrivate
{
public:
    medAbstractImageView * view;
    medAbstractData* imageData;
    QVector3D vec;
    double radius;
    unsigned int maskValue;
    bool run3D;

    typedef itk::Image<unsigned char, 3> MaskType;
    MaskType::Pointer itkMask;
};


medMagicWandCommand::medMagicWandCommand(medAbstractImageView * view, medAbstractData* imageData, QVector3D &vec,
                                         double radius, MaskType::Pointer itkMask, unsigned int maskValue,
                                         bool run3D, /*medPaintMode mode = PAINT,*/ QUndoCommand *parent )
    :QUndoCommand("Magic Wand", parent), d(new medMagicWandCommandPrivate)
{
    d->view = view;
    d->imageData = imageData;
    d->vec = vec;
    d->radius = radius;
    d->itkMask = itkMask;
    d->maskValue = maskValue;
    d->run3D = run3D;
}


void medMagicWandCommand::updateWandRegion(unsigned int maskValue)
{
    //this->updateFromGuiItems();

//    if ( !m_imageData )
//    {
//        this->setData(view->layerData(view->currentLayer()));
//    }
//    if (!m_imageData) {
//        dtkWarn() << "Could not set data";
//        return;
//    }

//    if ((m_imageData->identifier().contains("4"))||
//            (m_imageData->identifier().contains("RGB"))||
//            (m_imageData->identifier().contains("Vector"))||
//            (m_imageData->identifier().contains("2")))
//    {
//        medMessageController::instance()->showError(tr("Magic wand option is only available for 3D images"),3000);
//        return;
//    }

    const QVector3D vpn = d->view->viewPlaneNormal();

    const MaskType::DirectionType & direction = d->itkMask->GetDirection();

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

    point[0] = d->vec.x();
    point[1] = d->vec.y();
    point[2] = d->vec.z();

    bool isInside = d->itkMask->TransformPhysicalPointToIndex (point, index);

    if (isInside)
    {
        RunConnectedFilter < itk::Image <char,3> > (index,planeIndex, maskValue);
        RunConnectedFilter < itk::Image <unsigned char,3> > (index,planeIndex, maskValue);
        RunConnectedFilter < itk::Image <short,3> > (index,planeIndex, maskValue);
        RunConnectedFilter < itk::Image <unsigned short,3> > (index,planeIndex, maskValue);
        RunConnectedFilter < itk::Image <int,3> > (index,planeIndex, maskValue);
        RunConnectedFilter < itk::Image <unsigned int,3> > (index,planeIndex, maskValue);
        RunConnectedFilter < itk::Image <long,3> > (index,planeIndex, maskValue);
        RunConnectedFilter < itk::Image <unsigned long,3> > (index,planeIndex, maskValue);
        RunConnectedFilter < itk::Image <float,3> > (index,planeIndex, maskValue);
        RunConnectedFilter < itk::Image <double,3> > (index,planeIndex, maskValue);
    }
}

template <typename IMAGE>
void
medMagicWandCommand::RunConnectedFilter (MaskType::IndexType &index, unsigned int planeIndex, unsigned int maskValue)
{
    IMAGE *tmpPtr = dynamic_cast<IMAGE *> ((itk::Object*)(d->imageData->data()));

    MaskType::PixelType pxValue = maskValue;

    if (!tmpPtr)
        return;

    typedef itk::ConnectedThresholdImageFilter<IMAGE, MaskType> ConnectedThresholdImageFilterType;
    typename ConnectedThresholdImageFilterType::Pointer ctiFilter = ConnectedThresholdImageFilterType::New();

    double value = tmpPtr->GetPixel(index);

    ctiFilter->SetUpper( value + d->radius );
    ctiFilter->SetLower( value - d->radius );

    MaskType::RegionType regionRequested = tmpPtr->GetLargestPossibleRegion();
    regionRequested.SetIndex(planeIndex, index[planeIndex]);
    regionRequested.SetSize(planeIndex, 1);
    MaskType::RegionType outRegion = regionRequested;
    outRegion.SetIndex(planeIndex,0);

    if ( !d->run3D )
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
        itk::ImageRegionIterator <MaskType> maskFilterItr (d->itkMask, regionRequested);
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
        itk::ImageRegionIterator <MaskType> maskFilterItr (d->itkMask, tmpPtr->GetLargestPossibleRegion());
        while (!maskFilterItr.IsAtEnd())
        {
            if (outFilterItr.Get() != 0)
                maskFilterItr.Set(pxValue);

            ++outFilterItr;
            ++maskFilterItr;
        }
    }

    d->itkMask->Modified();
    d->itkMask->GetPixelContainer()->Modified();
    d->itkMask->SetPipelineMTime(d->itkMask->GetMTime());
}

void medMagicWandCommand::undo()
{
   this->updateWandRegion(0);
   d->view->render();
}

void medMagicWandCommand::redo()
{
    this->updateWandRegion(d->maskValue);
    d->view->render();
}

