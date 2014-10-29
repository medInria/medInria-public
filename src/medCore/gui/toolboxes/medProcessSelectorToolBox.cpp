/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medProcessSelectorToolBox.h>


#include <medAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medDataManager.h>

#include <medAbstractView.h>
#include <medAbstractImageView.h>
#include <medAbstractImageData.h>

#include <medToolBoxTab.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>

#include <medRegistrationAbstractToolBox.h>


#include <QtGui>

class medProcessSelectorToolBoxPrivate
{
public:
    QComboBox *processesCombobox;
    QVBoxLayout *toolBoxLayout;
    medToolBox * currentToolBox;
};

medProcessSelectorToolBox::medProcessSelectorToolBox(QWidget *parent) : medToolBox(parent), d(new medProcessSelectorToolBoxPrivate)
{

    // --- Setting up custom toolboxes list ---
    d->processesCombobox = new QComboBox(this);
    d->processesCombobox->addItem(tr("Choose algorithm"));
    d->processesCombobox->setToolTip(tr( "Choose the algorithm amongst the loaded plugins" ));

    QWidget *toolBoxWidget =  new QWidget;
    d->toolBoxLayout = new QVBoxLayout(toolBoxWidget);

    d->toolBoxLayout->addWidget(d->processesCombobox);

    this->addWidget(toolBoxWidget);

    d->currentToolBox = NULL;

    connect(d->processesCombobox, SIGNAL(activated(QString)), this, SIGNAL(processSelected(QString)));
}

medProcessSelectorToolBox::~medProcessSelectorToolBox(void)
{
    delete d;

    d = NULL;
}

void medProcessSelectorToolBox::setAvailableProcesses(QStringList processes)
{
    d->processesCombobox->addItems(processes);
}

void medProcessSelectorToolBox::setProcessToolbox(medToolBox* tb)
{
    if (d->currentToolBox)
    {
        d->currentToolBox->hide();
        d->toolBoxLayout->removeWidget(d->currentToolBox);
        d->currentToolBox = NULL;
    }

    d->currentToolBox = tb;
    d->currentToolBox->show();
    d->currentToolBox->header()->hide();
    d->toolBoxLayout->addWidget(d->currentToolBox);
}

//! Clears the toolbox.
void medProcessSelectorToolBox::clear(void)
{
    if (d->currentToolBox)
        d->currentToolBox->clear();
}

void medProcessSelectorToolBox::enableSelectorToolBox(bool enable)
{
    this->setEnabled(enable);
}

