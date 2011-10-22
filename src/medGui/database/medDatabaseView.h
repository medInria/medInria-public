/* medDatabaseView.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Mar 31 13:16:32 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Jun 14 13:37:53 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 42
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDDATABASEVIEW_H
#define MEDDATABASEVIEW_H

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
    ~medDatabaseView(void);

    virtual int sizeHintForColumn(int column) const;

    void setModel(QAbstractItemModel *model);

signals:
    void patientClicked(const medDataIndex &index);
    void studyClicked(const medDataIndex &index);
    void seriesClicked(const medDataIndex &index);

    void patientDoubleClicked(const medDataIndex &index);
    void seriesDoubleClicked(const medDataIndex &index);

    void open(const medDataIndex&);
    void exportData(const medDataIndex &index);
    void dataRemoved(const medDataIndex &index);

public slots:
    virtual void onMenuViewClicked(void);
    virtual void onMenuExportClicked(void);
    virtual void onMenuRemoveClicked(void);
    virtual void onMenuSaveClicked(void);
    virtual void selectionChanged(const QModelIndex&, const QModelIndex&);
    void onOpeningFailed(const medDataIndex& index);

protected slots:
    virtual void updateContextMenu(const QPoint&);
    virtual void onItemClicked(const QModelIndex& index);
    virtual void onItemDoubleClicked(const QModelIndex& index);

private:
    medDatabaseViewPrivate *d;
};

#endif
