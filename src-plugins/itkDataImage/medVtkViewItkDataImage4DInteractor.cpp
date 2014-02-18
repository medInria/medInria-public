#include "medVtkViewItkDataImage4DInteractor.h"

#include <medAbstractImageView.h>
#include <medVtkViewBackend.h>
#include <medImageViewFactory.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <medTimeLineParameter.h>


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

    medTimeLineParameter *timeLineParameter;

    double currentTime;

};

template <typename TYPE>
bool AppendImageSequence(medAbstractData* data,medAbstractImageView* view,vtkMetaDataSetSequence* sequence) {

    if (itk::Image<TYPE,4>* image = dynamic_cast<itk::Image<TYPE,4>*>(static_cast<itk::Object*>(data->data()))) {
        unsigned int layer = view->layersCount();

        medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(view->backend());

        if (layer==1 && !backend->view2D->GetInput())
            layer = 0;
        vtkMetaDataSetSequence* metaDataSetSequence = vtkMetaDataSetSequence::New();
        metaDataSetSequence->SetITKDataSet<TYPE>(image);
        sequence = metaDataSetSequence;
        vtkMetaImageData* metaimage = vtkMetaImageData::SafeDownCast(metaDataSetSequence->GetMetaDataSet(0U));
        vtkImageData*     vtkimage  = vtkImageData::SafeDownCast(metaDataSetSequence->GetDataSet());

        backend->view2D->SetInput(vtkimage,metaimage->GetOrientationMatrix(),layer);
        backend->view3D->SetInput(vtkimage,metaimage->GetOrientationMatrix(),layer);

        return true;
    }
    return false;
}

medVtkViewItkDataImage4DInteractor::medVtkViewItkDataImage4DInteractor(medAbstractImageView* parent): medVtkViewItkDataImageInteractor(parent), d(new medVtkViewItkDataImage4DInteractorPrivate)
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

    if (data->identifier().contains("Image") && data->identifier().contains ("4")) {

        if (!(AppendImageSequence<char>(data,d->view,d->sequence)           ||
              AppendImageSequence<unsigned char>(data,d->view,d->sequence)  ||
              AppendImageSequence<short>(data,d->view,d->sequence)          ||
              AppendImageSequence<unsigned short>(data,d->view,d->sequence) ||
              AppendImageSequence<float>(data,d->view,d->sequence)          ||
              AppendImageSequence<double>(data,d->view,d->sequence))) {

            d->timeLineParameter = new medTimeLineParameter("Time");

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
}

QWidget* medVtkViewItkDataImage4DInteractor::toolBoxWidget()
{
    return d->timeLineParameter->getWidget();
}

QWidget* medVtkViewItkDataImage4DInteractor::toolBarWidget()
{
    return new QWidget();
}

QWidget* medVtkViewItkDataImage4DInteractor::layerWidget()
{
    return new QWidget();
}

QList<medAbstractParameter*> medVtkViewItkDataImage4DInteractor::parameters()
{
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

