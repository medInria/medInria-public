#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>
#include <medWidgetsExport.h>

class QWidget;
class QTreeView;
class QSortFilterProxyModel;
class medSourceItemModel;
class medSourceView;
class medSourceItemModelPresenterPrivate;

class MEDWIDGETS_EXPORT medSourceItemModelPresenter : public QObject
{
    Q_OBJECT
public:
    medSourceItemModelPresenter(medSourceItemModel *parent = nullptr);
    virtual ~medSourceItemModelPresenter();

    virtual QWidget* buildWidget();

    virtual QTreeView* buildTree(QSortFilterProxyModel *proxy = nullptr);

    virtual medSourceItemModel *sourceModel() const;


private:
    const QScopedPointer<medSourceItemModelPresenterPrivate> d;
};
