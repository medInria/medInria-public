#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include <QTreeView>

class medSourcesItemDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;
//    explicit medSourcesItemDelegate(QAbstractItemModel *model);
    explicit medSourcesItemDelegate(QTreeView *treeView);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
//    QSize sizeHint(const QStyleOptionViewItem &option,
//                   const QModelIndex &index) const override;
//    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
//                          const QModelIndex &index) const override;
//    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
//    void setModelData(QWidget *editor, QAbstractItemModel *model,
//                      const QModelIndex &index) const override;

private:
    QModelIndex getIndex(const QModelIndex &index) const;
    QMovie *m_mov;
    QPixmap m_currentPixmap;
    QSortFilterProxyModel *m_proxy;
    QTreeView *m_treeView;
};

