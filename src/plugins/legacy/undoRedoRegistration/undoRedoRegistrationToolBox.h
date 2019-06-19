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
    MED_TOOLBOX_INTERFACE("undoRedoRegistration",
                          "short tooltip description",
                          << "UndoRedoRegistration")
public:
    undoRedoRegistrationToolBox(QWidget *parent = 0);
    ~undoRedoRegistrationToolBox(void);
    
    static bool registered(void);
    void updatePositionArrow(int);
    undoRedoRegistration::RegImageType::Pointer output(void);
    
    virtual void setRegistrationToolBox(medRegistrationSelectorToolBox *toolbox);
    dtkPlugin * plugin() override;

public slots:
    void onUndo(void);
    void onRedo(void);
    void onTransformationStackReset(void);
    void addTransformationIntoList(int, QString);
    void onRegistrationSuccess();

private:
    undoRedoRegistrationToolBoxPrivate *d;
};


