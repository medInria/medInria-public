/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
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
    medAbstractSelectableToolBox *currentToolBox;
    QHash<QString, medAbstractSelectableToolBox*> toolBoxes;
    QVBoxLayout *mainLayout;
    medAbstractData *inputData;
};

medSelectorToolBox::medSelectorToolBox(QWidget *parent, QString tlbxId)
    : medToolBox(parent), d(new medSelectorToolBoxPrivate)
{
    d->currentToolBox = nullptr;

    d->chooseComboBox = new medComboBox;
    d->chooseComboBox->addItem("* Choose a toolbox *");
    d->chooseComboBox->setToolTip(tr("Choose a toolbox in the list"));
    d->chooseComboBox->setItemData(0, "Choose a toolbox", Qt::ToolTipRole);

    medToolBoxFactory *tbFactory = medToolBoxFactory::instance();

    // Get informations about the workspace toolboxes:
    // displayed name, description for tooltip, and identifier
    QHash<QString, QStringList> toolboxDataHash;
    for(QString toolboxName : tbFactory->toolBoxesFromCategory(tlbxId))
    {
        medToolBoxDetails *details = tbFactory->toolBoxDetailsFromId(toolboxName);

        QStringList current;
        current.append(toolboxName);
        current.append(details->description);
        toolboxDataHash[details->name] = current;
    }

    // Sort toolboxes names alphabetically
    QList<QString> names = toolboxDataHash.keys();
    std::sort(names.begin(), names.end());

    int i = 1; // toolboxes positions
    for( QString name : names )
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

    clear();
}

medSelectorToolBox::~medSelectorToolBox(void)
{
    delete d;
    d = nullptr;
}

medAbstractSelectableToolBox* medSelectorToolBox::currentToolBox()
{
    return d->currentToolBox;
}

void medSelectorToolBox::changeCurrentToolBox(int index)
{
    // Get current toolbox identifier from combobox
    QString identifier = d->chooseComboBox->itemData(index).toString();

    if (!d->currentToolBox || identifier != d->currentToolBox->identifier())
    {
        this->changeCurrentToolBox(identifier);
    }
}

void medSelectorToolBox::changeCurrentToolBox(const QString &identifier)
{
    medAbstractSelectableToolBox *toolbox = nullptr;

    if (d->toolBoxes.contains (identifier))
    {
        toolbox = d->toolBoxes[identifier];
    }
    else
    {
        medToolBox *tb = medToolBoxFactory::instance()->createToolBox(identifier, this);
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
        d->currentToolBox->clear();

        d->mainLayout->removeWidget(d->currentToolBox);
        d->currentToolBox = nullptr;
    }

    if(toolbox)
    {
        d->currentToolBox = toolbox;
        d->currentToolBox->header()->hide();

        dtkPlugin *plugin = d->currentToolBox->plugin();

        if (plugin)
        {
            this->setAboutPluginButton(plugin);
            this->setAboutPluginVisibility(true);
        }

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

int medSelectorToolBox::getIndexOfToolBox(const QString &toolboxName)
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

medAbstractData* medSelectorToolBox::data()
{
    return d->inputData;
}

medComboBox *medSelectorToolBox::comboBox()
{
    return d->chooseComboBox;
}

void medSelectorToolBox::onInputSelected(medAbstractData *data)
{
    d->inputData = data;
    emit inputChanged();
}

void medSelectorToolBox::clear()
{
    d->inputData = nullptr;
    emit inputChanged();
}
