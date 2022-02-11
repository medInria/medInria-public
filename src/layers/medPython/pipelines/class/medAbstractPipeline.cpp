#include "medAbstractPipeline.h"

#include <medAbstractMeshData.h>

#include "medPipelineFactory.h"
#include "medPipelineValuesToolBox.h"

namespace med::pipeline
{

class AbstractPipelinePrivate
{
public:
    QSharedPointer<ReformatDelegate> reformatDelegate;
    QSharedPointer<RegistrationDelegate> registrationDelegate;
    QSharedPointer<SegmentationDelegate> segmentationDelegate;
    QSharedPointer<FilteringDelegate> filteringDelegate;
    QSharedPointer<RoiToolboxDelegate> roiToolboxDelegate;
};

AbstractPipeline::AbstractPipeline()
    : d(new AbstractPipelinePrivate())
{
    PipelineDelegate* delegate;

    delegate = PipelineFactory::instance()->createDelegate(ReformatDelegate::staticMetaObject.className());
    d->reformatDelegate = QSharedPointer<ReformatDelegate>(dynamic_cast<ReformatDelegate*>(delegate));

    delegate = PipelineFactory::instance()->createDelegate(RegistrationDelegate::staticMetaObject.className());
    d->registrationDelegate = QSharedPointer<RegistrationDelegate>(dynamic_cast<RegistrationDelegate*>(delegate));

    delegate = PipelineFactory::instance()->createDelegate(SegmentationDelegate::staticMetaObject.className());
    d->segmentationDelegate = QSharedPointer<SegmentationDelegate>(dynamic_cast<SegmentationDelegate*>(delegate));

    delegate = PipelineFactory::instance()->createDelegate(FilteringDelegate::staticMetaObject.className());
    d->filteringDelegate = QSharedPointer<FilteringDelegate>(dynamic_cast<FilteringDelegate*>(delegate));

    delegate = PipelineFactory::instance()->createDelegate(RoiToolboxDelegate::staticMetaObject.className());
    d->roiToolboxDelegate = QSharedPointer<RoiToolboxDelegate>(dynamic_cast<RoiToolboxDelegate*>(delegate));
}

AbstractPipeline::~AbstractPipeline()
{
    delete d;
}

ReformatDelegate* AbstractPipeline::getReformatDelegate()
{
    return d->reformatDelegate.data();
}

RegistrationDelegate* AbstractPipeline::getRegistrationDelegate()
{
    return d->registrationDelegate.data();
}

SegmentationDelegate* AbstractPipeline::getSegmentationDelegate()
{
    return d->segmentationDelegate.data();
}

FilteringDelegate* AbstractPipeline::getFilteringDelegate()
{
    return d->filteringDelegate.data();
}

RoiToolboxDelegate *AbstractPipeline::getRoiToolboxDelegate()
{
    return d->roiToolboxDelegate.data();
}

PipelineStepProperties AbstractPipeline::createDisplayResultsStep(QString name)
{
    return PipelineStepProperties(name)
            .setTitle("Display results")
            .setDescription("Visualisation of the results");
}

void AbstractPipeline::startDisplayResultsStep(QList<dtkSmartPointer<medAbstractData> > resultData, QHash<QString, QVariant> resultValues)
{
    bool onlyMeshData = true;

    for (int i = 0; i < resultData.length(); i++)
    {
        medAbstractData* data = resultData[i];

        if (!dynamic_cast<medAbstractMeshData*>(data))
        {
            onlyMeshData = false;
        }

        getWorkspace()->addData(resultData[i]);
    }

    if (onlyMeshData && (resultData.length() > 0))
    {
        getWorkspace()->setVolumeRendering();
    }

    getWorkspace()->addToolBox("med::pipeline::PipelineExportToolBox");

    ValuesToolBox* valuesToolBox = getWorkspace()->addToolBox<ValuesToolBox>();
    valuesToolBox->addValues(resultValues);
}

PipelineStepProperties AbstractPipeline::createDelegateDisplayResultsStep()
{
    return createDisplayResultsStep("DisplayResults");
}

void AbstractPipeline::startDisplayResultsStep(PipelineWorkingState* state)
{
    QList<QString> orderedData;
    foreach (QString desired,  this->getMeshOrder())
    {
        if (state->getAllMedData().contains(desired))
        {
            orderedData.append(desired);
        }
    }
    foreach (QString remaining, state->getAllMedData())
    {
        if (!orderedData.contains(remaining))
        {
            orderedData.append(remaining);
        }
    }

    QList<dtkSmartPointer<medAbstractData> > resultData;
    foreach (QString identifier, orderedData)
    {
        dtkSmartPointer<medAbstractData> data = state->peekValue(identifier);
        foreach (medAttachedData* attachedData, data->attachedData())
        {
            PipelineAttachedData* pipelineAttachedData = dynamic_cast<PipelineAttachedData*>(attachedData);
            if (pipelineAttachedData && pipelineAttachedData->isFinalResult())
            {
                resultData.append(data);
                break;
            }
        }
    }
    QHash<QString, QVariant> resultValues = state->peekValue<QHash<QString, QVariant> >(med::pipeline::resultValuesId);
    startDisplayResultsStep(resultData, resultValues);
}

} // namespace med::pipeline
