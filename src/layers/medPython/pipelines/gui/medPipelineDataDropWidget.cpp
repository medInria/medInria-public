#include "medPipelineDataDropWidget.h"

#include <QLabel>
#include <QPushButton>

#include <medAbstractLayeredView.h>
#include <medCompositeParameterL.h>
#include <medTabbedViewContainers.h>
#include <medViewContainer.h>
#include <medVtkViewBackend.h>

#include "medPipelineWorkspace.h"

namespace med::pipeline
{

class DataDropWidgetPrivate
{
public:
    dtkSmartPointer<PipelineInstance> pipeline;
    QString currentDrop;
    medViewContainer* viewContainer;
    QWidget* topWidget;
    QPushButton* okButton;
    bool windowLevelChanged;
    QHash<QString, QVariant> windowLevel;
    bool multiDropMode;
};

DataDropWidget::DataDropWidget(QWidget* parent)
    : QWidget(parent), d(new DataDropWidgetPrivate())
{
    setLayout(new QVBoxLayout());
    d->multiDropMode = false;
}

DataDropWidget::~DataDropWidget()
{
    delete d;
}

void DataDropWidget::requestInitialDataDrop(dtkSmartPointer<PipelineInstance> pipeline)
{
    clearWidget();

    bool atLeastOneInitialData = false;

    d->pipeline = pipeline;
    QStringList initialDataIdentifiers = d->pipeline->getInitialDataIdentifiers();
    initialDataIdentifiers.sort();

    layout()->addWidget(new QLabel("Select the data for the pipeline"));

    foreach (QString identifier, initialDataIdentifiers)
    {
        if (!d->pipeline->initialDataWasSet(identifier))
        {
            QPushButton* button = new QPushButton(d->pipeline->getInitialDataName(identifier));
            layout()->addWidget(button);
            connect(button, &QPushButton::clicked, [=] { requestInitialDataDrop(identifier); });
        }
        else
        {
            atLeastOneInitialData = true;
        }
    }

    QHBoxLayout* subLayout = new QHBoxLayout();
    static_cast<QVBoxLayout*>(layout())->addLayout(subLayout);

    QPushButton* doneButton = new QPushButton("Run");
    subLayout->addWidget(doneButton);
    connect(doneButton, SIGNAL(clicked()), this, SIGNAL(finished()));
    doneButton->setEnabled(atLeastOneInitialData);

    QPushButton* cancelButton = new QPushButton("Cancel");
    subLayout->addWidget(cancelButton);
    connect(cancelButton, SIGNAL(clicked()), this, SIGNAL(cancelled()));

    d->multiDropMode = true;
}

void DataDropWidget::requestDataDrop(QString identifier, QString name, bool withWindowLevel)
{
    d->currentDrop = identifier;

    clearWidget();

    QString labelText;
    labelText += "Drop the "  + name + " into the view";
    if (withWindowLevel)
    {
        labelText += " and set the window level if needed";
    }
    labelText += ".";
    QLabel* label = new QLabel(labelText);
    label->setWordWrap(true);
    layout()->addWidget(label);

    d->okButton = new QPushButton("Ok");
    d->okButton->setEnabled(false);
    layout()->addWidget(d->okButton);
    connect(d->okButton, SIGNAL(clicked()), this, SLOT(acceptDrop()));

    QPushButton* backButton = new QPushButton();
    if (d->multiDropMode)
    {
        backButton->setText("Back");
    }
    else
    {
        backButton->setText("Cancel");
    }
    layout()->addWidget(backButton);
    connect(backButton, SIGNAL(clicked()), this, SLOT(cancelDrop()));

    medTabbedViewContainers* viewContainers = PipelineWorkspace::instance()->tabbedViewContainers();
    d->viewContainer = viewContainers->addContainerInTabNamed(name);
    d->viewContainer->setMultiLayered(false);
    d->viewContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    viewContainers->lockTabs();

    PipelineWorkspace::instance()->setToolBoxVisibility(LAYERS_TOOLBOX, false);
    PipelineWorkspace::instance()->getContainer()->setMultiLayered(false);

    connect(viewContainers, SIGNAL(containersSelectedChanged()), this, SLOT(actOnContainerChange()), Qt::UniqueConnection);
}

void DataDropWidget::requestInitialDataDrop(QString identifier)
{
    requestDataDrop(identifier, d->pipeline->getInitialDataName(identifier), true);
}

void DataDropWidget::acceptDrop()
{
    medAbstractLayeredView* layeredView = qobject_cast<medAbstractLayeredView*>(d->viewContainer->view());
    QString identifier = d->currentDrop;
    dtkSmartPointer<medAbstractData> data = layeredView->layerData(0);
    QHash<QString, QVariant> windowLevel = d->windowLevel;
    endDrop();
    emit received(identifier, data, windowLevel);
}

void DataDropWidget::cancelDrop()
{
    endDrop();
    if (d->multiDropMode)
    {
        requestInitialDataDrop(d->pipeline);
    }
    else
    {
        emit cancelled();
    }
}

void DataDropWidget::actOnContainerChange()
{
    if (d->viewContainer && d->viewContainer->view())
    {
        d->okButton->setEnabled(true);
        connectWindowLevelIfPossible();
        switchToWindowingMode();
    }
    else
    {
        d->okButton->setEnabled(false);
    }
}

void DataDropWidget::windowLevelChanged(const QHash<QString, QVariant>& windowLevel)
{
    d->windowLevel = windowLevel;
    d->windowLevelChanged = true;
}

void DataDropWidget::endDrop()
{
    medTabbedViewContainers* viewContainers = PipelineWorkspace::instance()->tabbedViewContainers();
    disconnect(viewContainers, nullptr, this, nullptr);
    d->viewContainer = nullptr;
    clearWidget();
}

void DataDropWidget::clearWidget()
{
    d->okButton = nullptr;

    foreach (QWidget* child, findChildren<QWidget*>())
    {
        delete child;
    }
}

void DataDropWidget::switchToWindowingMode()
{
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(d->viewContainer->view()->backend());
    backend->view2D->SetLeftButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeWindowLevel);
}

void DataDropWidget::connectWindowLevelIfPossible()
{
    d->windowLevelChanged = false;

    medAbstractLayeredView* layeredView = qobject_cast<medAbstractLayeredView*>(d->viewContainer->view());

    foreach (medAbstractInteractor* interactor, layeredView->layerInteractors(0))
    {
        if (interactor->identifier() == "medVtkViewItkDataImageInteractor")
        {
            foreach (medAbstractParameterL* parameter, interactor->linkableParameters())
            {
                if (parameter->name() == "WindowLevel")
                {
                    medCompositeParameterL* compositeParameter = qobject_cast<medCompositeParameterL*>(parameter);

                    QObject::connect(compositeParameter, SIGNAL(valuesChanged(QHash<QString,QVariant>)),
                                     this, SLOT(windowLevelChanged(QHash<QString,QVariant>)), Qt::UniqueConnection);
                    return;
                }
            }
        }
    }
}

} // namespace med::pipeline
