/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <QtGui>

class medDataIndex;
class medDatabaseModel;
class medDatabaseViewPrivate;
class medAbstractDatabaseItem;

class MEDCORE_EXPORT medDatabaseView : public QTreeView
{
    Q_OBJECT

public:
     medDatabaseView(QWidget *parent = 0);
    ~medDatabaseView();

    void setModel(QAbstractItemModel *model);
    int sizeHintForColumn(int column) const;

signals:

    /** Signal emitted when user clicks on a patient item. */
    void patientClicked(const medDataIndex &index);

    /** Signal emitted when user clicks on a study item. */
    void studyClicked(const medDataIndex &index);
    
    /** Signal emitted when user clicks on a series item. */
    void seriesClicked(const medDataIndex &index);

    /** Signal emitted when user double-clicks on a patient item. */
    void patientDoubleClicked(const medDataIndex &index);

    /** Signal emitted when user double-clicks on a series item. */
    void seriesDoubleClicked(const medDataIndex &index);

    /** Signal emitted when the selection changes and no item is selected. */
    void noPatientOrSeriesSelected();

    /** Signal emitted when multiple entries are selected **/
    void multipleEntriesSelected(const QVector<medDataIndex>& selectedIndexes);

    void open(const medDataIndex&);
    void exportData(const medDataIndex &index);
    void dataRemoved(const medDataIndex &index);

public slots:

    virtual void onViewSelectedItemRequested();
    virtual void onExportSelectedItemRequested();
    virtual void onRemoveSelectedItemRequested();
    virtual void onSaveSelectedItemRequested();
    void onOpeningFailed(const medDataIndex& index);
    void onCreatePatientRequested();
    void onCreateStudyRequested();
    void onEditRequested();

protected slots:
    virtual void updateContextMenu(const QPoint&);
    virtual void onItemDoubleClicked(const QModelIndex& index);
    virtual void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

protected:
    medAbstractDatabaseItem* getItemFromIndex(const QModelIndex& selected);

private:
    medDatabaseViewPrivate *d;
};


