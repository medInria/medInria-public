#include "medPipelineState.h"

#include <QHash>

#include <medDataIndex.h>

#include "medPipelineTypes.h"

namespace med::pipeline
{

class PipelineStatePrivate
{
public:
    QHash<QString, QVariant> values;
    QHash<QString, bool> viewedData;
    QHash<QString, bool> writtenData;
    QHash<QString, QString> seriesInstanceUID;
};

PipelineState::PipelineState()
    : d(new PipelineStatePrivate())
{
}

PipelineState::PipelineState(const PipelineState& other)
    : d(new PipelineStatePrivate())
{
    d->values = other.d->values;
    d->viewedData = other.d->viewedData;
    d->writtenData = other.d->writtenData;
    d->seriesInstanceUID = other.d->seriesInstanceUID;
}

PipelineState::~PipelineState()
{
    delete d;
}

PipelineState& PipelineState::operator=(const PipelineState& other)
{
    d->viewedData = other.d->viewedData;
    d->writtenData = other.d->writtenData;
    d->values = other.d->values;
    d->seriesInstanceUID = other.d->seriesInstanceUID;
    return *this;
}

void PipelineState::copyValues(const PipelineState& other)
{
    d->values = other.d->values;
    d->seriesInstanceUID = other.d->seriesInstanceUID;
}

void PipelineState::copyMarkers(const PipelineState& other)
{
    d->viewedData = other.d->viewedData;
    d->writtenData = other.d->writtenData;
}

void PipelineState::setQVariantValue(QString identifier, QVariant value)
{
    d->values[identifier] = value;
}

QVariant PipelineState::getQVariantValue(QString identifier) const
{
    return d->values.value(identifier);
}

bool PipelineState::hasValue(QString identifier)
{
    return d->values.contains(identifier);
}

bool PipelineState::isMedData(QString identifier) const
{
    return d->values.value(identifier).canConvert<medDataIndex>();
}

QStringList PipelineState::findMedDataIdentifiers(QVariant dataIndex)
{
    QStringList result;
    QHash<QString, QVariant>::const_iterator iterator;

    for (iterator = d->values.constBegin(); iterator != d->values.constEnd(); iterator++)
    {
        if (iterator.value() == dataIndex)
        {
            result.append(iterator.key());
        }
    }

    return result;
}

void PipelineState::setSeriesInstanceUID(QString identifier, QString seriesInstanceUID)
{
    d->seriesInstanceUID[identifier] = seriesInstanceUID;
}

QString PipelineState::getSeriesInstanceUID(QString identifier) const
{
    return d->seriesInstanceUID.value(identifier);
}

bool PipelineState::hasSeriesInstanceUID(QString identifier) const
{
    return d->seriesInstanceUID.contains(identifier);
}

void PipelineState::markAsViewed(QString identifier)
{
    d->viewedData[identifier] = true;
}

void PipelineState::markAsWritten(QString identifier)
{
    d->writtenData[identifier] = true;
}

bool PipelineState::wasViewed(QString identifier) const
{
    return d->viewedData.value(identifier, false);
}

bool PipelineState::wasWritten(QString identifier) const
{
    return d->writtenData.value(identifier, false);
}

QList<QString> PipelineState::getViewedData() const
{
    return d->viewedData.keys();
}

QList<QString> PipelineState::getWrittenData() const
{
    return d->writtenData.keys();
}

QList<QString> PipelineState::getWrittenMedData()
{
    QList<QString> result;

    foreach (QString identifier, getAllMedData())
    {
        if (d->writtenData.contains(identifier))
        {
            result.append(identifier);
        }
    }

    return result;
}

QList<QString> PipelineState::getAllMedData()
{
    QList<QString> result;

    foreach (QString identifier, d->values.keys())
    {
        if (isMedData(identifier))
        {
            result.append(identifier);
        }
    }

    return result;
}

} // namespace med::pipeline

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, const med::pipeline::PipelineState& state)
{
    out.writeStartElement("state");

    foreach (QString identifier, state.d->values.keys())
    {
        QVariant value = state.d->values[identifier];

        out.writeStartElement("value");
        out.writeAttribute("identifier", identifier);
        out.writeAttribute("type", value.typeName());
        out.writeAttribute("viewed", QString::number(state.wasViewed(identifier)));
        out.writeAttribute("written", QString::number(state.wasWritten(identifier)));

        if (state.hasSeriesInstanceUID(identifier))
        {
            out.writeAttribute("seriesInstanceUID", state.getSeriesInstanceUID(identifier));
        }

        med::pipeline::writeQVariantContent(out, value);

        out.writeEndElement();
    }

    out.writeEndElement();

    return out;
}

QXmlStreamReader& operator>>(QXmlStreamReader& in, med::pipeline::PipelineState& state)
{
    while (in.readNextStartElement())
    {
        if (in.name() == "value")
        {
            QXmlStreamAttributes attributes = in.attributes();
            QString identifier = attributes.value("identifier").toString();

            state.d->viewedData[identifier] = attributes.value("viewed").toString().toInt();
            state.d->writtenData[identifier] = attributes.value("written").toString().toInt();

            if (attributes.hasAttribute("seriesInstanceUID"))
            {
                state.setSeriesInstanceUID(identifier, attributes.value("seriesInstanceUID").toString());
            }
            else if (attributes.hasAttribute("seriesDicomId"))
            {
                state.setSeriesInstanceUID(identifier, attributes.value("seriesDicomId").toString());
            }

             QString type = in.attributes().value("type").toString();
             state.d->values[identifier] = med::pipeline::readQVariantContent(in, type);
        }
        else
        {
            med::pipeline::throwXMLException(in);
        }
    }

    return in;
}
