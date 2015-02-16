/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVtkViewItk4DScalarImageInteractor.h"

#include <vtkMetaDataSetSequence.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkImageActor.h>
#include <vtkImageProperty.h>

#include <medAbstractImageView.h>
#include <medVtkViewBackend.h>
#include <medViewFactory.h>
#include <medAbstractImageData.h>
#include <medDoubleParameter.h>


class medVtkViewItk4DScalarImageInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;

    medAbstractImageView *view;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;

    vtkMetaDataSetSequence *sequence;
    medAbstractImageData *imageData;

    double currentTime;

};

template <typename TYPE>
bool AppendImageSequence(medAbstractData* data,medAbstractImageView* view,vtkMetaDataSetSequence* sequence, int& layer) {

    if (itk::Image<TYPE,4>* image = dynamic_cast<itk::Image<TYPE,4>*>(static_cast<itk::Object*>(data->data())))
    {

        medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(view->backend());

        sequence->SetITKDataSet<TYPE>(image);

        vtkMetaImageData* metaimage = vtkMetaImageData::SafeDownCast(sequence->GetMetaDataSet(0U));
        vtkImageData*     vtkimage  = vtkImageData::SafeDownCast(sequence->GetDataSet());

        backend->view2D->SetInput(vtkimage,metaimage->GetOrientationMatrix(), layer);
        backend->view3D->SetInput(vtkimage,metaimage->GetOrientationMatrix(), layer);
        layer = backend->view2D->GetNumberOfLayers()-1;

        return true;
    }
    return false;
}

medVtkViewItk4DScalarImageInteractor::medVtkViewItk4DScalarImageInteractor(medAbstractView *parent):
    medVtkViewItkScalarImageInteractor(parent), d(new medVtkViewItk4DScalarImageInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView *>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->currentTime = 0.0;
}

medVtkViewItk4DScalarImageInteractor::~medVtkViewItk4DScalarImageInteractor()
{

}

QString medVtkViewItk4DScalarImageInteractor::description() const
{
    return tr("Interactor displaying 4D Meshes");
}

QString medVtkViewItk4DScalarImageInteractor::identifier() const
{
    return "medVtkViewItk4DScalarImageInteractor";
}

QStringList medVtkViewItk4DScalarImageInteractor::handled() const
{
    return dataHandled();
}

QStringList medVtkViewItk4DScalarImageInteractor::dataHandled()
{
    QStringList d = QStringList() << "medItkChar4ImageData"
                                  << "medItkUChar4ImageData"
                                  << "medItkShort4ImageData"
                                  << "medItkUShort4ImageData"
                                  << "medItkInt4ImageData"
                                  << "medItkUInt4ImageData"
                                  << "medItkLong4ImageData"
                                  << "medItkULong4ImageData"
                                  << "medItkFloat4ImageData"
                                  << "medItkDouble4ImageData";
    return  d;
}

bool medVtkViewItk4DScalarImageInteractor::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    return factory->registerInteractor<medVtkViewItk4DScalarImageInteractor>("medVtkViewItk4DScalarImageInteractor",
                                                                          QStringList () << "medVtkView" <<
                                                                          medVtkViewItk4DScalarImageInteractor::dataHandled());
}

void medVtkViewItk4DScalarImageInteractor::setInputData(medAbstractData *data)
{
    d->imageData = dynamic_cast<medAbstractImageData *>(data);
    if(!d->imageData)
        return;

    if( data->identifier().contains("medItk") && data->identifier().contains("ImageData") &&  d->imageData->dimension() == 4 )
    {
        d->sequence = vtkMetaDataSetSequence::New();

        int layer = d->view->layer(data);

        if (  AppendImageSequence<char>(data,d->view,d->sequence, layer)           ||
              AppendImageSequence<unsigned char>(data,d->view,d->sequence, layer)  ||
              AppendImageSequence<short>(data,d->view,d->sequence, layer)          ||
              AppendImageSequence<unsigned short>(data,d->view,d->sequence, layer) ||
              AppendImageSequence<int>(data,d->view,d->sequence, layer)            ||
              AppendImageSequence<unsigned int>(data,d->view,d->sequence, layer)   ||
              AppendImageSequence<long>(data,d->view,d->sequence, layer)           ||
              AppendImageSequence<unsigned long>(data,d->view,d->sequence, layer)  ||
              AppendImageSequence<float>(data,d->view,d->sequence, layer)          ||
              AppendImageSequence<double>(data,d->view,d->sequence, layer))
        {
            d->imageData->addMetaData("SequenceDuration", QString::number(d->sequence->GetMaxTime()));
            d->imageData->addMetaData("SequenceFrameRate", QString::number((double)d->sequence->GetNumberOfMetaDataSets() /
                                                                           (double)d->sequence->GetMaxTime()));

            qDebug() << "SequenceDuration" << d->sequence->GetMaxTime();
            qDebug() << "SequenceFrameRate" <<(double)d->sequence->GetNumberOfMetaDataSets() / (double)d->sequence->GetMaxTime();

            d->view2d->GetImageActor(d->view2d->GetCurrentLayer())->GetProperty()->SetInterpolationTypeToCubic();
            initParameters(d->imageData);

            double* range = d->sequence->GetCurrentScalarRange();
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

medAbstractData *medVtkViewItk4DScalarImageInteractor::inputData() const
{
    return d->imageData;
}

QWidget* medVtkViewItk4DScalarImageInteractor::buildToolBoxWidget()
{
    QWidget *toolBoxWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout(toolBoxWidget);
    tbLayout->addWidget(medVtkViewItkScalarImageInteractor::buildToolBoxWidget());

    return toolBoxWidget;
}

QWidget* medVtkViewItk4DScalarImageInteractor::buildToolBarWidget()
{
    return medVtkViewItkScalarImageInteractor::buildToolBarWidget();
}

QWidget* medVtkViewItk4DScalarImageInteractor::buildLayerWidget()
{
    return medVtkViewItkScalarImageInteractor::buildLayerWidget();
}

QList<medAbstractParameter*> medVtkViewItk4DScalarImageInteractor::linkableParameters()
{
    QList<medAbstractParameter*> parameters;
    parameters << medVtkViewItkScalarImageInteractor::linkableParameters();
    return parameters;
}

void medVtkViewItk4DScalarImageInteractor::setCurrentTime(double time)
{
    if(d->sequence->GetTime() == time)
        return;
    d->sequence->UpdateToTime(time);
}

void medVtkViewItk4DScalarImageInteractor::updateWidgets()
{
    medVtkViewItkScalarImageInteractor::updateWidgets();
}


