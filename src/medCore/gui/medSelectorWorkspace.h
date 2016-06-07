/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractWorkspace.h>

class medSelectorWorkspacePrivate;
class medSelectorToolBox;

/**
 * class medSelectorWorkspace
 * Defines the Selector workspace.
 */
class medSelectorWorkspace : public medAbstractWorkspace
{
    Q_OBJECT

public:
    medSelectorWorkspace(QWidget * parent, QString name);

    virtual ~medSelectorWorkspace();

    QPointer<medSelectorToolBox> selectorToolBox();

protected slots:
    virtual void onProcessSuccess(){}

private:
    medSelectorWorkspacePrivate *d;
};


