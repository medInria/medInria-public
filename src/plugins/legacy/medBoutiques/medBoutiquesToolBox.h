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
    // Check the "installed" setting: perform install checks if false, ignore install otherwise
    void checkBoutiquesInstallation();

    // Perform installation check: verify that python and docker can run properly and warn the user otherwise
    // Set the "installed" setting to "true" if everything works fine
    void installBoutiques(QJsonObject *settings = nullptr);

    // Set the "installed" setting to "true"
    void setBoutiquesInstalled(QJsonObject *settings);

    // Check that the python command works ("python" on Linux and OS X, "$EXEC_PATH/BoutiquesGUI-DATA/python/python.exe" on Windows)
    bool isPythonWorking(const QString &version = "");

    // Check that the docker command works "docker"
    bool isDockerWorking();

private:
     medBoutiquesToolBoxPrivate *d;
};
