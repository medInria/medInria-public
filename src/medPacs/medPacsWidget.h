/* medPacsWidget.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 11:07:02 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 11:40:06 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 25
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDPACSWIDGET_H
#define MEDPACSWIDGET_H

#include <QtGui/QTreeWidget>

#include "medPacsExport.h"

class medPacsWidgetPrivate;

class MEDPACS_EXPORT medPacsWidget : public QTreeWidget
{
    Q_OBJECT

public:
     medPacsWidget(QWidget* parent);
    ~medPacsWidget(void);

    void readSettings(void);
    void writeSettings(void);

public slots:
    void search(QString query);

protected slots:
    void onItemClicked(QTreeWidgetItem *, int);

protected:
  void findSeriesLevel(QTreeWidgetItem* item);
  void findImageLevel(QTreeWidgetItem* item);

private:
    medPacsWidgetPrivate *d;
};

#endif
