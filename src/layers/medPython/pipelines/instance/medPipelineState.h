#pragma once

#include <QObject>
#include <QVariant>
#include <qxmlstream.h>

#include "medPipelineExport.h"

namespace med::pipeline
{

class MEDPIPELINE_EXPORT PipelineState;

}

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, const med::pipeline::PipelineState& state);
QXmlStreamReader& operator>>(QXmlStreamReader& in, med::pipeline::PipelineState& state);

namespace med::pipeline
{

class PipelineStatePrivate;

class MEDPIPELINE_EXPORT PipelineState : public QObject
{
    Q_OBJECT

public:
    PipelineState();
    PipelineState(const PipelineState& other);
    virtual ~PipelineState();

    PipelineState& operator=(const PipelineState& other);

    void copyValues(const PipelineState& other);
    void copyMarkers(const PipelineState& other);

    void setQVariantValue(QString identifier, QVariant value);
    QVariant getQVariantValue(QString identifier) const;
    bool hasValue(QString identifier);
    bool isMedData(QString identifier) const;
    QStringList findMedDataIdentifiers(QVariant dataIndex);

    void setSeriesInstanceUID(QString identifier, QString seriesInstanceUID);
    QString getSeriesInstanceUID(QString identifier) const;
    bool hasSeriesInstanceUID(QString identifier) const;

    void markAsViewed(QString identifier);
    void markAsWritten(QString identifier);
    bool wasViewed(QString identifier) const;
    bool wasWritten(QString identifier) const;
    QList<QString> getViewedData() const;
    QList<QString> getWrittenData() const;
    QList<QString> getWrittenMedData();
    QList<QString> getAllMedData();

private:
    PipelineStatePrivate* const d;

    friend QXmlStreamWriter& ::operator<<(QXmlStreamWriter&, const PipelineState&);
    friend QXmlStreamReader& ::operator>>(QXmlStreamReader&, PipelineState&);
};

} // namespace med::pipeline

Q_DECLARE_METATYPE(med::pipeline::PipelineState)
