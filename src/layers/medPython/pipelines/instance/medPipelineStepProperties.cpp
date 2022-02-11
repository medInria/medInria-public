#include "medPipelineStepProperties.h"

#include <medPythonConversionUtils.h>

#include "medPipelineTypes.h"

namespace med::pipeline
{

class PipelineStepPropertiesPrivate
{
public:
    QString delegateClass;
    QString name, title, description;
    QList<QStringList> required;
    bool isExecutable, wasExecuted, isValid, isAutomatic;
    ResultDropMode resultDropMode;
    QHash<QString, QVariant> options;
};

PipelineStepProperties::PipelineStepProperties()
    : d(new PipelineStepPropertiesPrivate())
{
    initialize();
}

PipelineStepProperties::PipelineStepProperties(const PipelineStepProperties& other)
    : d(new PipelineStepPropertiesPrivate())
{
    *d = *other.d;
}

PipelineStepProperties::PipelineStepProperties(QString name)
    : d(new PipelineStepPropertiesPrivate())
{
    initialize();
    setName(name);
}

PipelineStepProperties::~PipelineStepProperties()
{
    delete d;
}

PipelineStepProperties PipelineStepProperties::setDelegateClass(QString delegateClass)
{
    d->delegateClass = delegateClass;
    return *this;
}

QString PipelineStepProperties::getDelegateClass() const
{
    return d->delegateClass;
}

PipelineStepProperties PipelineStepProperties::setName(QString name)
{
    d->name = name;

    if (d->title.isEmpty())
    {
        d->title = name;
    }

    return *this;
}

QString PipelineStepProperties::getName() const
{
    return d->name;
}

PipelineStepProperties PipelineStepProperties::setTitle(QString title)
{
    d->title = title;
    return *this;
}

PipelineStepProperties PipelineStepProperties::appendTitle(QString postfix)
{
    d->title += postfix;
    return *this;
}

PipelineStepProperties PipelineStepProperties::prependTitle(QString prefix)
{
    d->title = prefix + d->title;
    return *this;
}

QString PipelineStepProperties::getTitle() const
{
    return d->title;
}

PipelineStepProperties PipelineStepProperties::setDescription(QString description)
{
    d->description = description;
    return *this;
}

PipelineStepProperties PipelineStepProperties::appendDescription(QString postfix)
{
    d->description += postfix;
    return *this;
}

PipelineStepProperties PipelineStepProperties::prependDescription(QString prefix)
{
    d->description = prefix + d->description;
    return *this;
}

QString PipelineStepProperties::getDescription() const
{
    return d->description;
}

PipelineStepProperties PipelineStepProperties::requires(QString dataIdentifier)
{
    d->required.append(QStringList(dataIdentifier));
    return *this;
}

PipelineStepProperties PipelineStepProperties::requiresOneOf(QStringList dataIdentifiers)
{
    d->required.append(dataIdentifiers);
    return *this;
}

QList<QStringList> PipelineStepProperties::getRequiredData() const
{
    return d->required;
}

PipelineStepProperties PipelineStepProperties::setResultDropMode(ResultDropMode dropMode)
{
    d->resultDropMode = dropMode;
    return *this;
}

ResultDropMode PipelineStepProperties::getResultDropMode()
{
    return d->resultDropMode;
}

PipelineStepProperties PipelineStepProperties::setOption(QString name, QVariant value)
{
    d->options[name] = value;
    return *this;
}

QHash<QString, QVariant> PipelineStepProperties::getOptions() const
{
    return d->options;
}

void PipelineStepProperties::setExecutable(bool value)
{
    d->isExecutable = value;
}

bool PipelineStepProperties::isExecutable() const
{
    return d->isExecutable;
}

void PipelineStepProperties::setExecuted(bool value)
{
    d->wasExecuted = value;
}

bool PipelineStepProperties::wasExecuted() const
{
    return d->wasExecuted;
}

void PipelineStepProperties::setValid(bool value)
{
    d->isValid = value;
}

bool PipelineStepProperties::isValid() const
{
    return d->isValid;
}

PipelineStepProperties PipelineStepProperties::setAutomatic(bool value)
{
    d->isAutomatic = value;
    return *this;
}

bool PipelineStepProperties::isAutomatic() const
{
    return d->isAutomatic;
}

PipelineStepProperties& PipelineStepProperties::operator=(const PipelineStepProperties& other)
{
    *d = *other.d;
    return *this;
}

void PipelineStepProperties::initialize()
{
    d->isExecutable = false;
    d->wasExecuted = false;
    d->isValid = true;
    d->resultDropMode = NO_RESULT_DROP;
    d->isAutomatic = false;
}

} // namespace med::pipeline

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, const med::pipeline::PipelineStepProperties& properties)
{
    out.writeStartElement("stepProperties");

    out.writeAttribute("name", properties.d->name);
    out.writeAttribute("isExecutable", QString::number(properties.isExecutable()));
    out.writeAttribute("isValid", QString::number(properties.isValid()));
    out.writeAttribute("wasExecuted", QString::number(properties.wasExecuted()));
    out.writeAttribute("isAutomatic", QString::number(properties.isAutomatic()));

    if (!properties.d->delegateClass.isEmpty())
    {
        out.writeTextElement("delegateClass", properties.d->delegateClass);
    }

    if (!properties.d->title.isEmpty())
    {
        out.writeTextElement("title", properties.d->title);
    }

    if (!properties.d->description.isEmpty())
    {
        out.writeTextElement("description", properties.d->description);
    }

    foreach (QStringList requiredGroup, properties.d->required)
    {
        if (requiredGroup.length() == 1)
        {
            out.writeTextElement("requires", requiredGroup.first());
        }
        else
        {
            out.writeTextElement("requiresOneOf", requiredGroup.join(" "));
        }
    }

    out.writeTextElement("resultDropMode", QString::number(properties.d->resultDropMode));

    foreach (QString key, properties.d->options.keys())
    {
        QVariant value = properties.d->options[key];

        out.writeStartElement("option");
        out.writeAttribute("name", key);
        out.writeAttribute("type", value.typeName());
        med::pipeline::writeQVariantContent(out, value);
        out.writeEndElement();
    }

    out.writeEndElement();

    return out;
}

QXmlStreamReader& operator>>(QXmlStreamReader& in, med::pipeline::PipelineStepProperties& properties)
{
    QXmlStreamAttributes attributes = in.attributes();
    properties.d->name = attributes.value("name").toString();
    properties.d->isExecutable = attributes.value("isExecutable").toString().toInt();
    properties.d->isValid = attributes.value("isValid").toString().toInt();
    properties.d->wasExecuted = attributes.value("wasExecuted").toString().toInt();
    if ( attributes.hasAttribute("isAutomatic") )
    {
        properties.d->isAutomatic = attributes.value("isAutomatic").toString().toInt();
    }

    while (in.readNextStartElement())
    {
        if (in.name() == "delegateClass")
        {
            properties.d->delegateClass = in.readElementText();
        }
        else if (in.name() == "title")
        {
            properties.d->title = in.readElementText();
        }
        else if (in.name() == "description")
        {
            properties.d->description = in.readElementText();
        }
        else if (in.name() == "requires")
        {
            properties.d->required.append(QStringList(in.readElementText()));
        }
        else if (in.name() == "requiresOneOf")
        {
            properties.d->required.append(in.readElementText().split(" "));
        }
        else if (in.name() == "resultDropMode")
        {
            properties.d->resultDropMode = static_cast<med::pipeline::ResultDropMode>(in.readElementText().toInt());
        }
        else if (in.name() == "option")
        {
            QString name = in.attributes().value("name").toString();
            QString type = in.attributes().value("type").toString();
            properties.d->options[name] = med::pipeline::readQVariantContent(in, type);
        }
        else
        {
            med::pipeline::throwXMLException(in);
        }
    }

    return in;
}

MED_PYTHON_STANDARD_VALUE_CONVERSIONS(med::pipeline::PipelineStepProperties);
