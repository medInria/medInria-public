/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medRegistrationAbstractToolBox.h>
#include <undoRedoRegistration.h>
#include <undoRedoRegistrationPluginExport.h>

class undoRedoRegistrationToolBoxPrivate;

class UNDOREDOREGISTRATIONPLUGIN_EXPORT undoRedoRegistrationToolBox : public medRegistrationAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Stack of transformations",
                          "Undo-Redo Registration toolbox.",
                          << "UndoRedoRegistration")
public:
    undoRedoRegistrationToolBox(QWidget *parent = nullptr);
    ~undoRedoRegistrationToolBox(void);
    
    static bool registered();
    void updatePositionArrow(int);
    undoRedoRegistration::RegImageType::Pointer output();
    
    virtual void setRegistrationToolBox(medRegistrationSelectorToolBox *toolbox);

public slots:
    void onUndo();
    void onRedo();
    void onTransformationStackReset();
    void addTransformationIntoList(int, QString);
    void onRegistrationSuccess();

private:
    undoRedoRegistrationToolBoxPrivate *d;
};


