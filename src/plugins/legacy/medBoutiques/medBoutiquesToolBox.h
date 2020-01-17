/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medFilteringAbstractToolBox.h>
#include <medDataIndex.h>
#include "boutiquesGUI/medBoutiquesSearchToolsWidget.h"
#include "boutiquesGUI/medBoutiquesInvocationWidget.h"
#include "boutiquesGUI/medBoutiquesInvocationGUIWidget.h"
#include "boutiquesGUI/medBoutiquesExecutionWidget.h"

class medBoutiquesToolBoxPrivate;

class medBoutiquesToolBox : public medFilteringAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("medBoutiques", "Boutiques in medInria", << "filtering")

public:
     medBoutiquesToolBox(QWidget *parentToolBox = nullptr);
    ~medBoutiquesToolBox();

    static bool registered();
    medAbstractData* processOutput();
    medAbstractData* getInput();
    dtkPlugin* plugin();
    void update(medAbstractData *data);

signals:
    void success();
    void failure();

 public slots:
     void executionSuccess(const QString &outputFileName);
     void open_waitForImportedSignal(medDataIndex index, QUuid uuid);


private:
     medBoutiquesToolBoxPrivate *d;
};
