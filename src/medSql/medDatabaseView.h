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

#include "medSqlExport.h"

#include <QtGui>

class medDataIndex;
class medDatabaseModel;

class MEDSQL_EXPORT medDatabaseView : public QTreeView
{
    Q_OBJECT

public:
     medDatabaseView(QWidget *parent = 0);
    ~medDatabaseView(void);

    int sizeHintForColumn(int column) const;

    void setModel(medDatabaseModel *model);

signals:
    void patientClicked(int id);
    void   studyClicked(int id);
    void  seriesClicked(int id);
    void   imageClicked(int id);

    void open(const medDataIndex&);
  
public slots:
    void onPatientClicked(int id);
    void onStudyClicked(int id);
    void onSeriesClicked(int id);
    void onImageClicked(int id);
    void onMenuViewClicked(void);

protected slots:
    void updateContextMenu(const QPoint&);

private slots:
    void onItemClicked(const QModelIndex& index);
    void onItemDoubleClicked(const QModelIndex& index);
};

#endif
