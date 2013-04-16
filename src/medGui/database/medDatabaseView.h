/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medGuiExport.h"

#include <QtGui>

class medDataIndex;
class medDatabaseModel;
class medDatabaseViewPrivate;

class MEDGUI_EXPORT medDatabaseView : public QTreeView
{
    Q_OBJECT

public:
     medDatabaseView(QWidget *parent = 0);
    ~medDatabaseView();

    virtual int sizeHintForColumn(int column) const;

    void setModel(QAbstractItemModel *model);

signals:

    /** Signal emitted when user clicks on a patient item. */
    void patientClicked(const medDataIndex &index);

    /** Signal emitted when user clicks on a series item. */
    void seriesClicked(const medDataIndex &index);

    /** Signal emitted when user double-clicks on a patient item. */
    void patientDoubleClicked(const medDataIndex &index);

    /** Signal emitted when user double-clicks on a series item. */
    void seriesDoubleClicked(const medDataIndex &index);

    /** Signal emitted when the selection changes and no item is selected. */
    void noPatientOrSeriesSelected();

    void open(const medDataIndex&);
    void exportData(const medDataIndex &index);
    void dataRemoved(const medDataIndex &index);

public slots:

    /** Opens the currently selected item. */
    virtual void onViewSelectedItemRequested();

    /** Exports the currently selected item. */
    virtual void onExportSelectedItemRequested();

    /** Removes the currently selected item. */
    virtual void onRemoveSelectedItemRequested();

    /** Saves the currently selected item. */
    virtual void onSaveSelectedItemRequested();

    /** Called after having failed to open a file. Will add a visual indicator of the failed file. */
    void onOpeningFailed(const medDataIndex& index);

protected slots:
    virtual void updateContextMenu(const QPoint&);
    virtual void onItemDoubleClicked(const QModelIndex& index);
    virtual void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
    medDatabaseViewPrivate *d;
};


