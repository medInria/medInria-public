/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVtkViewItkDataImage4DInteractor.h"

#include <vtkMetaDataSetSequence.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkImageActor.h>
#include <vtkImageProperty.h>

#include <medAbstractImageView.h>
#include <medVtkViewBackend.h>
#include <medViewFactory.h>
#include <medAbstractImageData.h>
#include <medDoubleParameterL.h>

#include <QVBoxLayout>
#include <QWidget>

#include <vtkItkConversion.h>

class medVtkViewItkDataImage4DInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;

    medAbstractImageView *view;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;

    vtkMetaDataSetSequence *sequence;
    medAbstractImageData *imageData;

    //double currentTime;

};


medVtkViewItkDataImage4DInteractor::medVtkViewItkDataImage4DInteractor(medAbstractView *parent):
    medVtkViewItkDataImageInteractor(parent), d(new medVtkViewItkDataImage4DInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView *>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    //d->currentTime = 0.0;


    m_poConv = nullptr;
}

medVtkViewItkDataImage4DInteractor::~medVtkViewItkDataImage4DInteractor()
{
    delete(d);
    delete(m_poConv);
    m_poConv = nullptr;
}

QString medVtkViewItkDataImage4DInteractor::description() const
{
    return tr("Interactor displaying 4D Meshes");
}

QString medVtkViewItkDataImage4DInteractor::identifier() const
{
    return "medVtkViewItkDataImage4DInteractor";
}

QStringList medVtkViewItkDataImage4DInteractor::handled() const
{
    return medVtkViewItkDataImage4DInteractor::dataHandled();
}

QStringList medVtkViewItkDataImage4DInteractor::dataHandled()
{
    QStringList d = QStringList() << "itkDataImageChar4"
                                  << "itkDataImageUChar4"
                                  << "itkDataImageShort4"
                                  << "itkDataImageUShort4"
                                  << "itkDataImageInt4"
                                  << "itkDataImageUInt4"
                                  << "itkDataImageLong4"
                                  << "itkDataImageULong4"
                                  << "itkDataImageFloat4"
                                  << "itkDataImageDouble4";
    return  d;
}

bool medVtkViewItkDataImage4DInteractor::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    return factory->registerInteractor<medVtkViewItkDataImage4DInteractor>("medVtkViewItkDataImage4DInteractor",
                                                                          QStringList () << "medVtkView" <<
                                                                          medVtkViewItkDataImage4DInteractor::dataHandled());
}

void medVtkViewItkDataImage4DInteractor::setInputData(medAbstractData *data)
{
    d->imageData = dynamic_cast<medAbstractImageData *>(data);
    if(!d->imageData)
        return;

    if( data->identifier().contains("itkDataImage") &&  d->imageData->Dimension() == 4 ) {

        //d->sequence = vtkMetaDataSetSequence::New();

        int layer = d->view->layer(data);

        if (SetViewInput(data, layer) /* AppendImageSequence<char>(data,d->view,d->sequence, layer)           ||
              AppendImageSequence<unsigned char>(data,d->view,d->sequence, layer)  ||
              AppendImageSequence<short>(data,d->view,d->sequence, layer)          ||
              AppendImageSequence<unsigned short>(data,d->view,d->sequence, layer) ||
              AppendImageSequence<int>(data,d->view,d->sequence, layer)            ||
              AppendImageSequence<unsigned int>(data,d->view,d->sequence, layer)   ||
              AppendImageSequence<long>(data,d->view,d->sequence, layer)           ||
              AppendImageSequence<unsigned long>(data,d->view,d->sequence, layer)  ||
              AppendImageSequence<float>(data,d->view,d->sequence, layer)          ||
              AppendImageSequence<double>(data,d->view,d->sequence, layer)*/)
        {
            d->imageData->addMetaData("SequenceDuration", QString::number(m_poConv->getTotalTime()));
            d->imageData->addMetaData("SequenceFrameRate", QString::number((double)m_poConv->getNumberOfVolumes() / (double)m_poConv->getTotalTime()));

            //FloTODO dtkDebug() << "SequenceDuration" << d->sequence->GetMaxTime();
            //FloTODO dtkDebug() << "SequenceFrameRate" <<(double)d->sequence->GetNumberOfMetaDataSets() / (double)d->sequence->GetMaxTime();

            d->view2d->GetImageActor(d->view2d->GetCurrentLayer())->GetProperty()->SetInterpolationTypeToCubic();
            initParameters(d->imageData);

            //double* range = d->sequence->GetCurrentScalarRange();
            double* range = m_poConv->getCurrentScalarRange();
            d->view2d->SetColorRange(range);
            this->initWindowLevelParameters(range);

            if(d->view->layer(d->imageData) == 0)
            {
                switch(d->view2d->GetViewOrientation())
                {
                case vtkImageView2D::VIEW_ORIENTATION_AXIAL:
                    d->view->setOrientation(medImageView::VIEW_ORIENTATION_AXIAL);
                    break;
                case vtkImageView2D::VIEW_ORIENTATION_SAGITTAL:
                    d->view->setOrientation(medImageView::VIEW_ORIENTATION_SAGITTAL);
                    break;
                case vtkImageView2D::VIEW_ORIENTATION_CORONAL:
                    d->view->setOrientation(medImageView::VIEW_ORIENTATION_CORONAL);
                    break;
                }
            }
        }
    }
}

bool medVtkViewItkDataImage4DInteractor::SetViewInput(medAbstractData* data, int layer)
{
    bool bRes = true;

    if (m_poConv = vtkItkConversionInterface::creatInstance(data))
    {
        itk::DataObject::Pointer image = (itk::DataObject*)(data->data());
        vtkAlgorithmOutput *poVtkAlgoOutputPort = nullptr;
        vtkMatrix4x4 *poMatrix = nullptr;

        bRes = m_poConv->SetITKInput(image);
        if (bRes)
        {
            bRes = m_poConv->GetConversion(poVtkAlgoOutputPort, poMatrix);
            if (bRes)
            {
                d->view2d->SetInput(poVtkAlgoOutputPort, poMatrix, layer); //SetITKInput(image, layer);
                d->view3d->SetInput(poVtkAlgoOutputPort, poMatrix, layer); //SetITKInput(image, layer);
            }
        }
    }
    else
    {
        bRes = false;
    }

    return bRes;
}
template <typename TYPE>
bool AppendImageSequence(medAbstractData* data, medAbstractImageView* view, vtkMetaDataSetSequence* sequence, int& layer)
{

    if (itk::Image<TYPE, 4>* image = dynamic_cast<itk::Image<TYPE, 4>*>(static_cast<itk::Object*>(data->data())))
    {

        medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(view->backend());

        //sequence->SetITKDataSet<TYPE>(image);

        vtkMetaImageData* metaimage = vtkMetaImageData::SafeDownCast(sequence->GetMetaDataSet(0U));
        vtkImageData*     vtkimage = vtkImageData::SafeDownCast(sequence->GetDataSet());

        backend->view2D->SetInput(metaimage->GetAlgorithmOutputPort(), metaimage->GetOrientationMatrix(), layer);
        backend->view3D->SetInput(metaimage->GetAlgorithmOutputPort(), metaimage->GetOrientationMatrix(), layer);
        layer = backend->view2D->GetNumberOfLayers() - 1;

        return true;
    }
    return false;
}

/*
template <typename TYPE>
bool AppendImageSequence(medAbstractData* data, medAbstractImageView* view, vtkMetaDataSetSequence* sequence, int& layer)
{

    if (itk::Image<TYPE, 4>* image = dynamic_cast<itk::Image<TYPE, 4>*>(static_cast<itk::Object*>(data->data())))
    {

        medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(view->backend());

        sequence->SetITKDataSet<TYPE>(image);

        vtkMetaImageData* metaimage = vtkMetaImageData::SafeDownCast(sequence->GetMetaDataSet(0U));
        vtkImageData*     vtkimage = vtkImageData::SafeDownCast(sequence->GetDataSet());

        backend->view2D->SetInput(metaimage->GetAlgorithmOutputPort(), metaimage->GetOrientationMatrix(), layer);
        backend->view3D->SetInput(metaimage->GetAlgorithmOutputPort(), metaimage->GetOrientationMatrix(), layer);
        layer = backend->view2D->GetNumberOfLayers() - 1;

        return true;
    }
    return false;
}
*/

void medVtkViewItkDataImage4DInteractor::SetITKDataSet(typename itk::Image<float, 4> *dataset)
{
    //SetITKDataSet<float>(dataset);
}

template <typename IMAGE>
bool medVtkViewItkDataImage4DInteractor::SetViewInput(const char* type, medAbstractData* data, int layer)
{
    bool bRes = data->identifier() == type;

    if (bRes)
    {
        if (IMAGE* image = dynamic_cast<IMAGE*>((itk::Object*)(data->data())))
        {
            vtkAlgorithmOutput *poVtkAlgoOutputPort = nullptr;
            vtkMatrix4x4 *poMatrix = nullptr;
            bRes = m_poConv->SetITKInput(image);
            if (bRes)
            {
                bRes = m_poConv->GetConversion(poVtkAlgoOutputPort, poMatrix);
                if (bRes)
                {
                    d->view2d->SetInput(poVtkAlgoOutputPort, poMatrix, layer);
                    d->view3d->SetInput(poVtkAlgoOutputPort, poMatrix, layer);
                }
            }
        }
        else
        {
            bRes = false;
        }
    }

    return bRes;
}

medAbstractData *medVtkViewItkDataImage4DInteractor::inputData() const
{
    return d->imageData;
}

void medVtkViewItkDataImage4DInteractor::setCurrentTime(double time)
{
    if(m_poConv->getTimeIndex() * m_poConv->getTotalTime() / m_poConv->getNumberOfVolumes() == time)
        return;
    //d->sequence->UpdateToTime(time);
    m_poConv->setTimeIndex(static_cast<unsigned int>(round(time * m_poConv->getNumberOfVolumes() / m_poConv->getTotalTime() )) -1);
}