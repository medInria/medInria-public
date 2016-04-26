/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMeshingAbstractToolBox.h>
#include <medMeshingSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medToolBoxTab.h>
#include <medViewEventFilter.h>

class medMeshingSelectorToolBoxPrivate
{
public:
    medComboBox *chooseMeshingComboBox;
    medMeshingAbstractToolBox * currentMeshingToolBox;
    QHash<QString, medMeshingAbstractToolBox*> meshingToolBoxes;
    QVBoxLayout *mainLayout;
};

medMeshingSelectorToolBox::medMeshingSelectorToolBox(QWidget *parent) :
    medToolBox(parent),
    d(new medMeshingSelectorToolBoxPrivate)
{
    d->currentMeshingToolBox = NULL;

    d->chooseMeshingComboBox = new medComboBox;
    d->chooseMeshingComboBox->addItem("Choose algorithm");
    d->chooseMeshingComboBox->setToolTip(tr("Browse through the list of available Meshing algorithm"));

    medToolBoxFactory* tbFactory = medToolBoxFactory::instance();
    int i = 1; //account for the choose Filter item
    foreach(QString toolboxName, tbFactory->toolBoxesFromCategory("mesh"))
    {
        medToolBoxDetails* details = tbFactory->toolBoxDetailsFromId(toolboxName);
        d->chooseMeshingComboBox->addItem(details->name, toolboxName);
        d->chooseMeshingComboBox->setItemData(i,
                                                   details->description,
                                                   Qt::ToolTipRole);
        i++;
    }

    connect(d->chooseMeshingComboBox, SIGNAL(activated(int)), this, SLOT(changeCurrentToolBox(int)));

    QWidget *mainWidget = new QWidget;
    d->mainLayout = new QVBoxLayout;

    d->chooseMeshingComboBox->adjustSize();
    d->mainLayout->addWidget(d->chooseMeshingComboBox);
    mainWidget->setLayout(d->mainLayout);
    this->addWidget(mainWidget);
    this->setTitle("Meshing");
}

medMeshingSelectorToolBox::~medMeshingSelectorToolBox(void)
{
    delete d;
    d = NULL;
}

medMeshingAbstractToolBox* medMeshingSelectorToolBox::currentToolBox()
{
    return d->currentMeshingToolBox;
}

void medMeshingSelectorToolBox::changeCurrentToolBox(int index)
{
    medMeshingAbstractToolBox *toolbox = NULL;

    //get identifier for toolbox.
    QString identifier = d->chooseMeshingComboBox->itemData(index).toString();

    if (d->meshingToolBoxes.contains (identifier))
    {
        toolbox = d->meshingToolBoxes[identifier];
    }
    else
    {
        medToolBox* tb = medToolBoxFactory::instance()->createToolBox(identifier, this);
        toolbox = qobject_cast<medMeshingAbstractToolBox*>(tb);
        if (toolbox)
        {
            toolbox->setWorkspace(getWorkspace());
            toolbox->setStyleSheet("medToolBoxBody {border:none}");

            d->meshingToolBoxes[identifier] = toolbox;
        }
    }

    if(d->currentMeshingToolBox)
    {
        // Remove previous tlbx from current tlbx
        d->currentMeshingToolBox->hide();
        d->mainLayout->removeWidget(d->currentMeshingToolBox);
        d->currentMeshingToolBox = NULL;
    }

    if(toolbox)
    {
        d->currentMeshingToolBox = toolbox;
        d->currentMeshingToolBox->header()->hide();

        dtkPlugin* plugin = d->currentMeshingToolBox->plugin();
        this->setAboutPluginButton(plugin);
        this->setAboutPluginVisibility(true);

        d->currentMeshingToolBox->show();
        d->mainLayout->addWidget(d->currentMeshingToolBox);

        connect ( d->currentMeshingToolBox, SIGNAL(success()), this, SIGNAL(success()));
    }
    else
    {
        this->setAboutPluginVisibility(false);
    }
}
