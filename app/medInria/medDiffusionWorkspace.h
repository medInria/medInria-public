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
#include <dtkCore/dtkAbstractViewInteractor.h>

class dtkAbstractProcess;
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

signals:
    void newOutput(dtkAbstractData *);
    
public slots:

    void runProcess(dtkAbstractProcess *process, QString category);
    void getOutput();
    void resetRunningFlags();
    
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



