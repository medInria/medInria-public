/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui>
#include <DiffusionSequenceDelegate.h>

class DiffusionSequenceWidget: public QTableWidget {
    Q_OBJECT

public:

    typedef DiffusionSequenceDelegate Vector3DDelegate;

    DiffusionSequenceWidget(QWidget* parent=0): QTableWidget(parent) {
        clear();
        setAcceptDrops(true);
        setColumnCount(2);
        setColumnWidth(0,120);
        //setColumnWidth(1,120);
        setShowGrid(false);
        setSelectionMode(QAbstractItemView::NoSelection);
        horizontalHeader()->setStretchLastSection(true);
        
        setItemDelegate(new Vector3DDelegate(this));

        QStringList headers;
        headers << tr("Image") << tr("Gradient");
        setHorizontalHeaderLabels(headers);
        //setAlternatingRowColors(true);
        setContextMenuPolicy(Qt::CustomContextMenu);
    }

    void clear() { setRowCount(0); }

    void dragEnterEvent(QDragEnterEvent* event) { event->acceptProposedAction(); }
    void dragMoveEvent(QDragMoveEvent* event)   { event->acceptProposedAction(); }
    void dragLeaveEvent(QDragLeaveEvent* event) { event->accept();               }
    void dropEvent(QDropEvent* event) {
        const QMimeData* mimeData = event->mimeData();
        if (mimeData->hasUrls()) {
            const QList<QUrl>& urls = mimeData->urls();
            for (QList<QUrl>::ConstIterator i        = urls.begin();i!=urls.end();++i) {
                const QString&              filename = i->toLocalFile();
                bool valid = false;
                emit newItem(filename,valid);
                if (valid) {
                    const QFileInfo& info = QFileInfo(filename);
                    const unsigned nrows  = rowCount();
                    setRowCount(nrows+1);
                    setRowHeight(nrows,15);
                    QTableWidgetItem* item0 = new QTableWidgetItem(info.baseName());
                    QTableWidgetItem* item1 = new QTableWidgetItem("0, 0, 0");
                    item0->setData(Qt::UserRole,QVariant(filename));
                    setItem(nrows,0,item0);
                    setItem(nrows,1,item1);
                }
            }
        }
    }

    void contextMenuEvent(QContextMenuEvent* event) {
        QMenu menu(this);
        menu.addAction(tr("Remove"),this,SLOT(onRemoveClicked()));
        menu.exec(event->globalPos());
    }

signals:

    void newItem(const QString& item,bool& valid);

public slots:

    void onRemoveClicked() {
        if (!selectedIndexes().count())
            return;
        const QModelIndex& index = selectedIndexes().first();
        if (!index.isValid())
            return;
        removeRow(index.row());
    }
};


