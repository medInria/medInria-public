/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>

class medUndoRedoToolBoxPrivate;

class MEDCORE_EXPORT medUndoRedoToolBox : public medToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("undoRedo",
                          "short tooltip description",
                          << "UndoRedo")
public:
    medUndoRedoToolBox(QWidget *parent = 0);
    ~medUndoRedoToolBox(void);
    
public:
    static bool registered(void);
    //undoRedo::RegImageType::Pointer output(void);
    
public slots:
    void onUndo(void);
    void onRedo(void);
    void onReset(void);
    void onSuccess();
    void addCommand(QString methodParameters);

protected:
    void updatePositionArrow(int);

signals:
    void undoRequested();
    void redoRequested();
    void resetRequested();

private:
    medUndoRedoToolBoxPrivate *d;
};


