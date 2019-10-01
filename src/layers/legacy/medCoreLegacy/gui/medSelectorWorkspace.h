/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractWorkspaceLegacy.h>
#include <medCoreLegacyExport.h>

class medSelectorWorkspacePrivate;
class medSelectorToolBox;

/**
 * class medSelectorWorkspace
 * Defines the Selector workspace which has a tool to select toolboxes.
 */
class MEDCORELEGACY_EXPORT medSelectorWorkspace : public medAbstractWorkspaceLegacy
{
    Q_OBJECT

public:
    medSelectorWorkspace(QWidget *parent, QString name, medSelectorToolBox *selectorToolbox = nullptr);

    ~medSelectorWorkspace() override;

    medSelectorToolBox *selectorToolBox();

protected slots:
    virtual void importProcessOutput();

private:
    medSelectorWorkspacePrivate *d;
};


