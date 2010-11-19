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
#include "medMoveCommandItem.h"

class medPacsWidgetPrivate;

class MEDPACS_EXPORT medPacsWidget : public QTreeWidget
{
    Q_OBJECT

public:
     medPacsWidget(QWidget* parent);
    ~medPacsWidget(void);

signals:
    void moveList(const QVector<medMoveCommandItem>&);
    void import(QString);
    void requestSelectedNodes();
    void echoResponse(QVector<bool>);

public slots:
    void search(QString query);
    void updateSelectedNodes(QVector<int> list);
    void onEchoRequest();

protected slots:
    void onItemExpanded(QTreeWidgetItem *);
    void onItemImported(void);
    void updateContextMenu(const QPoint&);

protected:

  void readSettings(void);
  void findSeriesLevel(QTreeWidgetItem* item);
  void findImageLevel(QTreeWidgetItem* item);

private:
    medPacsWidgetPrivate *d;
};

int  tryToInt(QString value);


#endif
