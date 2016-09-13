/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSelectableToolBox.h>
#include <medSelectorToolBox.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medToolBoxTab.h>

class medSelectorToolBoxPrivate
{
public:
    medComboBox *chooseComboBox;
    medAbstractSelectableToolBox * currentToolBox;
    QHash<QString, medAbstractSelectableToolBox*> toolBoxes;
    QVBoxLayout *mainLayout;

    medAbstractData *inputData;
};

medSelectorToolBox::medSelectorToolBox(QWidget *parent, QString tlbxId) :
    medToolBox(parent),
    d(new medSelectorToolBoxPrivate)
{
    d->currentToolBox = NULL;

    d->chooseComboBox = new medComboBox;
    d->chooseComboBox->addItem("Choose algorithm");
    d->chooseComboBox->setToolTip(tr("Choose a toolbox"));

    medToolBoxFactory* tbFactory = medToolBoxFactory::instance();
    int i = 1; // toolboxes positions
    foreach(QString toolboxName, tbFactory->toolBoxesFromCategory(tlbxId))
    {
        medToolBoxDetails* details = tbFactory->toolBoxDetailsFromId(toolboxName);

        d->chooseComboBox->addItem(details->name, toolboxName);
        d->chooseComboBox->setItemData(i, details->description, Qt::ToolTipRole);
        i++;
    }

    connect(d->chooseComboBox, SIGNAL(activated(int)), this, SLOT(changeCurrentToolBox(int)));

    QWidget *mainWidget = new QWidget;
    d->mainLayout = new QVBoxLayout;

    d->chooseComboBox->adjustSize();
    d->mainLayout->addWidget(d->chooseComboBox);
    mainWidget->setLayout(d->mainLayout);

    this->addWidget(mainWidget);

    clear();
}

medSelectorToolBox::~medSelectorToolBox(void)
{
    delete d;
    d = NULL;
}

medAbstractSelectableToolBox* medSelectorToolBox::currentToolBox()
{
    return d->currentToolBox;
}

void medSelectorToolBox::changeCurrentToolBox ( int index )
{
    // Get current toolbox identifier from combobox
    QString identifier = d->chooseComboBox->itemData(index).toString();
    this->changeCurrentToolBox(identifier);
}

void medSelectorToolBox::changeCurrentToolBox(const QString &identifier)
{
    medAbstractSelectableToolBox *toolbox = NULL;

    if (d->toolBoxes.contains (identifier))
    {
        toolbox = d->toolBoxes[identifier];
    }
    else
    {
        medToolBox* tb = medToolBoxFactory::instance()->createToolBox(identifier, this);
        toolbox = qobject_cast<medAbstractSelectableToolBox*>(tb);
        if (toolbox)
        {
            toolbox->setWorkspace(getWorkspace());
            toolbox->setStyleSheet("medToolBoxBody {border:none}");
            d->toolBoxes[identifier] = toolbox;
        }
    }

    if(d->currentToolBox)
    {
        // Remove previous tlbx from current tlbx
        d->currentToolBox->hide();
        d->mainLayout->removeWidget(d->currentToolBox);
        d->currentToolBox = NULL;
    }

    if(toolbox)
    {
        d->currentToolBox = toolbox;
        d->currentToolBox->header()->hide();

        dtkPlugin* plugin = d->currentToolBox->plugin();
        this->setAboutPluginButton(plugin);
        this->setAboutPluginVisibility(true);

        d->currentToolBox->show();
        d->mainLayout->addWidget(d->currentToolBox);

        connect(d->currentToolBox, SIGNAL(success()), this, SIGNAL(success()));
        connect(d->currentToolBox, SIGNAL(failure()), this, SIGNAL(failure()));

        emit currentToolBoxChanged();
    }
    else
    {
        this->setAboutPluginVisibility(false);
    }
}

medAbstractData* medSelectorToolBox::data()
{
    return d->inputData;
}


medComboBox* medSelectorToolBox::comboBox()
{
    return d->chooseComboBox;
}

void medSelectorToolBox::onInputSelected(medAbstractData *data)
{
    d->inputData = data;
}

void medSelectorToolBox::clear()
{
    d->inputData = NULL;
}
