/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSelectableToolBox.h>
#include <medDataManager.h>
#include <medSelectorToolBox.h>
#include <medSelectorWorkspace.h>

class medSelectorWorkspacePrivate
{
public:
    // Give values to items without a constructor.
    medSelectorWorkspacePrivate() :
       selectorToolBox(nullptr)
    {}

    QPointer<medSelectorToolBox> selectorToolBox;
};

medSelectorWorkspace::medSelectorWorkspace(QWidget *parent, QString name, medSelectorToolBox *toolbox)
    : medAbstractWorkspaceLegacy(parent), d(new medSelectorWorkspacePrivate)
{
    if(!toolbox)
    {
        d->selectorToolBox = new medSelectorToolBox(parent, name);
    }
    else
    {
        d->selectorToolBox = toolbox;
    }

    this->addToolBox(d->selectorToolBox);
    d->selectorToolBox->setTitle(name); // get workspace name
}

medSelectorWorkspace::~medSelectorWorkspace(void)
{
    delete d;
    d = nullptr;
}

medSelectorToolBox* medSelectorWorkspace::selectorToolBox()
{
    return d->selectorToolBox;
}

void medSelectorWorkspace::importProcessOutput()
{
    medAbstractData *output = selectorToolBox()->currentToolBox()->processOutput();
    medDataManager::instance().importData(output);
}
