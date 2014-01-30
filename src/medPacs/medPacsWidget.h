/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui/QTreeWidget>

#include <medPacsExport.h>
#include <medMoveCommandItem.h>

class medPacsWidgetPrivate;

class MEDPACS_EXPORT medPacsWidget : public QTreeWidget
{
    Q_OBJECT

public:
     medPacsWidget(QWidget* parent = NULL);
    ~medPacsWidget();
    
    /*
     * Check if the server was correctly instantiated at construction. If not, this means that the pacs plugin doesn't exist or had problems being loaded
     * @return Boolean telling if the server is instantiated
     */
    bool isServerFunctional();

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
    void onItemImported();
    void updateContextMenu(const QPoint&);

protected:

  void readSettings();
  void findSeriesLevel(QTreeWidgetItem* item);
  void findImageLevel(QTreeWidgetItem* item);

private:
    medPacsWidgetPrivate *d;
};

int  tryToInt(QString value);



