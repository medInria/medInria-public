/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>
#include <medClutEditor.h>
#include <medAbstractView.h>
#include <medToolBox.h>

#include <QtGui>

class medAbstractData;
class medAbstractImageView;

// /////////////////////////////////////////////////////////////////
// medClutEditorToolBox
// /////////////////////////////////////////////////////////////////

class medClutEditorToolBoxPrivate;

class MEDVTKINRIA_EXPORT medClutEditorToolBox : public medToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("medClutEditorToolBox","Displays an interactive histogram",<<"")

public:
     medClutEditorToolBox(QWidget *parent = 0);
    ~medClutEditorToolBox();

    static bool registered();
    void setData(medAbstractData *data);
    void setView(medAbstractView *view);
    void applyTable(medAbstractView *view);
    void setColorLookupTable (medAbstractView* view, QList<double> scalars, QList<QColor> colors );
	void clear();
    void getTransferFunctions ( QList<double> & scalars, QList<QColor> & colors );
    void forceLayer(int layer);
    medClutEditorScene * getScene();
    void addVertex(QPointF value, QColor color);
    void reset();
    void deleteTable();

public slots:
    void setDiscreteMode(bool);
    void showInfo();

protected:
    void initializeTable();
    

    void mousePressEvent(QMouseEvent *event);

protected slots:
    void onNewTableAction();
    void onLoadTableAction();
    void onSaveTableAction();
    // void onDeleteTableAction();
    void onApplyTablesAction();
    // void onColorAction();
    // void onDeleteAction();
    void onVertexMoved();
    void onToggleDirectUpdateAction();
private:
    medClutEditorToolBoxPrivate *d;
};


