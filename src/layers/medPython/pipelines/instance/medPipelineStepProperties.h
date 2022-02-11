#pragma once

#include <QHash>
#include <QStringList>
#include <QVariant>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <medPythonCore.h>

#include "medPipelineConstants.h"
#include "medPipelineExport.h"

namespace med::pipeline
{

class MEDPIPELINE_EXPORT PipelineStepProperties;

}

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, const med::pipeline::PipelineStepProperties& properties);
QXmlStreamReader& operator>>(QXmlStreamReader& in, med::pipeline::PipelineStepProperties& properties);

namespace med::pipeline
{

class PipelineStepPropertiesPrivate;

class MEDPIPELINE_EXPORT PipelineStepProperties
{
public:
    PipelineStepProperties();
    PipelineStepProperties(const PipelineStepProperties& other);
    PipelineStepProperties(QString name);
    virtual ~PipelineStepProperties();

    PipelineStepProperties setDelegateClass(QString delegateClass);
    QString getDelegateClass() const;

    PipelineStepProperties setName(QString name);
    QString getName() const;

    PipelineStepProperties setTitle(QString title);
    PipelineStepProperties appendTitle(QString postfix);
    PipelineStepProperties prependTitle(QString prefix);
    QString getTitle() const;

    PipelineStepProperties setDescription(QString description);
    PipelineStepProperties appendDescription(QString postfix);
    PipelineStepProperties prependDescription(QString prefix);
    QString getDescription() const;

    PipelineStepProperties requires(QString dataIdentifier);
    PipelineStepProperties requiresOneOf(QStringList dataIdentifiers);
    QList<QStringList> getRequiredData() const;

    PipelineStepProperties setResultDropMode(med::pipeline::ResultDropMode dropMode);
    med::pipeline::ResultDropMode getResultDropMode();

    PipelineStepProperties setOption(QString name, QVariant value);
    template<typename TYPE> PipelineStepProperties setOption(QString name, TYPE value);
    QHash<QString, QVariant> getOptions() const;

    void setExecutable(bool value);
    bool isExecutable() const;

    void setExecuted(bool value);
    bool wasExecuted() const;

    void setValid(bool value);
    bool isValid() const;

    PipelineStepProperties setAutomatic(bool value);
    bool isAutomatic() const;
    PipelineStepProperties& operator=(const PipelineStepProperties& other);

private:
    PipelineStepPropertiesPrivate* const d;

    void initialize();

    friend QXmlStreamWriter& ::operator<<(QXmlStreamWriter&, const PipelineStepProperties&);
    friend QXmlStreamReader& ::operator>>(QXmlStreamReader&, PipelineStepProperties&);
};

template<typename TYPE>
PipelineStepProperties PipelineStepProperties::setOption(QString name, TYPE value)
{
    return setOption(name, QVariant::fromValue<TYPE>(value));
}

} // namespace med::pipeline

MEDPIPELINE_EXPORT bool medPythonConvert(const med::pipeline::PipelineStepProperties& input, PyObject** output);
MEDPIPELINE_EXPORT bool medPythonConvert(const PyObject* input, med::pipeline::PipelineStepProperties* output);
