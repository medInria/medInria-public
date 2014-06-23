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
#include <medTimeLineParameter.h>
#include <medAbstractImageData.h>


class medVtkViewItkDataImage4DInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;

    medAbstractImageView *view;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkRenderWindow *render;

    vtkMetaDataSetSequence *sequence;
    medAbstractImageData *imageData;

    medTimeLineParameter *timeLineParameter;
    double currentTime;

};

template <typename TYPE>
bool AppendImageSequence(medAbstractData* data,medAbstractImageView* view,vtkMetaDataSetSequence* sequence, int& layer) {

    if (itk::Image<TYPE,4>* image = dynamic_cast<itk::Image<TYPE,4>*>(static_cast<itk::Object*>(data->data()))) {

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

medVtkViewItkDataImage4DInteractor::medVtkViewItkDataImage4DInteractor(medAbstractView *parent):
    medVtkViewItkDataImageInteractor(parent), d(new medVtkViewItkDataImage4DInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView *>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->render = backend->renWin;

    d->currentTime = 0.0;
}

medVtkViewItkDataImage4DInteractor::~medVtkViewItkDataImage4DInteractor()
{

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
    double range[2]={0,0};
    double mintimestep, mintime, maxtime;

    d->imageData = dynamic_cast<medAbstractImageData *>(data);
    if(!d->imageData)
        return;

    if( data->identifier().contains("itkDataImage") &&  d->imageData->Dimension() == 4 ) {

        d->sequence = vtkMetaDataSetSequence::New();

        int layer = d->view->layer(data);

        if (  AppendImageSequence<char>(data,d->view,d->sequence, layer)           ||
              AppendImageSequence<unsigned char>(data,d->view,d->sequence, layer)  ||
              AppendImageSequence<short>(data,d->view,d->sequence, layer)          ||
              AppendImageSequence<unsigned short>(data,d->view,d->sequence, layer) ||
              AppendImageSequence<float>(data,d->view,d->sequence, layer)          ||
              AppendImageSequence<double>(data,d->view,d->sequence, layer))
        {

            d->view2d->GetImageActor(d->view2d->GetCurrentLayer())->GetProperty()->SetInterpolationTypeToCubic();
            initParameters(d->imageData);

            d->timeLineParameter = new medTimeLineParameter("TimeLine", this);

            this->timeRange(range);
            mintimestep = this->frameRate();

            mintime = range[0];
            maxtime = range[1];

            d->timeLineParameter->setNumberOfFrame(d->sequence->GetNumberOfMetaDataSets());
            d->timeLineParameter->setDuration((maxtime));

            qDebug() << d->sequence->GetTimeResolution();

            connect(d->timeLineParameter, SIGNAL(frameChanged(double)), this, SLOT(setCurrentTime(double)));

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

medAbstractData *medVtkViewItkDataImage4DInteractor::inputData() const
{
    return d->imageData;
}

QWidget* medVtkViewItkDataImage4DInteractor::buildToolBoxWidget()
{
    QWidget *toolBoxWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout(toolBoxWidget);
    tbLayout->addWidget(medVtkViewItkDataImageInteractor::buildToolBoxWidget());
    tbLayout->addWidget(d->timeLineParameter->getWidget());

    return toolBoxWidget;
}

QWidget* medVtkViewItkDataImage4DInteractor::buildToolBarWidget()
{
    return medVtkViewItkDataImageInteractor::buildToolBarWidget();
}

QWidget* medVtkViewItkDataImage4DInteractor::buildLayerWidget()
{
    return medVtkViewItkDataImageInteractor::buildLayerWidget();
}

QList<medAbstractParameter*> medVtkViewItkDataImage4DInteractor::linkableParameters()
{
    QList<medAbstractParameter*> parameters;
    parameters << medVtkViewItkDataImageInteractor::linkableParameters();
    parameters << d->timeLineParameter;
    return parameters;
}

void medVtkViewItkDataImage4DInteractor::setCurrentTime (double time)
{
    if (!d->view)
        return;

    double range[2] = {0,0};
    this->timeRange(range);

    time = std::min (range[1], time);
    time = std::max (range[0], time);

    d->currentTime = time;

    d->sequence->UpdateToTime (time);

    d->view2d->Modified();
    d->view2d->Render();

    d->view3d->Modified();
    d->view3d->Render();
}

double medVtkViewItkDataImage4DInteractor::getCurrentTime()
{
    return d->currentTime;
}

void medVtkViewItkDataImage4DInteractor::timeRange (double* range)
{
    if (!d->sequence)
    {
        range[0] = 0;
        range[1] = 1.0;
        return;
    }

    double mintime = 3000;
    double maxtime = -3000;

    mintime = std::min (mintime, d->sequence->GetMinTime());
    maxtime = std::max (maxtime, d->sequence->GetMaxTime());

    range[0] = mintime;
    range[1] = maxtime;
}

double medVtkViewItkDataImage4DInteractor::frameRate()
{
    if (!d->sequence)
    {
        return 0.01;
    }

    double step = 3000;

    double mintime = d->sequence->GetMinTime();
    double maxtime = d->sequence->GetMaxTime();
    double number = d->sequence->GetNumberOfMetaDataSets();

    step = std::min ( step, (maxtime - mintime)/(number - 1.0) );

    return step;
}

void medVtkViewItkDataImage4DInteractor::updateWidgets()
{
    medVtkViewItkDataImageInteractor::updateWidgets();
}


