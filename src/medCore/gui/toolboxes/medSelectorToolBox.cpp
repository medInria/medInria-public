/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractToolBox.h>
#include <medSelectorToolBox.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medToolBoxTab.h>

class medSelectorToolBoxPrivate
{
public:
    medComboBox *chooseComboBox;
    medAbstractToolBox * currentToolBox;
    QHash<QString, medAbstractToolBox*> toolBoxes;
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
    d->chooseComboBox->setToolTip(tr("Browse through the list of available algorithm"));

    medToolBoxFactory* tbFactory = medToolBoxFactory::instance();
    int i = 1; //account for the choose Filter item
    foreach(QString toolboxName, tbFactory->toolBoxesFromCategory(tlbxId))
    {
        medToolBoxDetails* details = tbFactory->toolBoxDetailsFromId(toolboxName);

        d->chooseComboBox->addItem(details->name, toolboxName);
        d->chooseComboBox->setItemData(i,
                                                   details->description,
                                                   Qt::ToolTipRole);
        i++;
    }

    connect(d->chooseComboBox, SIGNAL(activated(int)), this, SLOT(changeCurrentToolBox(int)));

    QWidget *mainWidget = new QWidget;
    d->mainLayout = new QVBoxLayout;

    d->chooseComboBox->adjustSize();
    d->mainLayout->addWidget(d->chooseComboBox);
    mainWidget->setLayout(d->mainLayout);

    this->addWidget(mainWidget);
    this->setTitle("");
}

medSelectorToolBox::~medSelectorToolBox(void)
{
    delete d;
    d = NULL;
}

/**
 * @brief returns current selected toolbox
 */
medAbstractToolBox* medSelectorToolBox::currentToolBox()
{
    return d->currentToolBox;
}

/**
 * @brief instantiates the right process toolbox according to its description
 */
void medSelectorToolBox::changeCurrentToolBox ( int index )
{
    // Get current toolbox identifier from combobox
    QString identifier = d->chooseComboBox->itemData(index).toString();
    this->changeCurrentToolBox(identifier);
}

void medSelectorToolBox::changeCurrentToolBox(const QString &identifier)
{
    medAbstractToolBox *toolbox = NULL;

    if (d->toolBoxes.contains (identifier))
    {
        toolbox = d->toolBoxes[identifier];
    }
    else
    {
        medToolBox* tb = medToolBoxFactory::instance()->createToolBox(identifier, this);
        toolbox = qobject_cast<medAbstractToolBox*>(tb);
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

        emit currentToolBoxChanged();
    }
    else
    {
        this->setAboutPluginVisibility(false);
    }
}

/**
 * @brief returns input data
 */
medAbstractData* medSelectorToolBox::data()
{
    return d->inputData;
}

/**
 * @brief retrieve data from the selected input image index
 * @param index
 */
void medSelectorToolBox::onInputSelected(medAbstractData *data)
{
    d->inputData = data;

    if(d->currentToolBox)
    {
        d->currentToolBox->updateType(d->inputData);
    }
}

/**
 * @brief clear input data and the current process toolbox
 */
void medSelectorToolBox::clear()
{
    d->inputData = NULL;

    if(d->currentToolBox)
    {
        d->currentToolBox->updateType(d->inputData);
    }
}
