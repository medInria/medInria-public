/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSegmentationAbstractToolBox.h>
#include <medSegmentationSelectorToolBox.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>

class medSegmentationSelectorToolBoxPrivate
{
public:
    QComboBox *chooseComboBox;
    medSegmentationAbstractToolBox * currentToolBox;
    QHash<QString, medSegmentationAbstractToolBox*> toolBoxes;
    QVBoxLayout *mainLayout;

    medAbstractData *inputData;
};

medSegmentationSelectorToolBox::medSegmentationSelectorToolBox(QWidget *parent) :
    medToolBox(parent),
    d(new medSegmentationSelectorToolBoxPrivate)
{
    d->currentToolBox = nullptr;

    d->chooseComboBox = new QComboBox;
    d->chooseComboBox->addItem("* Choose a toolbox *");
    d->chooseComboBox->setToolTip(tr("Choose a toolbox in the list"));
    d->chooseComboBox->setItemData(0, "Choose a toolbox", Qt::ToolTipRole);

    medToolBoxFactory* tbFactory = medToolBoxFactory::instance();

    // Get informations about the workspace toolboxes:
    // displayed name, description for tooltip, and identifier
    QHash<QString, QStringList> toolboxDataHash;
    foreach(QString toolboxName, tbFactory->toolBoxesFromCategory("segmentation"))
    {
        medToolBoxDetails* details = tbFactory->toolBoxDetailsFromId(toolboxName);

        QStringList current;
        current.append(toolboxName);
        current.append(details->description);
        toolboxDataHash[details->name] = current;
    }

    // Sort toolboxes names alphabetically
    QList<QString> names = toolboxDataHash.keys();
    qSort(names.begin(), names.end());

    int i = 1; // toolboxes positions
    foreach( QString name, names )
    {
        QStringList values = toolboxDataHash.value(name);
        d->chooseComboBox->addItem(name, values.at(0));
        d->chooseComboBox->setItemData(i, values.at(1), Qt::ToolTipRole);
        i++;
    }

    connect(d->chooseComboBox, SIGNAL(activated(int)), this, SLOT(changeCurrentToolBox(int)));

    QWidget *mainWidget = new QWidget;
    d->mainLayout = new QVBoxLayout;

    d->chooseComboBox->adjustSize();
    d->mainLayout->addWidget(d->chooseComboBox);
    mainWidget->setLayout(d->mainLayout);

    this->addWidget(mainWidget);

    this->setTitle("Segmentation");
}

medSegmentationSelectorToolBox::~medSegmentationSelectorToolBox(void)
{
    delete d;
    d = nullptr;
}

medSegmentationAbstractToolBox* medSegmentationSelectorToolBox::currentToolBox()
{
    return d->currentToolBox;
}

void medSegmentationSelectorToolBox::changeCurrentToolBox(int index)
{
    // Get current toolbox identifier from combobox
    QString identifier = d->chooseComboBox->itemData(index).toString();
    this->changeCurrentToolBox(identifier);
}

void medSegmentationSelectorToolBox::changeCurrentToolBox(const QString &identifier)
{
    medSegmentationAbstractToolBox *toolbox = nullptr;

    if (d->toolBoxes.contains(identifier))
    {
        toolbox = d->toolBoxes[identifier];
    }
    else
    {
        medToolBox* tb = medToolBoxFactory::instance()->createToolBox(identifier, this);
        toolbox = qobject_cast<medSegmentationAbstractToolBox*>(tb);
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
        d->currentToolBox->clear();

        d->mainLayout->removeWidget(d->currentToolBox);
        d->currentToolBox = nullptr;
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

        connect(d->currentToolBox, SIGNAL(success()), this, SIGNAL(success()), Qt::UniqueConnection);
        connect(d->currentToolBox, SIGNAL(failure()), this, SIGNAL(failure()), Qt::UniqueConnection);

        emit currentToolBoxChanged();
    }
    else
    {
        this->setAboutPluginVisibility(false);
    }
}

int medSegmentationSelectorToolBox::getIndexOfToolBox(const QString &toolboxName)
{
    for (int i=0; i<d->chooseComboBox->count(); ++i)
    {
        if(d->chooseComboBox->itemText(i) == toolboxName)
        {
            return i;
        }
    }
    return -1;
}

medAbstractData* medSegmentationSelectorToolBox::data()
{
    return d->inputData;
}


QComboBox* medSegmentationSelectorToolBox::comboBox()
{
    return d->chooseComboBox;
}

void medSegmentationSelectorToolBox::onInputSelected(medAbstractData *data)
{
    d->inputData = data;
    emit inputChanged();
}

void medSegmentationSelectorToolBox::clear()
{
    d->inputData = nullptr;
    emit inputChanged();
}
