/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMessageController.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medCompositeDataSetImporterSelectorToolBox.h>
#include <medCompositeDataSetImporterAbstractToolBox.h>

class medCompositeDataSetImporterSelectorToolBoxPrivate
{
public:

  QHash<QString, medCompositeDataSetImporterAbstractToolBox*> toolBoxes;

  medCompositeDataSetImporterAbstractToolBox* currentToolBox;

  // methods
  void read(QString filename);

  // member
  QWidget* parent;
  QVBoxLayout* customContainerLayout;

  QComboBox* type;
  QPushButton* import;
  QPushButton* reset;
  QPushButton* load;
  bool isInitialized;
};

medCompositeDataSetImporterSelectorToolBox::medCompositeDataSetImporterSelectorToolBox(QWidget *parent): medToolBox(parent), d(new medCompositeDataSetImporterSelectorToolBoxPrivate) {
    d->isInitialized = false;
    d->currentToolBox = 0;
    this->setTitle(tr("Composite DataSet Import"));
    this->initialize();
}

medCompositeDataSetImporterSelectorToolBox::~medCompositeDataSetImporterSelectorToolBox(void)
{
    delete d;
    d = NULL;
}

void medCompositeDataSetImporterSelectorToolBoxPrivate::read(QString filename) { }

/**
 * @brief Performs validation tests on each section and tires to save.
 *
 * If any section fails, an error message is emitted for it, and the other sections are still saved.
 *
 * The widget closes if the save action is successful.
 *
*/
void medCompositeDataSetImporterSelectorToolBox::onImportClicked() {
    if (!d->currentToolBox) {
        this->showError(tr("Select a type first"),3000);
        return;
    }
    d->currentToolBox->import();
}

/**
 * @brief Load in non-persistent database.
 *
*/
void medCompositeDataSetImporterSelectorToolBox::onLoadClicked() {
    if (!d->currentToolBox)
        return;
    d->currentToolBox->load();
}

/**
 * @brief reset the display to the last saved values.
 *
*/
void medCompositeDataSetImporterSelectorToolBox::onResetClicked() {
    if (!d->currentToolBox)
        return;
    d->currentToolBox->reset();
}

/**
 * @brief initialize layout
 */
void medCompositeDataSetImporterSelectorToolBox::initialize()
{
    if (d->isInitialized)
        return;
    QWidget* mainwidget = new QWidget(this);

    int buttonWidth = 100;

    QVBoxLayout * vLayout = new QVBoxLayout();
    mainwidget->setLayout (vLayout);

    // the row of buttons
    QHBoxLayout * topLayout = new QHBoxLayout();
    topLayout->addStretch(0);

    d->type = new QComboBox(mainwidget);
    d->type->addItem(tr("Select data type"));
    d->type->setToolTip(tr("Choose a type of composite data set to import"));

    int i=1;
    medToolBoxFactory* factory = medToolBoxFactory::instance();
    foreach (QString toolbox,factory->toolBoxesFromCategory("composite_dataset"))
    {
        medToolBoxDetails* details = factory->toolBoxDetailsFromId(toolbox);

        d->type->addItem(details->name, toolbox);
        d->type->setItemData(i, details->description, Qt::ToolTipRole);
        i++;
    }

    connect(d->type,SIGNAL(activated(int)),this,SLOT(onCurrentTypeChanged(int)));

    topLayout->addWidget(d->type);
    topLayout->addStretch(1);
    vLayout->addLayout(topLayout);

    d->customContainerLayout = new QVBoxLayout();

    vLayout->addLayout(d->customContainerLayout);

    //  Import button
    d->import = new QPushButton (tr("Import"),mainwidget);
    d->import->setMaximumWidth(buttonWidth);
    connect(d->import,SIGNAL(clicked()),this,SLOT(onImportClicked()));

    //  Load button
    d->load = new QPushButton (tr("Load"),mainwidget);
    d->load->setMaximumWidth(buttonWidth);
    connect(d->load,SIGNAL(clicked()),this,SLOT(onLoadClicked()));

    //  Reset button
    d->reset = new QPushButton (tr("Reset"), mainwidget);
    d->reset->setMaximumWidth(buttonWidth);
    connect(d->reset,SIGNAL(clicked()),this,SLOT(onResetClicked()));

    // the row of buttons
    QHBoxLayout * buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(d->reset,1);
    buttonLayout->addWidget(d->load,1);
    buttonLayout->addWidget(d->import,1);
    vLayout->addLayout(buttonLayout);

    d->reset->hide ();
    d->load->hide ();
    d->import->hide ();

    this->addWidget (mainwidget);

    // connections
    connect(this,SIGNAL(showError(const        QString&,unsigned int)),
        medMessageController::instance(),SLOT(showError (const QString&,unsigned int)));
    connect(this,SIGNAL(showInfo(const        QString&,unsigned int)),
        medMessageController::instance(),SLOT(showInfo (const QString&,unsigned int)));
    //connect(this,SIGNAL(success()),this,SLOT(onSuccess()));

    d->isInitialized = true;
}

/**
 * Call save on all child widgets and return the status
 */
bool medCompositeDataSetImporterSelectorToolBox::import()
{
    return true;
}


void medCompositeDataSetImporterSelectorToolBox::onCurrentTypeChanged(const int i) {

    medCompositeDataSetImporterAbstractToolBox* toolbox = NULL;
    const QString id = d->type->itemData(i).toString();

    if (d->toolBoxes.contains(id))
        toolbox = d->toolBoxes[id];
    else {
        toolbox = qobject_cast<medCompositeDataSetImporterAbstractToolBox*>(medToolBoxFactory::instance()->createToolBox(id, this));
        if (toolbox) {
            toolbox->header()->hide();

            connect (toolbox, SIGNAL (success()), this, SIGNAL (success()));
            connect (toolbox, SIGNAL (failure()), this, SIGNAL (failure()));

            d->toolBoxes[id] = toolbox;
        }
    }

    if (!toolbox) {
        if (d->currentToolBox) {
            d->currentToolBox->hide();
            d->customContainerLayout->removeWidget ( d->currentToolBox );
            d->currentToolBox = 0;
        }
        d->reset->hide ();
        d->load->hide ();
        d->import->hide ();
        return;
    }

    toolbox->setCompositeDataSetImporterToolBox(this);

    //get rid of old toolBox
    if (d->currentToolBox) {
        d->currentToolBox->hide();
        d->customContainerLayout->removeWidget ( d->currentToolBox );
        d->currentToolBox = 0;
    }

    toolbox->show();
    d->customContainerLayout->addWidget ( toolbox );

    d->currentToolBox = toolbox;

    d->reset->show ();
    d->load->show ();
    d->import->show ();
}
