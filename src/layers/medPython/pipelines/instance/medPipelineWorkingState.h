#pragma once

#include <dtkCoreSupport/dtkSmartPointer>

#include <medAbstractData.h>

#include "medPipelineState.h"
#include "medPipelineTypes.h"

namespace med::pipeline
{

class PipelineAttachedData;
class PipelineWorkingStatePrivate;

class MEDPIPELINE_EXPORT PipelineWorkingState : public PipelineState
{
    Q_OBJECT

signals:
    void importsFinished();

public:
    static QString seriesDescriptionForStepData(int step, QString identifier);

    PipelineWorkingState();
    PipelineWorkingState(const PipelineWorkingState& other);
    PipelineWorkingState(const PipelineState& referenceState, int step, QHash<QString, QVariant> stepOptions);
    virtual ~PipelineWorkingState();

    void importNewMedData();

    void applyTo(PipelineState& state) const;
    QList<QString> getModifiedData();

    void setValue(QString identifier, medAbstractData* value);
    void setValue(QString identifier, dtkSmartPointer<medAbstractData> value);
    dtkSmartPointer<medAbstractData> getValue(QString identifier);
    dtkSmartPointer<medAbstractData> peekValue(QString identifier);

    template<typename TYPE> void setValue(QString identifier, TYPE value);
    template<typename TYPE> TYPE getValue(QString identifier);
    template<typename TYPE> TYPE peekValue(QString identifier);

    bool hasStepOption(QString name);
    QVariant getStepOption(QString name);
    template<typename TYPE> TYPE getStepOption(QString name);

    void reject(QString message);
    bool wasRejected();
    QString rejectionMessage();

    void setWindowLevel(QString identifier, double window, double level);
    void setWindowLevel(medAbstractData* value, double window, double level);

    void setWindowLevel(QString identifier, QHash<QString, QVariant> windowLevel);
    void setWindowLevel(medAbstractData* value, QHash<QString, QVariant> windowLevel);

    void copyWindowLevel(QString targetId, QString sourceId);
    void copyWindowLevel(medAbstractData* targetValue, medAbstractData* sourceValue);

    void setRenderingParameters(QString identifier, med::pipeline::RenderingParameters renderingParameters);
    void setRenderingParameters(medAbstractData* value, med::pipeline::RenderingParameters renderingParameters);

    void setFinalResult(QString identifier, bool isFinalResult = true);
    void setFinalResult(medAbstractData* data, bool isFinalResult = true);

    template<typename TYPE> void setResultValue(QString name, TYPE value);
    template<typename TYPE> TYPE getResultValue(QString name);

    QStringList findMedDataIdentifiers(medAbstractData* value);

private:
    using PipelineState::setQVariantValue;
    using PipelineState::getQVariantValue;
    using PipelineState::findMedDataIdentifiers;

    PipelineWorkingStatePrivate* const d;

    void addToMedDataCache(QString identifier, dtkSmartPointer<medAbstractData> value);
    void removeFromMedDataCache(QString identifier);
    dtkSmartPointer<medAbstractData> retrieveMedData(QString identifier);

    void setMetaDataForDataToImport(QString identifier, dtkSmartPointer<medAbstractData> data);
    void waitForImportsFinished();

private slots:
    void dataImported(const medDataIndex& index, QUuid importId);
};

template<typename TYPE>
void PipelineWorkingState::setValue(QString identifier, TYPE value)
{
    markAsWritten(identifier);
    removeFromMedDataCache(identifier);
    setQVariantValue(identifier, QVariant::fromValue<TYPE>(value));
}

template<typename TYPE>
TYPE PipelineWorkingState::getValue(QString identifier)
{
    markAsViewed(identifier);
    return getQVariantValue(identifier).value<TYPE>();
}

template<typename TYPE>
TYPE PipelineWorkingState::peekValue(QString identifier)
{
    return getQVariantValue(identifier).value<TYPE>();
}

template<typename TYPE>
TYPE PipelineWorkingState::getStepOption(QString name)
{
    return getStepOption(name).value<TYPE>();
}

template<typename TYPE>
void PipelineWorkingState::setResultValue(QString name, TYPE value)
{
    QHash<QString, QVariant> resultValues = peekValue<QHash<QString, QVariant> >(med::pipeline::resultValuesId);
    resultValues[name] = QVariant::fromValue<TYPE>(value);
    setValue(med::pipeline::resultValuesId, resultValues);
}

template<typename TYPE>
TYPE PipelineWorkingState::getResultValue(QString name)
{
    QHash<QString, QVariant> resultValues = getValue<QHash<QString, QVariant> >(med::pipeline::resultValuesId);
    return resultValues[name].value<TYPE>();
}

} // namespace med::pipeline

Q_DECLARE_METATYPE(med::pipeline::PipelineWorkingState)
