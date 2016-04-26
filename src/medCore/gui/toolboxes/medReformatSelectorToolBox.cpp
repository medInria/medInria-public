/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medReformatAbstractToolBox.h>
#include <medReformatSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medToolBoxTab.h>
#include <medViewEventFilter.h>

class medReformatSelectorToolBoxPrivate
{
public:
    medComboBox *chooseReformatComboBox;
    medReformatAbstractToolBox * currentReformatToolBox;
    QHash<QString, medReformatAbstractToolBox*> reformatToolBoxes;
    QVBoxLayout *mainLayout;
};

medReformatSelectorToolBox::medReformatSelectorToolBox(QWidget *parent) :
    medToolBox(parent),
    d(new medReformatSelectorToolBoxPrivate)
{
    d->currentReformatToolBox = NULL;

    d->chooseReformatComboBox = new medComboBox;
    d->chooseReformatComboBox->addItem("Choose algorithm");
    d->chooseReformatComboBox->setToolTip(tr("Browse through the list of available Reformat algorithm"));

    medToolBoxFactory* tbFactory = medToolBoxFactory::instance();
    int i = 1; //account for the choose Filter item
    foreach(QString toolboxName, tbFactory->toolBoxesFromCategory("reformat"))
    {
        medToolBoxDetails* details = tbFactory->toolBoxDetailsFromId(toolboxName);
        d->chooseReformatComboBox->addItem(details->name, toolboxName);
        d->chooseReformatComboBox->setItemData(i,
                                                   details->description,
                                                   Qt::ToolTipRole);
        i++;
    }

    connect(d->chooseReformatComboBox, SIGNAL(activated(int)), this, SLOT(changeCurrentToolBox(int)));

    QWidget *mainWidget = new QWidget;
    d->mainLayout = new QVBoxLayout;

    d->chooseReformatComboBox->adjustSize();
    d->mainLayout->addWidget(d->chooseReformatComboBox);
    mainWidget->setLayout(d->mainLayout);
    this->addWidget(mainWidget);
    this->setTitle("Reformat");
}

medReformatSelectorToolBox::~medReformatSelectorToolBox(void)
{
    delete d;
    d = NULL;
}

medReformatAbstractToolBox* medReformatSelectorToolBox::currentToolBox()
{
    return d->currentReformatToolBox;
}

void medReformatSelectorToolBox::changeCurrentToolBox(int index)
{
    medReformatAbstractToolBox *toolbox = NULL;

    //get identifier for toolbox.
    QString identifier = d->chooseReformatComboBox->itemData(index).toString();

    if (d->reformatToolBoxes.contains (identifier))
    {
        toolbox = d->reformatToolBoxes[identifier];
    }
    else
    {
        medToolBox* tb = medToolBoxFactory::instance()->createToolBox(identifier, this);
        toolbox = qobject_cast<medReformatAbstractToolBox*>(tb);
        if (toolbox)
        {
            toolbox->setWorkspace(getWorkspace());
            toolbox->setStyleSheet("medToolBoxBody {border:none}");

            d->reformatToolBoxes[identifier] = toolbox;
        }
    }

    if(d->currentReformatToolBox)
    {
        // Remove previous tlbx from current tlbx
        d->currentReformatToolBox->hide();
        d->mainLayout->removeWidget(d->currentReformatToolBox);
        d->currentReformatToolBox = NULL;
    }

    if(toolbox)
    {
        d->currentReformatToolBox = toolbox;
        d->currentReformatToolBox->header()->hide();

        dtkPlugin* plugin = d->currentReformatToolBox->plugin();
        this->setAboutPluginButton(plugin);
        this->setAboutPluginVisibility(true);

        d->currentReformatToolBox->show();
        d->mainLayout->addWidget(d->currentReformatToolBox);

        connect ( d->currentReformatToolBox, SIGNAL(success()), this, SIGNAL(success()));
    }
    else
    {
        this->setAboutPluginVisibility(false);
    }
}
