/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMagicWandCommand.h>

#include <medAbstractImageView.h>
#include <medMessageController.h>

#include <itkImageRegionIterator.h>
#include <itkLabelMapToLabelImageFilter.h>
#include <itkConnectedThresholdImageFilter.h>
#include <itkLabelMapToAttributeImageFilter.h>

#include <QVector3D>

#include <medVtkViewBackend.h>
#include <vtkImageAlgorithm.h>

class medMagicWandCommandPrivate
{
public:
    bool run3D;

    LabelObjectType::Pointer label;
    LabelMapType::Pointer labelMap;

    typedef itk::LabelMapToAttributeImageFilter< LabelMapType, MaskType > MapToImageFilter;
    MapToImageFilter::Pointer filter;

};


medMagicWandCommand::medMagicWandCommand(medPaintCommandOptions *options, bool run3D, QUndoCommand *parent )
    :medAbstractPaintCommand(options, parent), d(new medMagicWandCommandPrivate)
{
    d->run3D = run3D;

    this->setText("Magic Wand");

    d->label = 0;

    d->labelMap = medPaintCommandManager::instance()->labelMap(this->options()->itkMask);

    typedef itk::LabelMapToAttributeImageFilter< LabelMapType, MaskType > MapToImageFilter;
    d->filter = MapToImageFilter::New();
}

medMagicWandCommand::~medMagicWandCommand()
{
    delete d;
    d = 0;
}


void medMagicWandCommand::paint()
{
    if (!this->options()->data)
    {
        dtkWarn() << "Could not set data";
        return;
    }

    if ((this->options()->data->identifier().contains("4"))||
            (this->options()->data->identifier().contains("RGB"))||
            (this->options()->data->identifier().contains("Vector"))||
            (this->options()->data->identifier().contains("2")))
    {
        medMessageController::instance().showError("Magic wand option is only available for 3D images",3000);
        return;
    }

    d->label = LabelObjectType::New();
    d->label->SetAttribute(this->options()->maskValue);

    const QVector3D vpn = this->options()->view->viewPlaneNormal();

    const MaskType::DirectionType & direction = this->options()->itkMask->GetDirection();

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

    point[0] = this->options()->points[0].x();
    point[1] = this->options()->points[0].y();
    point[2] = this->options()->points[0].z();

    bool isInside = this->options()->itkMask->TransformPhysicalPointToIndex (point, index);

    if (isInside)
    {
        RunConnectedFilter < itk::Image <char,3> > (index,planeIndex, this->options()->maskValue);
        RunConnectedFilter < itk::Image <unsigned char,3> > (index,planeIndex, this->options()->maskValue);
        RunConnectedFilter < itk::Image <short,3> > (index,planeIndex, this->options()->maskValue);
        RunConnectedFilter < itk::Image <unsigned short,3> > (index,planeIndex, this->options()->maskValue);
        RunConnectedFilter < itk::Image <int,3> > (index,planeIndex, this->options()->maskValue);
        RunConnectedFilter < itk::Image <unsigned int,3> > (index,planeIndex, this->options()->maskValue);
        RunConnectedFilter < itk::Image <long,3> > (index,planeIndex, this->options()->maskValue);
        RunConnectedFilter < itk::Image <unsigned long,3> > (index,planeIndex, this->options()->maskValue);
        RunConnectedFilter < itk::Image <float,3> > (index,planeIndex, this->options()->maskValue);
        RunConnectedFilter < itk::Image <double,3> > (index,planeIndex, this->options()->maskValue);
    }
}

template <typename IMAGE>
void
medMagicWandCommand::RunConnectedFilter (MaskType::IndexType &index, unsigned int planeIndex, unsigned int maskValue)
{
    IMAGE *tmpPtr = dynamic_cast<IMAGE *> ((itk::Object*)(this->options()->data->data()));

    MaskType::PixelType pxValue = maskValue;

    if (!tmpPtr)
        return;

    typedef itk::ConnectedThresholdImageFilter<IMAGE, MaskType> ConnectedThresholdImageFilterType;
    typename ConnectedThresholdImageFilterType::Pointer ctiFilter = ConnectedThresholdImageFilterType::New();

    double value = tmpPtr->GetPixel(index);
    double valueMin =  value - this->options()->radius;
    double valueMax = value + this->options()->radius;

    if(valueMin < std::numeric_limits<typename IMAGE::PixelType>::min() )
        valueMin = std::numeric_limits<typename IMAGE::PixelType>::min();

    if(valueMax > std::numeric_limits<typename IMAGE::PixelType>::max() )
        valueMax = std::numeric_limits<typename IMAGE::PixelType>::max();

    ctiFilter->SetUpper( valueMax );
    ctiFilter->SetLower( valueMin );

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
        itk::ImageRegionIterator <MaskType> maskFilterItr (this->options()->itkMask, regionRequested);
        while (!maskFilterItr.IsAtEnd())
        {
            if (outFilterItr.Get() != 0)
            {
                maskFilterItr.Set(pxValue);
                d->label->AddIndex(maskFilterItr.GetIndex());
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
        itk::ImageRegionIterator <MaskType> maskFilterItr (this->options()->itkMask, tmpPtr->GetLargestPossibleRegion());
        while (!maskFilterItr.IsAtEnd())
        {
            if (outFilterItr.Get() != 0)
            {
                maskFilterItr.Set(pxValue);
                d->label->AddIndex(maskFilterItr.GetIndex());
            }

            ++outFilterItr;
            ++maskFilterItr;
        }
    }

    vtkImageView2D* poImageViewTmp = ((medVtkViewBackend*)this->options()->view->backend())->view2D;
    unsigned int uiLayer = this->options()->view->layer((medAbstractData*) this->options()->maskAnnotationData);
    vtkAlgorithm * poAlgoTmp = poImageViewTmp->GetImageAlgorithmForLayer(uiLayer);
    poAlgoTmp->Modified();
}


void medMagicWandCommand::undo()
{
    if(!d->label)
        return;

    d->labelMap->RemoveLabelObject(d->label);

    d->filter->SetInput(d->labelMap);
    d->filter->Update();

    this->options()->itkMask->Graft(d->filter->GetOutput());

    this->options()->view->render();
}

void medMagicWandCommand::redo()
{
    if(!d->label)
    {
        paint();

        if(d->label)
          d->labelMap->PushLabelObject(d->label);
    }
    else
    {
        d->labelMap->PushLabelObject(d->label);

        d->filter->SetInput(d->labelMap);
        d->filter->Update();

        this->options()->itkMask->Graft(d->filter->GetOutput());
    }

    this->options()->view->render();
}
