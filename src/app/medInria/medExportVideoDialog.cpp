
#include "medExportVideoDialog.h"

#include <medComboBox.h>
#include <medIntParameterL.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

class medExportVideoDialogPrivate
{
public:

    medComboBox *typeCombobox;
    medIntParameterL *nbStep;
    int numberOfFrames;
    int numberOfSlices;
};

medExportVideoDialog::medExportVideoDialog(QWidget *parent, int numberOfFrames, int numberOfSlices): QDialog(parent,
                Qt::Dialog | Qt::WindowCloseButtonHint), d (new medExportVideoDialogPrivate)

{
    QVBoxLayout *dialogLayout = new QVBoxLayout;

    // Warning
    QLabel *warningLabel = new QLabel(tr("Warning: do not hide your view during the process!\n"
                                         "This tool uses screenshots of the view to record your video.\n"));
    warningLabel->setStyleSheet("font-weight: bold; color: red");
    dialogLayout->addWidget(warningLabel);

    // Type layout
    QHBoxLayout *typeLayout = new QHBoxLayout;
    QLabel *explanationLabel = new QLabel(tr("Please choose a video recording type"));
    typeLayout->addWidget(explanationLabel);

    d->typeCombobox = new medComboBox;
    d->typeCombobox->addItem("Time",     ExportVideoName::TIME);
    d->typeCombobox->addItem("Rotation", ExportVideoName::ROTATION);
    d->typeCombobox->addItem("Slice",    ExportVideoName::SLICE);
    typeLayout->addWidget(d->typeCombobox);

    QObject::connect (d->typeCombobox, SIGNAL(activated(int)), this, SLOT(adaptWidgetForMethod(int)));

    dialogLayout->addLayout(typeLayout);

    // Step parameter
    QHBoxLayout *stepLayout = new QHBoxLayout;
    d->nbStep = new medIntParameterL("Recording step", this);
    d->nbStep->setValue(1);
    d->numberOfFrames = numberOfFrames;
    d->nbStep->setRange(1, numberOfFrames);
    d->nbStep->getSlider()->setOrientation(Qt::Horizontal);
    stepLayout->addWidget(d->nbStep->getLabel());
    stepLayout->addWidget(d->nbStep->getSlider());
    stepLayout->addWidget(d->nbStep->getSpinBox());

    d->numberOfSlices = numberOfSlices;

    dialogLayout->addLayout(stepLayout);

    // OK/Cancel buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *okButton     = new QPushButton(tr("OK"));
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    dialogLayout->addLayout(buttonLayout);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(okButton,     SIGNAL(clicked()), this, SLOT(validate()));

    setLayout(dialogLayout);
    setModal(true);
}

medExportVideoDialog::~medExportVideoDialog()
{
    delete d;
}

void medExportVideoDialog::cancel()
{
    this->reject();
}

void medExportVideoDialog::validate()
{
    this->accept();
}

QVector<int> medExportVideoDialog::value()
{
    QVector<int> resultsVector;
    resultsVector.append(d->typeCombobox->currentIndex());
    resultsVector.append(d->nbStep->value());
    return resultsVector;
}

void medExportVideoDialog::adaptWidgetForMethod(int method)
{
    switch (method)
    {
        case ExportVideoName::TIME:
        {
            d->nbStep->setRange(1, d->numberOfFrames);
            break;
        }
        case ExportVideoName::ROTATION:
        {
            d->nbStep->setRange(1, 360);
            break;
        }
        case ExportVideoName::SLICE:
        {
            d->nbStep->setRange(1, d->numberOfSlices);
            break;
        }
        default:
        {
            break;
        }
    }
}
