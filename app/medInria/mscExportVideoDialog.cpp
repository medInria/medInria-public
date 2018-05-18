
#include "mscExportVideoDialog.h"

#include <medComboBox.h>
#include <medIntParameter.h>

class mscExportVideoDialogPrivate
{
public:

    medComboBox* typeCombobox;
    medIntParameter* nbStepRotation;
    int numberOfFrames;
};

mscExportVideoDialog::mscExportVideoDialog(QWidget *parent, int numberOfFrames): QDialog(parent,
                Qt::Dialog | Qt::WindowCloseButtonHint), d (new mscExportVideoDialogPrivate)

{
    QVBoxLayout* dialogLayout = new QVBoxLayout;

    // Warning
    QLabel* warningLabel = new QLabel(tr("Warning: do not hide your view during the process!\n"
                                         "This tool uses screenshots of the view to record your video.\n"));
    warningLabel->setStyleSheet("font-weight: bold; color: red");
    dialogLayout->addWidget(warningLabel);

    // Type layout
    QHBoxLayout* typeLayout = new QHBoxLayout;
    QLabel* explanationLabel = new QLabel(tr("Please choose a video recording type"));
    typeLayout->addWidget(explanationLabel);

    d->typeCombobox = new medComboBox;
    d->typeCombobox->addItem("Time",     ExportVideoName::TIME);
    d->typeCombobox->addItem("Rotation", ExportVideoName::ROTATION);
    typeLayout->addWidget(d->typeCombobox);

    QObject::connect (d->typeCombobox, SIGNAL(activated(int)), this, SLOT(adaptWidgetForMethod(int)));

    dialogLayout->addLayout(typeLayout);

    // Step parameter
    QHBoxLayout* stepLayout = new QHBoxLayout;
    d->nbStepRotation = new medIntParameter("Recording step", this);
    d->nbStepRotation->setValue(10);
    d->numberOfFrames = numberOfFrames;
    d->nbStepRotation->setRange(1, numberOfFrames);
    d->nbStepRotation->getSlider()->setOrientation(Qt::Horizontal);
    stepLayout->addWidget(d->nbStepRotation->getLabel());
    stepLayout->addWidget(d->nbStepRotation->getSlider());
    stepLayout->addWidget(d->nbStepRotation->getSpinBox());

    dialogLayout->addLayout(stepLayout);

    // OK/Cancel buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* okButton     = new QPushButton(tr("OK"));
    QPushButton* cancelButton = new QPushButton(tr("Cancel"));
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    dialogLayout->addLayout(buttonLayout);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(okButton,     SIGNAL(clicked()), this, SLOT(validate()));

    setLayout(dialogLayout);
    setModal(true);
}

mscExportVideoDialog::~mscExportVideoDialog()
{
    delete d;
}

void mscExportVideoDialog::cancel()
{
    this->reject();
}

void mscExportVideoDialog::validate()
{
    this->accept();
}

QVector<int> mscExportVideoDialog::value()
{
    QVector<int> resultsVector;
    resultsVector.append(d->typeCombobox->currentIndex());
    resultsVector.append(d->nbStepRotation->value());
    return resultsVector;
}

void mscExportVideoDialog::adaptWidgetForMethod(int method)
{
    switch (method)
    {
    case ExportVideoName::TIME:
    {
        d->nbStepRotation->setRange(1, d->numberOfFrames);
        break;
    }
    case ExportVideoName::ROTATION:
    {
        d->nbStepRotation->setRange(1, 360);
        break;
    }
    default:
    {
        break;
    }
    }
}
