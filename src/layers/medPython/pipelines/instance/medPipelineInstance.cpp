#include "medPipelineInstance.h"

#include <stdexcept>

#include <QHash>

#include <medAbstractDataFactory.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medTabbedViewContainers.h>
#include <medUtilities.h>

#include "medPipelineChooserToolBox.h"
#include "medPipelineControlToolBox.h"
#include "medPipelineFactory.h"
#include "medPipelineTypes.h"
#include "medPipelineWorkingState.h"

namespace med::pipeline
{

class PipelineInstancePrivate
{
public:
    QSharedPointer<AbstractPipeline> pipeline;
    QSharedPointer<PipelineDelegate> pipelineDelegate;
    int step;
    QList<PipelineStepProperties> stepProperties;
    QList<PipelineState> states;
    QSharedPointer<PipelineWorkingState> workingState;
    bool validationEnabled;
};

bool PipelineInstance::registered()
{
    return medAbstractDataFactory::instance()->registerDataType<PipelineInstance>();
}

PipelineInstance::PipelineInstance() :
    d(new PipelineInstancePrivate())
{
    initialize();
}

PipelineInstance::PipelineInstance(AbstractPipeline* pipeline) :
    d(new PipelineInstancePrivate())
{
    initialize();
    d->pipeline = QSharedPointer<AbstractPipeline>(pipeline);
}

PipelineInstance::PipelineInstance(const PipelineInstance& other) :
    d(new PipelineInstancePrivate())
{
    initialize();
    *d = *other.d;
}

PipelineInstance::~PipelineInstance()
{
    delete d;
}

bool PipelineInstance::read(QXmlStreamReader& in, bool readOnlyMetaData)
{
    while (in.readNextStartElement())
    {
        if (in.name() == "metaData")
        {
            readAndUpdateMetaData(in);
        }
        else if (in.name() == "steps")
        {
            if (!readOnlyMetaData)
            {
                readAndUpdateSteps(in);
            }
            else
            {
                in.skipCurrentElement();
            }
        }
        else
        {
            throwXMLException(in);
        }
    }

    return hasXMLError(in);
}

void PipelineInstance::write(QXmlStreamWriter& out)
{
    writeMetaData(out);
    writeSteps(out);
}

void PipelineInstance::setup()
{
    if (d->stepProperties.isEmpty())
    {
        d->stepProperties = d->pipeline->createSteps();

        for (int i = 0; i <= getNumSteps(); i++)
        {
            d->states.append(PipelineState());
        }
    }
    else
    {
        updateDataIndices();
    }

    setStep(-1);
    updateFromWorkingState();
}

QSharedPointer<AbstractPipeline> PipelineInstance::getPipeline()
{
    return d->pipeline;
}

QString PipelineInstance::getTitle()
{
    return d->pipeline->getTitle();
}

QString PipelineInstance::getAbbreviation()
{
    return d->pipeline->getAbbreviation();
}

int PipelineInstance::getNumSteps() const
{
    return d->stepProperties.length();
}

void PipelineInstance::setStep(int step)
{
    d->step = step;

    int referenceStateIndex;
    QHash<QString, QVariant> stepOptions;

    if (step == -1)
    {
        referenceStateIndex = step;
    }
    else
    {
        referenceStateIndex = step - 1;
        stepOptions = getStepProperties(step).getOptions();
    }

    PipelineState& referenceState = getStateAtEndOfStep(referenceStateIndex);
    d->workingState = QSharedPointer<PipelineWorkingState>(new PipelineWorkingState(referenceState, step, stepOptions));
}

int PipelineInstance::getStep()
{
    return d->step;
}

bool PipelineInstance::isProperStep()
{
    return (getStep() >= 0) && (getStep() < getNumSteps());
}

bool PipelineInstance::isFirstStep()
{
    return getStep() == 0;
}

bool PipelineInstance::isLastStep()
{
    return getStep() == (getNumSteps() - 1);
}

int PipelineInstance::getNextExecutableStep()
{
    for (int i = getStep() + 1; i < getNumSteps(); i++)
    {
        if (getStepProperties(i).isExecutable())
        {
            return i;
        }
    }
    return -1;
}

int PipelineInstance::getPreviousExecutableStep()
{
    for (int i = getStep() - 1; i >= 0; i--)
    {
        if (getStepProperties(i).isExecutable())
        {
            return i;
        }
    }
    return -1;
}

int PipelineInstance::getFirstExecutedStep()
{
    for (int i = 0; i < getNumSteps(); i++)
    {
        if (getStepProperties(i).wasExecuted())
        {
            return i;
        }
    }
    return -1;
}

PipelineStepProperties& PipelineInstance::getStepProperties(int step)
{
    return d->stepProperties[step];
}

PipelineStepProperties& PipelineInstance::getCurrentStepProperties()
{
    return getStepProperties(getStep());
}

QStringList PipelineInstance::getInitialDataIdentifiers()
{
    return d->pipeline->getInitialDataIdsAndNames().keys();
}

QString PipelineInstance::getInitialDataName(QString identifier)
{
    return d->pipeline->getInitialDataIdsAndNames()[identifier];
}

void PipelineInstance::setInitialDataValue(QString identifier, dtkSmartPointer<medAbstractData> value,
                                              QHash<QString, QVariant> windowLevel)
{
    if (!getInitialState().hasValue(sharedMetaDataId))
    {
        initializeMetaData(value);
    }

    setStep(-1);
    d->workingState->setValue(identifier, value);

    if (!windowLevel.isEmpty())
    {
        d->workingState->setWindowLevel(identifier, windowLevel);
    }

    updateFromWorkingState();
}

bool PipelineInstance::initialDataWasSet(QString identifier)
{
    return getInitialState().hasValue(identifier);
}

bool PipelineInstance::isValidationEnabled()
{
    return d->validationEnabled;
}

bool PipelineInstance::tryStartStep(QString* errorMessage)
{
    try
    {
        startStep();
    }
    catch (std::exception& exception)
    {
        *errorMessage = exception.what();
        clearStep();
    }

    return errorMessage->isEmpty();
}

bool PipelineInstance::tryEndStep(bool validate, QString* errorMessage)
{
    try
    {
        endStep(validate);
    }
    catch (std::exception& exception)
    {
        *errorMessage = exception.what();
    }

    clearStep();

    return errorMessage->isEmpty();
}

QString PipelineInstance::getSeriesDescriptionForStepData(int step, QString identifier)
{
    for (int i = step - 1; i >= -1; i--)
    {
        if (getStateAtEndOfStep(i).getWrittenMedData().contains(identifier))
        {
            return PipelineWorkingState::seriesDescriptionForStepData(i, identifier);
        }
    }
    return nullptr;
}

void PipelineInstance::enableValidation()
{
    d->validationEnabled = true;
    emit validationEnabledChanged();
}

void PipelineInstance::disableValidation()
{
    d->validationEnabled = false;
    emit validationEnabledChanged();
}

void PipelineInstance::createDelegateIfNeeded()
{
    QString delegateClass = getCurrentStepProperties().getDelegateClass();

    if (!delegateClass.isEmpty())
    {
        d->pipelineDelegate = QSharedPointer<PipelineDelegate>(PipelineFactory::instance()->createDelegate(delegateClass));
    }
    else
    {
        d->pipelineDelegate = d->pipeline;
    }
}

void PipelineInstance::destroyDelegateIfExists()
{
    if ((d->pipelineDelegate != nullptr) && (d->pipelineDelegate != d->pipeline))
    {
        d->pipelineDelegate.clear();
    }
}

QSharedPointer<PipelineDelegate> PipelineInstance::getPipelineDelegate()
{
    return d->pipelineDelegate;
}

void PipelineInstance::setPipelineDelegate(QSharedPointer<PipelineDelegate> delegate)
{
    d->pipelineDelegate = delegate;
}

void PipelineInstance::initialize()
{
    d->step = -1;
}

void PipelineInstance::writeMetaData(QXmlStreamWriter& out)
{
    out.writeStartElement("metaData");

    foreach (QString metaData, metaDataList())
    {
        QStringList values = metaDataValues(metaData);

        switch (values.length())
        {
        case 0:
            break;
        case 1:
            out.writeStartElement(metaData);
            out.writeAttribute("type", "QString");
            out.writeCharacters(values.first());
            out.writeEndElement();
            break;
        default:
            out.writeStartElement(metaData);
            out.writeAttribute("type", "QList<QString>");
            med::pipeline::writeQStringListContent(out, values);
            out.writeEndElement();
        }
    }

    out.writeEndElement();
}

void PipelineInstance::readAndUpdateMetaData(QXmlStreamReader& in)
{
    while (in.readNextStartElement())
    {
        QString key = in.name().toString();
        QString type = in.attributes().value("type").toString();

        if (type == "QString")
        {
            setMetaData(key, in.readElementText());
        }
        else if (type == "QList<QString>")
        {
            setMetaData(key, med::pipeline::readQStringListContent(in));
        }
        else
        {
            QString message;
            message += "Unexpected metaData type " + type;
            throw std::runtime_error(message.toStdString().c_str());
        }
    }
}

void PipelineInstance::writeSteps(QXmlStreamWriter& out)
{
    out.writeStartElement("steps");
    out << getInitialState();

    for (int i = 0; i < getNumSteps(); i++)
    {
        out << getStepProperties(i);
        out << getStateAtEndOfStep(i);
    }

    out.writeEndElement();
}

void PipelineInstance::readAndUpdateSteps(QXmlStreamReader& in)
{
    while (in.readNextStartElement())
    {
        if (in.name() == "stepProperties")
        {
            PipelineStepProperties stepProperties;
            in >> stepProperties;
            d->stepProperties.append(stepProperties);
        }
        else if (in.name() == "state")
        {
            PipelineState state;
            in >> state;
            d->states.append(state);
        }
        else
        {
            throwXMLException(in);
        }
    }
}

PipelineState& PipelineInstance::getInitialState()
{
    return d->states[0];
}

PipelineState& PipelineInstance::getStateAtEndOfStep(int step)
{
    return d->states[step + 1];
}

void PipelineInstance::initializeMetaData(dtkSmartPointer<medAbstractData> referenceData)
{
    QHash<QString, QString> sharedMetaData;

    foreach (QString key, getSharedMetaDataKeys())
    {
        QString value;

        if (key == medMetaDataKeys::StudyDescription.key())
        {
            value = getAbbreviation() + ": " + QDate::currentDate().toString("yy/MM/dd")
                    + "-" + QTime::currentTime().toString("hh:mm:ss");
        }
        else
        {
            value = referenceData->metadata(key);
        }

        sharedMetaData[key] = value;
        setMetaData(key, value);
    }

    setStep(-1);
    QVariant qVariant = QVariant::fromValue<QHash<QString, QString> >(sharedMetaData);
    d->workingState->setValue(sharedMetaDataId, qVariant);
    updateFromWorkingState();

    setMetaData(medMetaDataKeys::SeriesDescription.key(), "#MAIN");
    medUtilities::generateSeriesAndSOPInstanceId(this);
}

QStringList PipelineInstance::getSharedMetaDataKeys()
{
    QStringList keys;

    keys << medMetaDataKeys::PatientID.key()
         << medMetaDataKeys::PatientName.key()
         << medMetaDataKeys::Age.key()
         << medMetaDataKeys::BirthDate.key()
         << medMetaDataKeys::Gender.key()
         << medMetaDataKeys::Description.key()
         << medMetaDataKeys::StudyID.key()
         << medMetaDataKeys::StudyInstanceUID.key()
         << medMetaDataKeys::StudyDescription.key();

    return keys;
}

void PipelineInstance::updateFromWorkingState()
{
    d->workingState->importNewMedData();
    d->workingState->applyTo(getStateAtEndOfStep(getStep()));
    updateSubsequentSteps(getStep() + 1, d->workingState->getModifiedData());
    setStep(getStep());
}

void PipelineInstance::updateSubsequentSteps(int step, QList<QString> modifiedDatas)
{
    if (step < getNumSteps())
    {
        PipelineState& endState = getStateAtEndOfStep(step);
        PipelineState& startState = getStateAtEndOfStep(step - 1);
        PipelineStepProperties& stepProperties = getStepProperties(step);

        foreach (QString modifiedData, modifiedDatas)
        {
            if (stepProperties.isValid() && endState.wasViewed(modifiedData))
            {
                stepProperties.setValid(false);
            }

            if (!endState.wasWritten(modifiedData))
            {
                endState.setQVariantValue(modifiedData, startState.getQVariantValue(modifiedData));
                if (startState.hasSeriesInstanceUID(modifiedData))
                {
                    endState.setSeriesInstanceUID(modifiedData, startState.getSeriesInstanceUID(modifiedData));
                }
            }
        }

        foreach (QString writtenData, endState.getWrittenData())
        {
            if (modifiedDatas.contains(writtenData))
            {
                modifiedDatas.removeOne(writtenData);
            }
            else if (!stepProperties.isValid())
            {
                modifiedDatas.append(writtenData);
            }
        }

        updateStepExecutability(step);
        updateSubsequentSteps(step + 1, modifiedDatas);
    }
}

void PipelineInstance::updateStepExecutability(int step)
{
    PipelineState& endState = getStateAtEndOfStep(step - 1);
    PipelineStepProperties& stepProperties = getStepProperties(step);

    stepProperties.setExecutable(true);

    foreach (QStringList requiredDataGroup, stepProperties.getRequiredData())
    {
        bool found = false;

        foreach (QString requiredData, requiredDataGroup)
        {
            if (endState.hasValue(requiredData))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            stepProperties.setExecutable(false);
            break;
        }
    }
}

void PipelineInstance::checkIfViewedDataWasDeclaredRequired()
{
    QList<QStringList> requiredData = getCurrentStepProperties().getRequiredData();

    foreach (QString identifier, d->workingState->getViewedData())
    {
        bool found = false;

        foreach (QStringList requirementGroup, requiredData)
        {
            if (requirementGroup.contains(identifier))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            QString message = QString(identifier);
            message += " was viewed by step " + getCurrentStepProperties().getTitle()
                    + " but not declared as required!";
            throw std::runtime_error(message.toStdString().c_str());
        }
    }
}

void PipelineInstance::startStep()
{
    createDelegateIfNeeded();

    PipelineWorkspace::instance()->setToolBoxVisibility(MOUSE_INTERACTION_TOOLBOX, true);
    PipelineWorkspace::instance()->setToolBoxVisibility(VIEW_SETTINGS_TOOLBOX, true);
    PipelineWorkspace::instance()->setToolBoxVisibility(LAYERS_TOOLBOX, true);
    enableValidation();

    d->pipelineDelegate->startStep(getCurrentStepProperties().getName(), d->workingState.data());

    if (d->workingState->wasRejected())
    {
        throw std::runtime_error(d->workingState->rejectionMessage().toStdString().c_str());
    }
    else
    {
        PipelineWorkspace::instance()->setToolBoxVisibility(PIPELINE_CONTROL_TOOLBOX, true);
        PipelineWorkspace::instance()->setToolBoxVisibility(PIPELINE_CHOOSER_TOOLBOX, false);
        setUnnamedTabsToStepName();
    }
}

void PipelineInstance::endStep(bool validate)
{
    checkIfViewedDataWasDeclaredRequired();

    if (validate)
    {
        d->pipelineDelegate->endStep(getCurrentStepProperties().getName(), d->workingState.data());

        if (d->workingState->wasRejected())
        {
            throw std::runtime_error(d->workingState->rejectionMessage().toStdString().c_str());
        }
        else
        {
            getCurrentStepProperties().setExecuted(true);
            getCurrentStepProperties().setValid(true);
            updateFromWorkingState();
            PipelineWorkspace::instance()->secureImport(this);
        }
    }
}

void PipelineInstance::clearStep()
{
    PipelineWorkspace::instance()->clear();
    d->workingState.clear();
    destroyDelegateIfExists();
}

void PipelineInstance::updateDataIndices()
{
    QList<QString> missingData = updateDataIndicesUsingSeriesInstanceUID();

    if (!missingData.isEmpty())
    {
        missingData = updateDataIndicesUsingSeriesDescription();

        if (!missingData.isEmpty())
        {
            QString message;
            message += "Cannot find the following data: " + missingData.takeFirst();

            foreach (QString item, missingData)
            {
                message += ", " + item;
            }

            throw std::runtime_error(message.toStdString().c_str());
        }
    }

    PipelineWorkspace::instance()->secureImport(this);
}

/* To find which data corresponds to an identifier in a step state, this function searches for the data's dicom
 * id. This dicom id is stored in the state and allows us to find the data even if its data index or its series
 * description has changed.
 */
QList<QString> PipelineInstance::updateDataIndicesUsingSeriesInstanceUID()
{
    QHash<QString, medDataIndex> dataIndices;
    QList<QString> missingData;

    foreach (medDataIndex dataIndex, medDataManager::instance()->getSeriesListFromStudy(this->dataIndex()))
    {
        QString seriesInstanceUID = medDataManager::instance()->getMetaData(dataIndex, medMetaDataKeys::SeriesInstanceUID.key());
        dataIndices[seriesInstanceUID] = dataIndex;
    }

    for (int i = -1; i < getNumSteps(); i++)
    {
        PipelineState& state = getStateAtEndOfStep(i);

        foreach (QString identifier, state.getAllMedData())
        {
            QString expectedSeriesInstanceUID = state.getSeriesInstanceUID(identifier);

            if (!expectedSeriesInstanceUID.isEmpty() && dataIndices.contains(expectedSeriesInstanceUID))
            {
                QVariant value = QVariant::fromValue<medDataIndex>(dataIndices[expectedSeriesInstanceUID]);
                state.setQVariantValue(identifier, value);
            }
            else
            {
                if (state.wasWritten(identifier))
                {
                    QString expectedSeriesDescription = PipelineWorkingState::seriesDescriptionForStepData(i, identifier);
                    missingData.append(expectedSeriesDescription);
                }
            }
        }
    }

    return missingData;
}

/* To find which data corresponds to an identifier in a step state, this function searches for the series
 * description that the data should have.
 */
QList<QString> PipelineInstance::updateDataIndicesUsingSeriesDescription()
{
    QHash<QString, medDataIndex> seriesIdToDataIndices;
    QHash<QString, medDataIndex> stateIdToDataIndices;
    QList<QString> missingData;

    foreach (medDataIndex dataIndex, medDataManager::instance()->getSeriesListFromStudy(this->dataIndex()))
    {
        QString seriesDescription = medDataManager::instance()->getMetaData(dataIndex, medMetaDataKeys::SeriesDescription.key());
        seriesIdToDataIndices[seriesDescription] = dataIndex;
    }

    for (int i = -1; i < getNumSteps(); i++)
    {
        PipelineState& state = getStateAtEndOfStep(i);

        foreach (QString identifier, state.getAllMedData())
        {
            medDataIndex dataIndex;

            if (state.wasWritten(identifier))
            {
                QString expectedSeriesDescription = PipelineWorkingState::seriesDescriptionForStepData(i, identifier);

                if (seriesIdToDataIndices.contains(expectedSeriesDescription))
                {
                    dataIndex = seriesIdToDataIndices[expectedSeriesDescription];
                    stateIdToDataIndices[identifier] = dataIndex;
                }
                else
                {
                    missingData.append(expectedSeriesDescription);
                }
            }
            else
            {
                dataIndex = stateIdToDataIndices[identifier];
            }

            if (dataIndex.isValidForSeries())
            {
                QVariant value = QVariant::fromValue<medDataIndex>(dataIndex);
                state.setQVariantValue(identifier, value);
                state.setSeriesInstanceUID(identifier, medDataManager::instance()->getMetaData(dataIndex, medMetaDataKeys::SeriesInstanceUID.key()));
            }
        }
    }

    return missingData;
}

void PipelineInstance::setUnnamedTabsToStepName()
{
    medTabbedViewContainers* tabs = PipelineWorkspace::instance()->tabbedViewContainers();
    int postFix = 1;

    for (int i = 0; i < tabs->count(); i++)
    {
        if (tabs->tabText(i) == PipelineWorkspace::staticName())
        {
            tabs->setTabText(i, getCurrentStepProperties().getTitle() + " " + QString::number(postFix));
            postFix++;
        }
    }
}

} // namespace med::pipeline
