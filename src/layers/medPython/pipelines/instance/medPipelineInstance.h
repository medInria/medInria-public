#pragma once

#include <dtkCoreSupport/dtkSmartPointer>

#include <medAbstractData.h>

#include "medAbstractPipeline.h"
#include "medPipelineState.h"
#include "medPipelineStepProperties.h"
#include "medPipelineExport.h"

namespace med::pipeline
{

class PipelineInstancePrivate;

class MEDPIPELINE_EXPORT PipelineInstance : public medAbstractData
{
    Q_OBJECT
    MED_DATA_INTERFACE("Pipeline instance", "Pipeline instance")

    signals:
        void validationEnabledChanged();

public:
    static bool registered();

    PipelineInstance();
    PipelineInstance(AbstractPipeline* pipeline);
    PipelineInstance(const PipelineInstance& other);
    virtual ~PipelineInstance();

    virtual bool read(QXmlStreamReader& in, bool readOnlyMetaData = false);
    using dtkAbstractData::read;

    virtual void write(QXmlStreamWriter& out);
    using dtkAbstractData::write;

    virtual void setup();

    QSharedPointer<AbstractPipeline> getPipeline();

    QString getTitle();
    QString getAbbreviation();

    int getNumSteps() const;
    void setStep(int step);
    int getStep();

    bool isProperStep();
    bool isFirstStep();
    bool isLastStep();

    int getNextExecutableStep();
    int getPreviousExecutableStep();
    int getFirstExecutedStep();

    PipelineStepProperties& getStepProperties(int step);
    PipelineStepProperties& getCurrentStepProperties();

    QStringList getInitialDataIdentifiers();
    QString getInitialDataName(QString identifier);
    void setInitialDataValue(QString identifier, dtkSmartPointer<medAbstractData> value, QHash<QString, QVariant> windowLevel);
    bool initialDataWasSet(QString identifier);

    bool isValidationEnabled();

    bool tryStartStep(QString* errorMessage);
    bool tryEndStep(bool validate, QString* errorMessage);

    QString getSeriesDescriptionForStepData(int step, QString identifier);

public slots:
    void enableValidation();
    void disableValidation();

protected:
    virtual void createDelegateIfNeeded();
    virtual void destroyDelegateIfExists();
    QSharedPointer<PipelineDelegate> getPipelineDelegate();
    void setPipelineDelegate(QSharedPointer<PipelineDelegate> delegate);

private:
    PipelineInstancePrivate* const d;

    void initialize();

    void writeMetaData(QXmlStreamWriter& out);
    void readAndUpdateMetaData(QXmlStreamReader& in);

    void writeSteps(QXmlStreamWriter& out);
    void readAndUpdateSteps(QXmlStreamReader& in);

    PipelineState& getInitialState();
    PipelineState& getStateAtEndOfStep(int step);

    void initializeMetaData(dtkSmartPointer<medAbstractData> referenceData);
    QStringList getSharedMetaDataKeys();

    void updateFromWorkingState();
    void updateSubsequentSteps(int step, QList<QString> modifiedDatas);
    void updateStepExecutability(int step);

    void checkIfViewedDataWasDeclaredRequired();

    void startStep();
    void endStep(bool validate);
    void clearStep();

    void updateDataIndices();
    QList<QString> updateDataIndicesUsingSeriesInstanceUID();
    QList<QString> updateDataIndicesUsingSeriesDescription();

    void setUnnamedTabsToStepName();
};

} // namespace med::pipeline

Q_DECLARE_METATYPE(med::pipeline::PipelineInstance)
