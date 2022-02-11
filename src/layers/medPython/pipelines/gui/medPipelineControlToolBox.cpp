#include "medPipelineControlToolBox.h"

#include <medDataManager.h>
#include <medDropSite.h>
#include <medMetaDataKeys.h>
#include <medPluginManager.h>
#include <medToolBoxBody.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>

#include "medPipelineDataDropWidget.h"
#include "medPipelineStepChooserWidget.h"
#include "medPipelineWorkspace.h"

namespace med::pipeline
{

class PipelineControlToolBoxPrivate
{
public:
    dtkSmartPointer<PipelineInstance> pipeline;
    QWidget* topWidget;
    QWidget* infoHeader;
    QLabel* seriesLabel;
    QLabel* patientLabel;
    QWidget* mainMenu;
    QLabel* stepTitle;
    QLabel* stepDescription;
    QPushButton* nextStepButton;
    QPushButton* skipStepButton;
    QPushButton* previousStepButton;
    QPushButton* resetStepButton;
    QPushButton* chooseStepButton;
    QPushButton* quitButton;
    QCheckBox* developerModeCheckBox;
    QPushButton* dropResultsButton;
    QWidget* resultDropSitesWidget;
    QList<medDropSite*> resultDropSites;
    QList<dtkSmartPointer<medAbstractData> > droppedResults;
};

bool PipelineControlToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<PipelineControlToolBox>();
}

PipelineControlToolBox::PipelineControlToolBox(QWidget* parent) : medToolBox(parent), d(new PipelineControlToolBoxPrivate())
{
    d->topWidget = new QWidget();
    d->topWidget->setLayout(new QVBoxLayout());
    addWidget(d->topWidget);

    d->developerModeCheckBox = new QCheckBox("developer mode");
    d->developerModeCheckBox->setText("developer mode");
    QFont font = d->developerModeCheckBox->font();
    font.setPointSize(font.pointSize() - 2);
    d->developerModeCheckBox->setFont(font);
    d->topWidget->layout()->addWidget(d->developerModeCheckBox);
    connect(d->developerModeCheckBox, SIGNAL(toggled(bool)), this, SLOT(setDeveloperModeEnabled(bool)));

    d->infoHeader = new QWidget();
    d->infoHeader->setLayout(new QVBoxLayout());
    d->topWidget->layout()->addWidget(d->infoHeader);

    d->seriesLabel = new QLabel();
    d->patientLabel = new QLabel();
    d->infoHeader->layout()->addWidget(d->seriesLabel);
    d->infoHeader->layout()->addWidget(d->patientLabel);

    QFont seriesLabelFont = d->seriesLabel->font();
    seriesLabelFont.setPointSize(seriesLabelFont.pointSize() - 2);
    d->seriesLabel->setFont(seriesLabelFont);
    d->patientLabel->setFont(seriesLabelFont);

    d->mainMenu = new QWidget();
    QVBoxLayout* mainMenuLayout = new QVBoxLayout();
    d->mainMenu->setLayout(mainMenuLayout);
    d->topWidget->layout()->addWidget(d->mainMenu);

    d->stepTitle = new QLabel();
    mainMenuLayout->addWidget(d->stepTitle);

    QHBoxLayout* rowLayouts[3];
    for (int i = 0; i < 3; i++)
    {
        rowLayouts[i] = new QHBoxLayout();
        mainMenuLayout->addLayout(rowLayouts[i]);
    }

    d->nextStepButton = new QPushButton("Next");
    d->nextStepButton->setObjectName("nextStepButton");
    rowLayouts[0]->addWidget(d->nextStepButton);
    connect(d->nextStepButton, SIGNAL(clicked()), this, SLOT(next()));

    QFont nextButtonFont = d->nextStepButton->font();
    nextButtonFont.setPointSize(nextButtonFont.pointSize() + 2);
    d->nextStepButton->setFont(nextButtonFont);

    d->previousStepButton = new QPushButton("Previous");
    d->previousStepButton->setObjectName("previousStepButton");
    rowLayouts[1]->addWidget(d->previousStepButton);
    connect(d->previousStepButton, SIGNAL(clicked()), this, SLOT(previous()));

    d->resetStepButton = new QPushButton("Reset");
    d->resetStepButton->setObjectName("resetStepButton");
    rowLayouts[1]->addWidget(d->resetStepButton);
    connect(d->resetStepButton, SIGNAL(clicked()), this, SLOT(reset()));

    d->skipStepButton = new QPushButton("Skip");
    d->skipStepButton->setObjectName("skipStepButton");
    rowLayouts[1]->addWidget(d->skipStepButton);
    connect(d->skipStepButton, SIGNAL(clicked()), this, SLOT(skip()));

    d->dropResultsButton = new QPushButton("Drop results");
    d->dropResultsButton->setObjectName("dropResultsButton");
    d->dropResultsButton->setCheckable(true);
    rowLayouts[2]->addWidget(d->dropResultsButton);
    connect(d->dropResultsButton, SIGNAL(clicked()), this, SLOT(updateResultDropVisibility()));

    d->chooseStepButton = new QPushButton("Choose step");
    d->chooseStepButton->setObjectName("chooseStepButton");
    rowLayouts[2]->addWidget(d->chooseStepButton);
    connect(d->chooseStepButton, SIGNAL(clicked()), this, SLOT(tryChooseStep()));

    d->quitButton = new QPushButton("Quit");
    d->quitButton->setObjectName("quitButton");

    rowLayouts[2]->addWidget(d->quitButton);
    connect(d->quitButton, SIGNAL(clicked()), this, SLOT(quit()));

    d->stepDescription = new QLabel();
    d->stepDescription->setWordWrap(true);
    mainMenuLayout->addWidget(d->stepDescription);

    d->resultDropSitesWidget = new QWidget();
    d->resultDropSitesWidget->setLayout(new QVBoxLayout());
    mainMenuLayout->addWidget(d->resultDropSitesWidget);
    QLabel* dropLabel = new QLabel("You can execute this step outside the pipeline and drop the results below:");
    dropLabel->setWordWrap(true);
    d->resultDropSitesWidget->layout()->addWidget(dropLabel);
    clearResultDrops();
}

PipelineControlToolBox::~PipelineControlToolBox(void)
{
    delete d;
}

void PipelineControlToolBox::setPipeline(dtkSmartPointer<PipelineInstance> pipeline)
{
    d->pipeline = pipeline;

    if (d->pipeline)
    {
        connect(d->pipeline.data(), SIGNAL(validationEnabledChanged()), this, SLOT(updateGUI()));
        updateGUI();

        if (pipelineHasSomeInitialData())
        {
            chooseStep();
        }
        else
        {
            addInitialData();
        }
    }
}

bool PipelineControlToolBox::developerModeEnabled()
{
    return d->developerModeCheckBox->isChecked();
}

QList<dtkSmartPointer<medAbstractData> > PipelineControlToolBox::getDroppedResults()
{
    return d->droppedResults;
}

void PipelineControlToolBox::enableValidation()
{
    d->pipeline->enableValidation();
}

void PipelineControlToolBox::disableValidation()
{
    d->pipeline->disableValidation();
}

void PipelineControlToolBox::updateGUI()
{
    setTitle(d->pipeline->getTitle() + " pipeline");

    if (d->pipeline->hasMetaData(medMetaDataKeys::SeriesDescription.key()))
    {
        d->infoHeader->show();
        d->seriesLabel->setText(QString("Study: ") + d->pipeline->metadata(medMetaDataKeys::StudyDescription.key()));
        d->patientLabel->setText(QString("Patient: ") + d->pipeline->metadata(medMetaDataKeys::PatientName.key()));
    }
    else
    {
        d->infoHeader->hide();
    }

    if (d->pipeline->isProperStep())
    {
        bool isLastStep = d->pipeline->isLastStep();
        bool isValidationEnabled = d->pipeline->isValidationEnabled();
        bool hasPreviousStep = (d->pipeline->getPreviousExecutableStep() >= 0);
        bool hasNextStep = (d->pipeline->getNextExecutableStep() >= 0);

        d->nextStepButton->setEnabled(isValidationEnabled && hasNextStep);
        d->skipStepButton->setEnabled(hasNextStep);
        d->resetStepButton->setEnabled(!isLastStep);
        d->previousStepButton->setEnabled(hasPreviousStep);

        PipelineStepProperties stepProperties = d->pipeline->getCurrentStepProperties();

        d->stepDescription->setText(stepProperties.getDescription());
        d->stepTitle->setText(QString::number(d->pipeline->getStep() + 1) + ". " + stepProperties.getTitle());

        d->dropResultsButton->setEnabled(stepProperties.getResultDropMode() != NO_RESULT_DROP);

        updateResultDropGUI();
    }
}

void PipelineControlToolBox::addInitialData()
{
    d->mainMenu->hide();

    DataDropWidget* dataDropWidget = new DataDropWidget();
    d->topWidget->layout()->addWidget(dataDropWidget);
    connect(dataDropWidget, SIGNAL(received(QString, dtkSmartPointer<medAbstractData>, QHash<QString,QVariant>)),
            this, SLOT(initialDataAdded(QString, dtkSmartPointer<medAbstractData>, QHash<QString,QVariant>)));
    connect(dataDropWidget, SIGNAL(received(QString, dtkSmartPointer<medAbstractData>, QHash<QString,QVariant>)),
            dataDropWidget, SLOT(deleteLater()));
    connect(dataDropWidget, SIGNAL(cancelled()), this, SLOT(cancelledInitialDataDrop()));
    connect(dataDropWidget, SIGNAL(cancelled()), dataDropWidget, SLOT(deleteLater()));
    connect(dataDropWidget, SIGNAL(finished()), this, SLOT(chooseStep()));
    connect(dataDropWidget, SIGNAL(finished()), dataDropWidget, SLOT(deleteLater()));

    dataDropWidget->requestInitialDataDrop(d->pipeline);
}

void PipelineControlToolBox::initialDataAdded(QString identifier, dtkSmartPointer<medAbstractData> value,
                                          QHash<QString, QVariant> windowLevel)
{
    d->pipeline->setInitialDataValue(identifier, value, windowLevel);
    PipelineWorkspace::instance()->secureImport(d->pipeline);
    updateGUI();

    if (!pipelineHasAllInitialData())
    {
        addInitialData();
    }
    else
    {
        chooseStep();
    }
}

void PipelineControlToolBox::cancelledInitialDataDrop()
{
    medDataIndex studyIndex = d->pipeline->dataIndex();
    studyIndex.setSeriesId(medDataIndex::NOT_VALID);
    medDataManager::instance()->removeData(studyIndex);
    quit();
}

void PipelineControlToolBox::chooseStep()
{
    d->mainMenu->hide();

    PipelineStepChooserWidget* stepChooserWidget = new PipelineStepChooserWidget();
    stepChooserWidget->setPipeline(d->pipeline);
    d->topWidget->layout()->addWidget(stepChooserWidget);
    connect(stepChooserWidget, SIGNAL(stepChosen(int)), this, SLOT(runStep(int)));
    connect(stepChooserWidget, SIGNAL(stepChosen(int)), stepChooserWidget, SLOT(deleteLater()));
    connect(stepChooserWidget, SIGNAL(quit()), this, SLOT(quit()));
    connect(stepChooserWidget, SIGNAL(quit()), stepChooserWidget, SLOT(deleteLater()));
}

void PipelineControlToolBox::tryChooseStep()
{
    if (!d->pipeline->isProperStep())
    {
        chooseStep();
    }
    else
    {
        if (d->pipeline->isLastStep())
        {
            finalizeStep(false);
            chooseStep();
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setDefaultButton(QMessageBox::Cancel);

            if (d->pipeline->isValidationEnabled())
            {
                msgBox.setText("Do you want to validate the current step?");
                msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);
            }
            else
            {
                msgBox.setText("The current step cannot be validated right now. You will loose any progress on this step. Do you still want to change steps?");
                msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
            }

            switch (msgBox.exec())
            {
            case QMessageBox::Yes:
                finalizeStep(d->pipeline->isValidationEnabled());
                chooseStep();
                break;
            case QMessageBox::No:
                finalizeStep(false);
                chooseStep();
                break;
            case QMessageBox::Cancel:
                break;
            default:
                throw std::invalid_argument("medPipelineControlToolBox::tryChooseStep wrong type given.");
            }
        }
    }
}

void PipelineControlToolBox::runStep(int step)
{
    d->pipeline->setStep(step);
    d->mainMenu->show();

    QString errorMessage;

    if (d->pipeline->tryStartStep(&errorMessage))
    {
        PipelineStepProperties& stepProperties = d->pipeline->getCurrentStepProperties();
        updateGUI();
        if (!stepProperties.isAutomatic() || stepProperties.wasExecuted())
        {
            setEnabled(true);
        }
        else
        {
            setDisabled(true);
            if (finalizeStep(true))
            {
                runStep(d->pipeline->getNextExecutableStep());
            }
            else
            {
                setEnabled(true);
                chooseStep();
            }

        }
    }
    else
    {
        displayInformativeMessageBox("The step could not be executed:", errorMessage);
        setEnabled(true);
        chooseStep();

    }
}

void PipelineControlToolBox::next()
{
    setEnabled(false);

    if (finalizeStep(true))
    {
        runStep(d->pipeline->getNextExecutableStep());
    }
    else
    {
        runStep(d->pipeline->getStep());
    }
}

void PipelineControlToolBox::previous()
{
    setEnabled(false);
    finalizeStep(false);
    runStep(d->pipeline->getPreviousExecutableStep());
}

void PipelineControlToolBox::reset()
{
    setEnabled(false);
    finalizeStep(false);
    runStep(d->pipeline->getStep());
}

void PipelineControlToolBox::skip()
{
    setEnabled(false);
    finalizeStep(false);
    runStep(d->pipeline->getNextExecutableStep());
}

void PipelineControlToolBox::quit()
{
    setPipeline(0);
}

void PipelineControlToolBox::setDeveloperModeEnabled(bool enabled)
{
    // setStyleSheet resets the font so it must be restored
    QFont font = d->developerModeCheckBox->font();

    if (enabled)
    {
        d->developerModeCheckBox->setStyleSheet("color: red;");
    }
    else
    {
        d->developerModeCheckBox->setStyleSheet("");
    }

    d->developerModeCheckBox->setFont(font);
}

void PipelineControlToolBox::updateResultDropVisibility()
{
    d->resultDropSitesWidget->setVisible(d->dropResultsButton->isChecked());
}

void PipelineControlToolBox::addDroppedResult(const medDataIndex& index)
{
    dtkSmartPointer<medAbstractData> data = medDataManager::instance()->retrieveData(index);
    d->droppedResults.append(data);

    medDropSite* dropSite = d->resultDropSites.last();
    dropSite->setPixmap(medDataManager::instance()->thumbnail(index).scaled(dropSite->sizeHint()));

    if (d->pipeline->getCurrentStepProperties().getResultDropMode() == SINGLE_RESULT_DROP)
    {
        next();
    }
    else
    {
        addResultDropSite();
    }
}

bool PipelineControlToolBox::finalizeStep(bool validate)
{
    QString errorMessage;
    bool success = d->pipeline->tryEndStep(validate, &errorMessage);
    clearResultDrops();

    if (!success)
    {
        displayInformativeMessageBox("The step could not be validated:", errorMessage);
        return false;
    }
    else
    {
        return true;
    }
}

void PipelineControlToolBox::displayInformativeMessageBox(QString text, QString informativeText)
{
    qDebug() << text;
    qDebug() << informativeText;

    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.setInformativeText(informativeText);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

bool PipelineControlToolBox::pipelineHasAllInitialData()
{
    foreach (QString identifier, d->pipeline->getInitialDataIdentifiers())
    {
        if (!d->pipeline->initialDataWasSet(identifier))
        {
            return false;
        }
    }
    return true;
}

bool PipelineControlToolBox::pipelineHasSomeInitialData()
{
    foreach (QString identifier, d->pipeline->getInitialDataIdentifiers())
    {
        if (d->pipeline->initialDataWasSet(identifier))
        {
            return true;
        }
    }
    return false;
}

void PipelineControlToolBox::clearResultDrops()
{
    d->droppedResults.clear();

    foreach (medDropSite* dropSite, d->resultDropSites)
    {
        d->resultDropSitesWidget->layout()->removeWidget(dropSite);
    }

    addResultDropSite();

    d->dropResultsButton->setChecked(false);
    updateResultDropVisibility();
}

void PipelineControlToolBox::addResultDropSite()
{
    medDropSite* dropSite = new medDropSite();
    d->resultDropSitesWidget->layout()->addWidget(dropSite);
    d->resultDropSites.append(dropSite);
    dropSite->setCanAutomaticallyChangeAppereance(false);
    dropSite->setMargin(10);
    dropSite->setToolTip(tr("Externally generated results should be obtained from the same input data used by this step."));
    connect(dropSite, SIGNAL(objectDropped(medDataIndex)), this, SLOT(addDroppedResult(medDataIndex)));
}

void PipelineControlToolBox::updateResultDropGUI()
{
    PipelineStepProperties stepProperties = d->pipeline->getCurrentStepProperties();

    if (!stepProperties.isExecutable())
    {
        d->stepDescription->setText("This step cannot be executed in the pipeline at the moment. Required data is missing or has not been generated yet.");
        d->dropResultsButton->setChecked(true);
        updateResultDropVisibility();
    }

    switch (d->pipeline->getCurrentStepProperties().getResultDropMode())
    {
    case SINGLE_RESULT_DROP:
        d->resultDropSites.last()->setText("Results");
        break;
    case MULTIPLE_RESULT_DROPS:
        d->resultDropSites.last()->setText("Results\n(click Next\nwhen finished)");
        break;
    default:
        break;
    }
}

} // namespace med::pipeline
