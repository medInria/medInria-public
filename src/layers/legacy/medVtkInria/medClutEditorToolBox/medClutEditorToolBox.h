/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractView.h>
#include <medClutEditor.h>
#include <medToolBox.h>
#include <medVtkInriaExport.h>

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
    MED_TOOLBOX_INTERFACE("medClutEditorToolBox",
                          "Display an interactive histogram",
                          << "")

public:
     medClutEditorToolBox(QWidget *parent = nullptr);
    ~medClutEditorToolBox();

    static bool registered();
    void setData(medAbstractData *data);
    void setView(medAbstractView *view);
    void applyTable(medAbstractView *view);
    void setColorLookupTable (medAbstractView* view, QList<double> scalars, QList<QColor> colors );
    void clear();
    void getTransferFunctions ( QList<double> &scalars, QList<QColor> &colors );
    void forceLayer(int layer);
    medClutEditorScene * getScene();
    void addVertex(QPointF value, QColor color);
    void reset();
    void deleteTable();
    void invertLUT(bool);
    void updateSavedTables();

public slots:
    void setDiscreteMode(bool);
    void showInfo();

protected:
    void initializeTable();
    
    void mousePressEvent(QMouseEvent *event);

protected slots:
    /**
     * @brief onNewTableAction delete every point on this histogram
     */
    void onNewTableAction();
    void onLoadTableAction();
    void onSaveTableAction();
    void onApplyTablesAction();
    void onVertexMoved();
    void onToggleDirectUpdateAction();

private:
    medClutEditorToolBoxPrivate *d;
};
