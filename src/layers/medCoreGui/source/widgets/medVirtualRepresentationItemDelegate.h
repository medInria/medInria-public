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

class medVirtualRepresentationItemDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    explicit medVirtualRepresentationItemDelegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

};

