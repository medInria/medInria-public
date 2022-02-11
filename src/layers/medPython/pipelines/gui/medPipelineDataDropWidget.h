#pragma once

#include <QWidget>

#include <medAbstractData.h>

#include "medPipelineInstance.h"
#include "medPipelineExport.h"

namespace med::pipeline
{

class DataDropWidgetPrivate;

class MEDPIPELINE_EXPORT DataDropWidget : public QWidget
{
    Q_OBJECT

signals:
    void received(QString identifier, dtkSmartPointer<medAbstractData> value, QHash<QString, QVariant> windowLevel);
    void finished();
    void cancelled();

public:
    DataDropWidget(QWidget* parent = 0);
    virtual ~DataDropWidget();

    void requestInitialDataDrop(dtkSmartPointer<PipelineInstance> pipeline);
    void requestDataDrop(QString identifier, QString name, bool withWindowLevel = false);

private slots:
    void requestInitialDataDrop(QString identifier);
    void acceptDrop();
    void cancelDrop();
    void actOnContainerChange();
    void windowLevelChanged(const QHash<QString,QVariant>& windowLevel);

private:
    DataDropWidgetPrivate* const d;

    void endDrop();
    void clearWidget();
    void switchToWindowingMode();
    void connectWindowLevelIfPossible();
};

} // namespace med::pipeline
