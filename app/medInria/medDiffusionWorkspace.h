/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once


#include <QtCore>

#include <medWorkspace.h>

class dtkAbstractProcess;
class dtkAbstractView;
class medTabbedViewContainers;
class medDiffusionWorkspacePrivate;

class medDiffusionWorkspace : public medWorkspace
{
    Q_OBJECT

public:
     medDiffusionWorkspace(QWidget *parent = 0);
    ~medDiffusionWorkspace();

    virtual QString identifier()  const;
    virtual QString description() const;
    static bool isUsable();
    void setupViewContainerStack();
    
public slots:

    void runProcess(dtkAbstractProcess *process, QString category);
    void getOutput();
    void cancelProcess();
    void resetRunningFlags();
    
    //Handle new data in central view, connect them to toolboxes
    void resetToolBoxesInputs(dtkAbstractView *view);
    void addToolBoxInput(dtkAbstractData *data);
    
    void connectCurrentViewSignals(dtkAbstractView *view);
    void disconnectCurrentViewSignals(dtkAbstractView *view);
    
    /**
      * @brief Adds a new tab to a workspace
      *
      * Re-implemented, replaces default implementation in medWorkspace
      */
    void onAddTabClicked();
    
    void changeCurrentContainer(QString name);

private:
    medDiffusionWorkspacePrivate *d;
};



