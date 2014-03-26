#include "medVtkViewItkDataImage4DInteractor.h"

#include <medAbstractImageView.h>
#include <medVtkViewBackend.h>
#include <medImageViewFactory.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkActor.h>
#include <vtkProperty.h>
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
    QWidget *toolBoxWidget;

    double currentTime;

};

template <typename TYPE>
bool AppendImageSequence(medAbstractData* data,medAbstractImageView* view,vtkMetaDataSetSequence* sequence, int& layer) {

    if (itk::Image<TYPE,4>* image = dynamic_cast<itk::Image<TYPE,4>*>(static_cast<itk::Object*>(data->data()))) {

        medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(view->backend());

        sequence->SetITKDataSet<TYPE>(image);

        vtkMetaImageData* metaimage = vtkMetaImageData::SafeDownCast(sequence->GetMetaDataSet(0U));
        vtkImageData*     vtkimage  = vtkImageData::SafeDownCast(sequence->GetDataSet());

        backend->view2D->AddInput(vtkimage,metaimage->GetOrientationMatrix());
        backend->view3D->AddInput(vtkimage,metaimage->GetOrientationMatrix());

        layer = backend->view2D->GetNumberOfLayers()-1;

        return true;
    }
    return false;
}

medVtkViewItkDataImage4DInteractor::medVtkViewItkDataImage4DInteractor(medAbstractImageView* parent):
    medVtkViewItkDataImageInteractor(parent), d(new medVtkViewItkDataImage4DInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView *>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->render = backend->renWin;

    d->toolBoxWidget = NULL;

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
    medImageViewFactory *factory = medImageViewFactory::instance();
    return factory->registerInteractor<medVtkViewItkDataImage4DInteractor>("medVtkViewItkDataImage4DInteractor",
                                                                          QStringList () << "medVtkView" <<
                                                                          medVtkViewItkDataImage4DInteractor::dataHandled());
}

void medVtkViewItkDataImage4DInteractor::setData(medAbstractData *data)
{
    double range[2]={0,0};
    double mintimestep, mintime, maxtime;
    unsigned int numberofsteps;

    d->imageData = dynamic_cast<medAbstractImageData *>(data);
    if(!d->imageData)
        return;

    if( data->identifier().contains("itkDataImage") &&  d->imageData->Dimension() == 4 ) {

        d->sequence = vtkMetaDataSetSequence::New();

        int layer = -1;

        if (  AppendImageSequence<char>(data,d->view,d->sequence, layer)           ||
              AppendImageSequence<unsigned char>(data,d->view,d->sequence, layer)  ||
              AppendImageSequence<short>(data,d->view,d->sequence, layer)          ||
              AppendImageSequence<unsigned short>(data,d->view,d->sequence, layer) ||
              AppendImageSequence<float>(data,d->view,d->sequence, layer)          ||
              AppendImageSequence<double>(data,d->view,d->sequence, layer)) {

            initParameters(d->imageData);
            setImageViewInternalLayer(layer);

            d->timeLineParameter = new medTimeLineParameter("Time", this);

            sequencesRange (range);
            mintimestep = sequencesMinTimeStep();
            mintime = range[0];
            maxtime = range[1];

            numberofsteps = std::ceil ((maxtime - mintime) / (mintimestep) + 1.0);

            d->timeLineParameter->setNumberOfFrame(numberofsteps);
            d->timeLineParameter->setDuration((maxtime+mintimestep));

            connect(d->timeLineParameter, SIGNAL(frameChanged(double)), this, SLOT(setCurrentTime(double)));
        }
    }
}

medAbstractData *medVtkViewItkDataImage4DInteractor::data() const
{
    return d->imageData;
}

QWidget* medVtkViewItkDataImage4DInteractor::toolBoxWidget()
{
    if(!d->toolBoxWidget)
    {
        d->toolBoxWidget = new QWidget;
        connect(d->toolBoxWidget, SIGNAL(destroyed()), this, SLOT(removeInternToolBox()));
        QVBoxLayout *tbLayout = new QVBoxLayout(d->toolBoxWidget);
        tbLayout->addWidget(medVtkViewItkDataImageInteractor::toolBoxWidget());
        tbLayout->addWidget(d->timeLineParameter->getWidget());
    }

    return d->toolBoxWidget;
}

QWidget* medVtkViewItkDataImage4DInteractor::toolBarWidget()
{
    return medVtkViewItkDataImageInteractor::toolBarWidget();
}

QWidget* medVtkViewItkDataImage4DInteractor::layerWidget()
{
    return medVtkViewItkDataImageInteractor::layerWidget();
}

QList<medAbstractParameter*> medVtkViewItkDataImage4DInteractor::parameters()
{
    QList<medAbstractParameter*> parameters;
    parameters << medVtkViewItkDataImageInteractor::parameters();
    parameters << d->timeLineParameter;
    return parameters;
}

void medVtkViewItkDataImage4DInteractor::setCurrentTime (double time)
{
    if (!d->view)
        return;

    double range[2] = {0,0};
    this->sequencesRange(range);

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

void medVtkViewItkDataImage4DInteractor::sequencesRange (double* range)
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

double medVtkViewItkDataImage4DInteractor::sequencesMinTimeStep()
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

}

void medVtkViewItkDataImage4DInteractor::removeInternToolBox()
{
    d->toolBoxWidget = NULL;
}

