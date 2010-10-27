/* medPacsWidget.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 11:07:02 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 15:52:13 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 31
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

signals:
    void move(int, int, QString, QString, int);
    void import(QString);

public slots:
    void search(QString query);

protected slots:
    void onItemExpanded(QTreeWidgetItem *);
    void onItemImported(void);
    void updateContextMenu(const QPoint&);

protected:
  void findSeriesLevel(QTreeWidgetItem* item);
  void findImageLevel(QTreeWidgetItem* item);

private:
    medPacsWidgetPrivate *d;
};

#endif
