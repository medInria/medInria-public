/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVtkViewItkDataImage4DInteractor.h"

#include <vtkActor.h>
#include <vtkImageActor.h>
#include <vtkImageProperty.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include <medAbstractImageView.h>
#include <medVtkViewBackend.h>
#include <medViewFactory.h>
#include <medAbstractImageData.h>
#include <medIntParameterL.h>
#include <medDoubleParameterL.h>
#include <medTimeLineParameterL.h>

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
    medAbstractImageData *imageData;

    medIntParameterL *slicingParameter;

    vtkSmartPointer<vtkTextActor> textActor;
};


medVtkViewItkDataImage4DInteractor::medVtkViewItkDataImage4DInteractor(medAbstractView *parent):
    medVtkViewItkDataImageInteractor(parent), d(new medVtkViewItkDataImage4DInteractorPrivate), m_poConv(nullptr)
{
    d->view = dynamic_cast<medAbstractImageView *>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->slicingParameter = nullptr;

    d->textActor = nullptr;
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

    if( data->identifier().contains("itkDataImage") &&  d->imageData->Dimension() == 4 )
    {
        int layer = d->view->layer(data);

        if (SetViewInput(data, layer) )
        {
            double frameRate = static_cast<double>(m_poConv->getNumberOfVolumes()) / static_cast<double>(m_poConv->getTotalTime());
            d->imageData->setMetaData("SequenceDuration", QString::number(m_poConv->getTotalTime()));
            d->imageData->setMetaData("SequenceFrameRate", QString::number(frameRate));

            qDebug() << "SequenceDuration" << m_poConv->getTotalTime();
            qDebug() << "SequenceFrameRate" << frameRate;

            d->view2d->GetImageActor(d->view2d->GetCurrentLayer())->GetProperty()->SetInterpolationTypeToCubic();
            initParameters(d->imageData);

            double* range = m_poConv->getCurrentScalarRange();
            d->view2d->SetColorRange(range);
            this->initWindowLevelParameters(range);

            createSlicingParam();

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

    auto *poOldConv = m_poConv;
    m_poConv = vtkItkConversionInterface::createInstance(data);

    if (m_poConv)
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
                d->view2d->SetInput(poVtkAlgoOutputPort, poMatrix, layer);
                d->view3d->SetInput(poVtkAlgoOutputPort, poMatrix, layer);
            }
        }
    }
    else
    {
        bRes = false;
    }

    delete poOldConv;

    return bRes;
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
    if (m_poConv->getTimeIndex() * m_poConv->getTotalTime() / m_poConv->getNumberOfVolumes() != time)
    {
        m_poConv->setTimeIndex( static_cast<unsigned int>(round(time * (m_poConv->getNumberOfVolumes()-1) / m_poConv->getTotalTime())) );
    }

    // Set the current time on the view if needed
    QString displayedTime = d->view->timeLineParameter()->getDisplayedTime();
    if (displayedTime != "")
    {
        // Refresh view size in case of resize
        QSize size = d->view->viewWidget()->size();
        int newSizeX = static_cast<int>(size.width()/90.0);
        int newSizeY = static_cast<int>(size.height()/1.5);

        // Display Time + Shift
        if (d->textActor == nullptr)
        {
            d->textActor = vtkSmartPointer<vtkTextActor>::New();
            d->textActor->SetInput(displayedTime.toStdString().c_str());
            d->textActor->SetDisplayPosition(newSizeX, newSizeY);
            d->textActor->GetTextProperty()->SetColor(1.0, 0.0, 0.0);
            d->textActor->GetTextProperty()->SetFontSize(20);

            d->view3d->GetRenderer()->AddViewProp(d->textActor);
            d->view2d->GetRenderer()->AddViewProp(d->textActor);
        }
        else
        {
            d->textActor->SetInput(displayedTime.toStdString().c_str());
            d->textActor->SetDisplayPosition(newSizeX, newSizeY);
        }
        d->view->render();
    }
    else
    {
        if (d->textActor != nullptr)
        {
            d->view3d->GetRenderer()->RemoveViewProp(d->textActor);
            d->view2d->GetRenderer()->RemoveViewProp(d->textActor);
            d->textActor = nullptr;
        }
    }
}
