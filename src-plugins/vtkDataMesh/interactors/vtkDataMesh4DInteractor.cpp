#include "vtkDataMesh4DInteractor.h"

#include <medAbstractImageView.h>
#include <medVtkViewBackend.h>
#include <medViewFactory.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <medTimeLineParameter.h>


class vtkDataMesh4DInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;

    medAbstractData *data;
    medAbstractImageView *view;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkRenderWindow *render;

    vtkMetaDataSetSequence *sequence;

    medTimeLineParameter *timeLineParameter;

    double currentTime;

};


vtkDataMesh4DInteractor::vtkDataMesh4DInteractor(medAbstractView* parent): medAbstractInteractor(parent), d(new vtkDataMesh4DInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView *>(parent);

    d->data = 0;

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->render = backend->renWin;

    d->currentTime = 0.0;
}

vtkDataMesh4DInteractor::~vtkDataMesh4DInteractor()
{

}

QString vtkDataMesh4DInteractor::description() const
{
    return tr("Interactor displaying 4D Meshes");
}

QString vtkDataMesh4DInteractor::identifier() const
{
    return "vtkDataMesh4DInteractor";
}

QStringList vtkDataMesh4DInteractor::handled() const
{
    return vtkDataMesh4DInteractor::dataHandled();
}

QStringList vtkDataMesh4DInteractor::dataHandled()
{
    QStringList d = QStringList() << "vtkDataMesh4D";
    return  d;
}

bool vtkDataMesh4DInteractor::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    return factory->registerAdditionalInteractor<vtkDataMesh4DInteractor>("vtkDataMesh4DInteractor",
                                                                          QStringList () << "medVtkView" <<
                                                                          vtkDataMesh4DInteractor::dataHandled());
}

void vtkDataMesh4DInteractor::setData(medAbstractData *data)
{
    double range[2]={0,0};
    double mintimestep, mintime, maxtime;
    unsigned int numberofsteps;


    if (data->identifier() == "vtkDataMesh4D" )
    {
        d->data = data;

        vtkMetaDataSetSequence *sequence = dynamic_cast<vtkMetaDataSetSequence *>((vtkDataObject *)(data->data()));
        //vtkProperty *prop = vtkProperty::New();
        if ( !sequence )
            return;

        switch (sequence->GetType())
        {
        case vtkMetaDataSet::VTK_META_SURFACE_MESH:
        case vtkMetaDataSet::VTK_META_VOLUME_MESH:
            d->sequence = sequence;

            d->timeLineParameter = new medTimeLineParameter("TimeLine", this);

            sequencesRange (range);
            mintimestep = sequencesMinTimeStep();
            mintime = range[0];
            maxtime = range[1];

            numberofsteps = std::ceil ((maxtime - mintime) / (mintimestep) + 1.0);
            d->timeLineParameter->setNumberOfFrame(numberofsteps);
            d->timeLineParameter->setDuration((maxtime+mintimestep));
            connect(d->timeLineParameter, SIGNAL(frameChanged(double)), this, SLOT(setCurrentTime(double)));
            break;
        default:
            break;
        }
    }
}

medAbstractData *vtkDataMesh4DInteractor::data() const
{
    return d->data;
}

QWidget* vtkDataMesh4DInteractor::buildToolBoxWidget()
{
    return d->timeLineParameter->getWidget();
}

QWidget* vtkDataMesh4DInteractor::buildToolBarWidget()
{
    return new QWidget;
}

QWidget* vtkDataMesh4DInteractor::buildLayerWidget()
{
    return new QWidget;
}

QList<medAbstractParameter*> vtkDataMesh4DInteractor::linkableParameters()
{
    QList<medAbstractParameter*> parameters;
    parameters << d->timeLineParameter;
    return parameters;
}


void vtkDataMesh4DInteractor::setCurrentTime (double time)
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

double vtkDataMesh4DInteractor::getCurrentTime()
{
    return d->currentTime;
}

void vtkDataMesh4DInteractor::sequencesRange (double* range)
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

double vtkDataMesh4DInteractor::sequencesMinTimeStep()
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

