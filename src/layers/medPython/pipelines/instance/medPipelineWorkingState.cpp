#include "medPipelineWorkingState.h"

#include <medAbstractDataFactory.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medUtilities.h>

#include "medPipelineAttachedData.h"
#include "medPipelineUtils.h"

namespace med::pipeline
{

class PipelineWorkingStatePrivate
{
public:
    QHash<QString, dtkSmartPointer<medAbstractData> > medDataCache;
    int step;
    QList<QString> modifiedData;
    bool wasRejected;
    QString rejectionMessage;
    QMutex importMutex;
    int importCounter;
    QHash<QUuid, QString> currentImports;
    QHash<QString, QVariant> stepOptions;
};

QString PipelineWorkingState::seriesDescriptionForStepData(int step, QString identifier)
{
    QString seriesDescription;
    seriesDescription = QString("%1").arg(step + 1, 2, 10, QChar('0')) + "_" + identifier;
    if (seriesDescription.endsWith("Id"))
    {
        seriesDescription.chop(2);
    }
    return seriesDescription;
}

PipelineWorkingState::PipelineWorkingState()
    : d(new PipelineWorkingStatePrivate())
{
    d->wasRejected = false;
}

PipelineWorkingState::PipelineWorkingState(const PipelineWorkingState& other)
    : PipelineState(other), d(new PipelineWorkingStatePrivate())
{
    d->medDataCache = other.d->medDataCache;
    d->step = other.d->step;
    d->wasRejected = other.d->wasRejected;
    d->rejectionMessage = other.d->rejectionMessage;
    d->stepOptions = other.d->stepOptions;
}

PipelineWorkingState::PipelineWorkingState(const PipelineState& referenceState, int step,
                                                 QHash<QString, QVariant> stepOptions)
    : d(new PipelineWorkingStatePrivate())
{
    if (step == -1)
    {
        copyMarkers(referenceState);
    }
    copyValues(referenceState);
    d->step = step;
    d->stepOptions = stepOptions;
    d->wasRejected = false;
}

PipelineWorkingState::~PipelineWorkingState()
{
    delete d;
}

void PipelineWorkingState::importNewMedData()
{
    QList<QString> newMedData;

    foreach (QString identifier, getWrittenData())
    {
        if (d->medDataCache.contains(identifier))
        {
            newMedData.append(identifier);
        }
    }

    if (!newMedData.isEmpty())
    {
        d->importCounter = newMedData.length();

        connect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex, QUuid)),
                this, SLOT(dataImported(medDataIndex, QUuid)));

        foreach (QString identifier, newMedData)
        {
            QMutexLocker locker(&d->importMutex);

            dtkSmartPointer<medAbstractData> data = retrieveMedData(identifier);
            setMetaDataForDataToImport(identifier, data);
            QUuid importId = medDataManager::instance()->importData(data, true);
            d->currentImports[importId] = identifier;
        }

        waitForImportsFinished();

        disconnect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex,QUuid)), this, nullptr);
    }
}

void PipelineWorkingState::applyTo(PipelineState& state) const
{
    if (d->step == -1)
    {
        d->modifiedData = getWrittenData();
        foreach (QString identifier, d->modifiedData)
        {
            if (!d->medDataCache.contains(identifier) && isMedData(identifier))
            {
                d->modifiedData.removeOne(identifier);
            }
        }
    }
    else
    {
        QList<QString> previouslyWrittenData = state.getWrittenData();
        d->modifiedData = previouslyWrittenData;

        foreach (QString identifier, getWrittenData())
        {
            if (previouslyWrittenData.contains(identifier))
            {
                if (state.isMedData(identifier))
                {
                    medDataManager::instance()->removeData(state.getQVariantValue(identifier).value<medDataIndex>());
                }
            }
            else
            {
                d->modifiedData.append(identifier);
            }
        }
    }

    state = PipelineState(*this);
}

QList<QString> PipelineWorkingState::getModifiedData()
{
    return d->modifiedData;
}

void PipelineWorkingState::setValue(QString identifier, medAbstractData* value)
{
    setValue(identifier, dtkSmartPointer<medAbstractData>(value));
}

void PipelineWorkingState::setValue(QString identifier, dtkSmartPointer<medAbstractData> value)
{
    markAsWritten(identifier);
    setQVariantValue(identifier, QVariant::fromValue<medDataIndex>(medDataIndex()));
    addToMedDataCache(identifier, value);
}

dtkSmartPointer<medAbstractData> PipelineWorkingState::getValue(QString identifier)
{
    markAsViewed(identifier);
    return retrieveMedData(identifier);
}

dtkSmartPointer<medAbstractData> PipelineWorkingState::peekValue(QString identifier)
{
    return retrieveMedData(identifier);
}

bool PipelineWorkingState::hasStepOption(QString name)
{
    return d->stepOptions.contains(name);
}

QVariant PipelineWorkingState::getStepOption(QString name)
{
    return d->stepOptions[name];
}

void PipelineWorkingState::reject(QString message)
{
    d->wasRejected = true;
    d->rejectionMessage = message;
}

bool PipelineWorkingState::wasRejected()
{
    return d->wasRejected;
}

QString PipelineWorkingState::rejectionMessage()
{
    return d->rejectionMessage;
}

void PipelineWorkingState::setWindowLevel(QString identifier, double window, double level)
{
    QHash<QString, QVariant> windowLevel;
    windowLevel["Window"] = windowLevel;
    windowLevel["Level"] = level;
    setWindowLevel(identifier, windowLevel);
}

void PipelineWorkingState::setWindowLevel(medAbstractData* value, double window, double level)
{
    foreach (QString identifier, findMedDataIdentifiers(value))
    {
        setWindowLevel(identifier, window, level);
    }
}

void PipelineWorkingState::setWindowLevel(QString identifier, QHash<QString, QVariant> windowLevel)
{
    setValue(identifier + ".windowLevel", windowLevel);
}

void PipelineWorkingState::setWindowLevel(medAbstractData* value, QHash<QString, QVariant> windowLevel)
{
    foreach (QString identifier, findMedDataIdentifiers(value))
    {
        setWindowLevel(identifier, windowLevel);
    }
}

void PipelineWorkingState::copyWindowLevel(QString targetId, QString sourceId)
{
    QString windowLevelId = sourceId + ".windowLevel";

    if (hasValue(windowLevelId))
    {
        QHash<QString, QVariant> windowLevel = peekValue<QHash<QString, QVariant> >(windowLevelId);
        setWindowLevel(targetId, windowLevel);

        if (d->medDataCache.contains(targetId))
        {
            dtkSmartPointer<PipelineAttachedData> attachedData = PipelineUtils::getAttachedData(getValue(targetId), true);
            attachedData->setWindowLevel(windowLevel["Window"].value<double>(), windowLevel["Level"].value<double>());
        }
    }
}

void PipelineWorkingState::copyWindowLevel(medAbstractData* targetValue, medAbstractData* sourceValue)
{
    dtkSmartPointer<PipelineAttachedData> attachedData = PipelineUtils::getAttachedData(sourceValue, false);

    if (attachedData)
    {
        QHash<QString, QVariant> windowLevel;
        windowLevel["Window"] = attachedData->getWindow();
        windowLevel["Level"] = attachedData->getLevel();

        foreach (QString identifier, findMedDataIdentifiers(targetValue))
        {
            setWindowLevel(identifier, windowLevel);
        }
    }
}

void PipelineWorkingState::setRenderingParameters(QString identifier, RenderingParameters renderingParameters)
{
    setValue(identifier + ".renderingParameters", renderingParameters);
}

void PipelineWorkingState::setRenderingParameters(medAbstractData* value, RenderingParameters renderingParameters)
{
    foreach (QString identifier, findMedDataIdentifiers(value))
    {
        setRenderingParameters(identifier, renderingParameters);
    }
}

void PipelineWorkingState::setFinalResult(QString identifier, bool isFinalResult)
{
    setValue(identifier + ".isFinalResult", isFinalResult);
}

void PipelineWorkingState::setFinalResult(medAbstractData* data, bool isFinalResult)
{
    foreach (QString identifier, findMedDataIdentifiers(data))
    {
        setFinalResult(identifier, isFinalResult);
    }
}

QStringList PipelineWorkingState::findMedDataIdentifiers(medAbstractData* value)
{
    QStringList result;
    QHash<QString, dtkSmartPointer<medAbstractData> >::const_iterator iterator;

    for (iterator = d->medDataCache.constBegin(); iterator != d->medDataCache.constEnd(); iterator++)
    {
        if (iterator.value() == value)
        {
            result.append(iterator.key());
        }
    }

    medDataIndex dataIndex = value->dataIndex();

    if (dataIndex.isValidForSeries())
    {
        result.append(PipelineState::findMedDataIdentifiers(QVariant::fromValue<medDataIndex>(dataIndex)));
    }

    return result;
}

void PipelineWorkingState::addToMedDataCache(QString identifier, dtkSmartPointer<medAbstractData> value)
{
    d->medDataCache[identifier] = value;
}

void PipelineWorkingState::removeFromMedDataCache(QString identifier)
{
    d->medDataCache.remove(identifier);
}

dtkSmartPointer<medAbstractData> PipelineWorkingState::retrieveMedData(QString identifier)
{
    if (d->medDataCache.contains(identifier))
    {
        return d->medDataCache[identifier];
    }

    if (isMedData(identifier))
    {
        medDataIndex dataIndex = getQVariantValue(identifier).value<medDataIndex>();

        if (dataIndex.isValidForSeries())
        {
            dtkSmartPointer<medAbstractData> data = medDataManager::instance()->retrieveData(dataIndex);

            if (data && (data->metadata(medMetaDataKeys::SeriesInstanceUID.key()) == getSeriesInstanceUID(identifier)))
            {
                addToMedDataCache(identifier, data);
                data->setDataIndex(dataIndex);

                dtkSmartPointer<PipelineAttachedData> attachedData = PipelineUtils::getAttachedData(data, true);

                QString windowLevelId = identifier + ".windowLevel";
                if (hasValue(windowLevelId))
                {
                    QHash<QString, QVariant> windowLevel = peekValue<QHash<QString, QVariant> >(windowLevelId);
                    attachedData->setWindowLevel(windowLevel["Window"].value<double>(), windowLevel["Level"].value<double>());
                }

                QString renderingParametersId = identifier + ".renderingParameters";
                if (hasValue(renderingParametersId))
                {
                    attachedData->setRenderingParameters(peekValue<RenderingParameters>(renderingParametersId));
                }

                QString isFinalResultId = identifier + ".isFinalResult";

                if (hasValue(isFinalResultId))
                {
                    attachedData->setFinalResult(peekValue<bool>(isFinalResultId));
                }

                return data;
            }
        }
    }

    return nullptr;
}

void PipelineWorkingState::setMetaDataForDataToImport(QString identifier, dtkSmartPointer<medAbstractData> data)
{
    data->setMetaData(medMetaDataKeys::SeriesDescription.key(), seriesDescriptionForStepData(d->step, identifier));

    QHash<QString, QString> sharedMetaData = getQVariantValue(sharedMetaDataId).value<QHash<QString, QString> >();
    foreach (QString key, sharedMetaData.keys())
    {
        data->setMetaData(key, sharedMetaData[key]);
    }

    medUtilities::generateSeriesAndSOPInstanceId(data);
    setSeriesInstanceUID(identifier, data->metadata(medMetaDataKeys::SeriesInstanceUID.key()));
}

void PipelineWorkingState::waitForImportsFinished()
{
    QEventLoop loop;
    connect(this, SIGNAL(importsFinished()), &loop, SLOT(quit()));
    loop.exec();
}

void PipelineWorkingState::dataImported(const medDataIndex& index, QUuid importId)
{
    QMutexLocker locker(&d->importMutex);

    if (d->currentImports.contains(importId))
    {
        QString identifier = d->currentImports.take(importId);
        setQVariantValue(identifier, QVariant::fromValue<medDataIndex>(index));

        d->importCounter--;
        if (d->importCounter == 0)
        {
            emit importsFinished();
        }
    }
}

} // namespace med::pipeline

