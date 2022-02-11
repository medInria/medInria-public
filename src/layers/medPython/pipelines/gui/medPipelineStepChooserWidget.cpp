#include "medPipelineStepChooserWidget.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "medPipelineStepProperties.h"

namespace med::pipeline
{

class PipelineStepChooserWidgetPrivate
{
public:
    dtkSmartPointer<PipelineInstance> pipeline;
    QList<QPushButton*> buttons;
};

PipelineStepChooserWidget::PipelineStepChooserWidget(QWidget* parent)
    : QWidget(parent), d(new PipelineStepChooserWidgetPrivate())
{
    setLayout(new QVBoxLayout());
}

PipelineStepChooserWidget::~PipelineStepChooserWidget()
{
    delete d;
}

void PipelineStepChooserWidget::setPipeline(dtkSmartPointer<PipelineInstance> pipeline)
{
    clearWidget();

    d->pipeline = pipeline;

    if (d->pipeline)
    {
        QPushButton* quitButton = new QPushButton("Quit");
        layout()->addWidget(quitButton);
        connect(quitButton, SIGNAL(clicked()), this, SIGNAL(quit()));

        layout()->addWidget(new QLabel("Select a step:"));

        for (int step = 0; step < d->pipeline->getNumSteps(); step++)
        {
            QPushButton* button = new QPushButton(QString::number(step + 1) + ". " + d->pipeline->getStepProperties(step).getTitle());
            layout()->addWidget(button);
            d->buttons.append(button);
            connect(button, &QPushButton::clicked, [=] { emit stepChosen(step); });
        }

        refresh();
    }
}

void PipelineStepChooserWidget::refresh()
{
    for (int step = 0; step < d->pipeline->getNumSteps(); step++)
    {
        PipelineStepProperties& stepProperties = d->pipeline->getStepProperties(step);

        QPushButton* button = d->buttons[step];
        button->setEnabled(stepProperties.isExecutable());
        button->setStyleSheet("");

        if (stepProperties.wasExecuted())
        {
            if (stepProperties.isValid())
            {
                button->setIcon(QIcon(":icons/music-check.svg"));
                button->setIconSize(QSize(16, 16));
            }
            else
            {
                button->setIcon(QIcon(":icons/music-cross.svg"));
                button->setIconSize(QSize(16, 16));
            }
        }
    }
}

void PipelineStepChooserWidget::clearWidget()
{
    d->buttons.clear();

    foreach (QWidget* child, findChildren<QWidget*>())
    {
        delete child;
    }
}

} // namespace med::pipeline
