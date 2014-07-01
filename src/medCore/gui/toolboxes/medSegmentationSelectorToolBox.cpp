/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSegmentationSelectorToolBox.h>

#include <medToolBoxFactory.h>
#include <medToolBoxTab.h>
#include <medSegmentationAbstractToolBox.h>
#include <medToolBoxHeader.h>
#include <medViewEventFilter.h>

#include <QtGui>


class medSegmentationSelectorToolBoxPrivate
{
public:
    QComboBox *chooseSegmentationComboBox;
    medSegmentationAbstractToolBox * currentSegmentationToolBox;
    QHash<QString, medSegmentationAbstractToolBox*> segmentationToolBoxes;
    QVBoxLayout *mainLayout;
    medAbstractWorkspace * workspace;
};

medSegmentationSelectorToolBox::medSegmentationSelectorToolBox(QWidget *parent):
    medToolBox(parent),
    d(new medSegmentationSelectorToolBoxPrivate)
{
    d->workspace = NULL;
    d->currentSegmentationToolBox = NULL;
    d->chooseSegmentationComboBox = new QComboBox;
    //TODO algorithm is not the best IMO - RDE
    d->chooseSegmentationComboBox->addItem("Choose algorithm");
    d->chooseSegmentationComboBox->setToolTip(tr("Browse through the list of available segmentation algorithm"));

    medToolBoxFactory* tbFactory = medToolBoxFactory::instance();
    int i = 1; //account for the choose Filter item
    foreach(QString toolboxName, tbFactory->toolBoxesFromCategory("segmentation"))
    {
        medToolBoxDetails* details = tbFactory->toolBoxDetailsFromId(toolboxName);
        d->chooseSegmentationComboBox->addItem(details->name, toolboxName);
        d->chooseSegmentationComboBox->setItemData(i,
                                                   details->description,
                                                   Qt::ToolTipRole);
        i++;
    }

    connect(d->chooseSegmentationComboBox, SIGNAL(activated(int)), this, SLOT(changeCurrentToolBox(int)));

    QWidget *mainWidget = new QWidget;
    d->mainLayout = new QVBoxLayout;

    d->chooseSegmentationComboBox->adjustSize();
    d->mainLayout->addWidget(d->chooseSegmentationComboBox);
    mainWidget->setLayout(d->mainLayout);
    this->addWidget(mainWidget);
    this->setTitle("Segmentation");
}

medSegmentationSelectorToolBox::~medSegmentationSelectorToolBox(void)
{
    delete d;
    d = NULL;
}

medSegmentationAbstractToolBox* medSegmentationSelectorToolBox::currentToolBox()
{
    return d->currentSegmentationToolBox;
}

void medSegmentationSelectorToolBox::setWorkspace(medAbstractWorkspace* workspace)
{
    d->workspace = workspace;
}

void medSegmentationSelectorToolBox::changeCurrentToolBox(int index)
{
    medSegmentationAbstractToolBox *toolbox = NULL;
    //get identifier for toolbox.
    QString identifier = d->chooseSegmentationComboBox->itemData(index).toString();
    if (d->segmentationToolBoxes.contains (identifier))
        toolbox = d->segmentationToolBoxes[identifier];
    else
    {
        medToolBox* tb = medToolBoxFactory::instance()->createToolBox(identifier, this);
        toolbox = qobject_cast<medSegmentationAbstractToolBox*>(tb);
        if (toolbox)
        {
            if (d->workspace)
                toolbox->setWorkspace(d->workspace);
            toolbox->setStyleSheet("medToolBoxBody {border:none}");
            d->segmentationToolBoxes[identifier] = toolbox;
            connect(toolbox, SIGNAL(installEventFilterRequest(medViewEventFilter*)),
                    this, SIGNAL(installEventFilterRequest(medViewEventFilter*)),
                    Qt::UniqueConnection);
        }
    }

    if(d->currentSegmentationToolBox)
    {
        d->currentSegmentationToolBox->hide();
        d->mainLayout->removeWidget(d->currentSegmentationToolBox);
        d->currentSegmentationToolBox = NULL;
    }

    if(!toolbox)
    {
        this->setAboutPluginVisibility(false);
        return;
    }


    d->currentSegmentationToolBox = toolbox;
    d->currentSegmentationToolBox->header()->hide();

    dtkPlugin* plugin = d->currentSegmentationToolBox->plugin();
    this->setAboutPluginButton(plugin);
    this->setAboutPluginVisibility(true);

    d->currentSegmentationToolBox->show();
    d->mainLayout->addWidget(d->currentSegmentationToolBox);

    connect ( d->currentSegmentationToolBox, SIGNAL(success()), this, SIGNAL(success()));
}
