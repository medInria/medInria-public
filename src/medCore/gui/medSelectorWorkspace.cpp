/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSelectorToolBox.h>
#include <medSelectorWorkspace.h>

class medSelectorWorkspacePrivate
{
public:
    // Give values to items without a constructor.
    medSelectorWorkspacePrivate() :
       selectorToolBox(NULL)
    {}

    medSelectorToolBox* selectorToolBox;
};


medSelectorWorkspace::medSelectorWorkspace(QWidget * parent, QString name) : medAbstractWorkspace(parent), d(new medSelectorWorkspacePrivate)
{
    d->selectorToolBox = new medSelectorToolBox(parent, name);

    connect(selectorToolBox(),SIGNAL(success()),this,SLOT(onProcessSuccess()));

    this->addToolBox(selectorToolBox());
    selectorToolBox()->setTitle(name); // get workspace name
}

medSelectorWorkspace::medSelectorWorkspace(QWidget * parent, QString name, medSelectorToolBox* toolbox) : medAbstractWorkspace(parent), d(new medSelectorWorkspacePrivate)
{
    d->selectorToolBox = toolbox;

    connect(toolbox,SIGNAL(success()),this,SLOT(onProcessSuccess()));

    this->addToolBox(toolbox);
    toolbox->setTitle(name); // get workspace name
}

medSelectorWorkspace::~medSelectorWorkspace(void)
{
    delete d;
    d = NULL;
}

medSelectorToolBox* medSelectorWorkspace::selectorToolBox()
{
    return d->selectorToolBox;
}

 void medSelectorWorkspace::setSelectorToolBox(medSelectorToolBox* toolbox)
{
    d->selectorToolBox = toolbox;
}
