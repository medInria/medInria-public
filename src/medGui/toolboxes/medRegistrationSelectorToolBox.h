/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medToolBox.h"
#include "medGuiExport.h"

class dtkAbstractView;
class dtkAbstractProcess;
class medAbstractDataImage;
class medDataIndex;
class medRegistrationSelectorToolBoxPrivate;

class MEDGUI_EXPORT medRegistrationSelectorToolBox : public medToolBox
{
    Q_OBJECT

public:
     medRegistrationSelectorToolBox(QWidget *parent = 0);
    ~medRegistrationSelectorToolBox();

    dtkAbstractView *fixedView();
    dtkAbstractView *movingView();

    medAbstractDataImage *fixedData();
    medAbstractDataImage *movingData();

    void setFuseView(dtkAbstractView *view);
    dtkAbstractView *fuseView();

    dtkAbstractProcess * process();
    void setProcess(dtkAbstractProcess* process);

    dtkAbstractProcess * undoRedoProcess();
    void setUndoRedoProcess(dtkAbstractProcess *proc);

signals:
    void setupLayoutCompare();
    void setupLayoutFuse();
    void showError (const QString&,unsigned int timeout);
    void showInfo(const QString&,unsigned int timeout);

public slots:
    void onMovingImageDropped(const medDataIndex& index);
    void onFixedImageDropped(const medDataIndex& index);

//    void onBlendModeSet(bool value);
//    void onCheckerboardModeSet(bool value);

    void onToolBoxChosen(int index);
    void clear();
    void onSaveImage();
    void onSaveTrans();
    void onSuccess();
    void onUndoRedo(void);

    void synchroniseWindowLevel(QObject * sender = NULL);
    void synchronisePosition(const QVector3D &position);

private:
    medRegistrationSelectorToolBoxPrivate *d;
};


