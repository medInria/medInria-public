/* medDatabaseView.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Mar 31 13:16:32 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Sat Oct  3 13:53:32 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 30
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASEVIEW_H
#define MEDDATABASEVIEW_H

#include <QtGui>

class medDatabaseModel;

class medDatabaseView : public QTreeView
{
    Q_OBJECT

public:
     medDatabaseView(QWidget *parent = 0);
    ~medDatabaseView(void);

    void setModel(medDatabaseModel *model);

signals:
    void patientClicked(int id);
    void   studyClicked(int id);
    void  seriesClicked(int id);
    void   imageClicked(int id);

    void patientDoubleClicked(int id);
    void   studyDoubleClicked(int id);
    void  seriesDoubleClicked(int id);
    void   imageDoubleClicked(int id);
    
private slots:
    void onItemClicked(const QModelIndex& index);
    void onItemDoubleClicked(const QModelIndex& index);
};

#endif
