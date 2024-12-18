#include "medSourceModelPresenter.h"

#include <QTreeView>
#include <QSortFilterProxyModel>
#include <medSourceModel.h>
#include <medSourcesItemDelegate.h>

class medSourceModelPresenterPrivate
{
public:
    medSourceModel    *sourceItemModel;
};

medSourceModelPresenter::medSourceModelPresenter(medSourceModel *parent) : d( new medSourceModelPresenterPrivate())
{
    d->sourceItemModel = parent;
}

medSourceModelPresenter::~medSourceModelPresenter()
{
}

QWidget * medSourceModelPresenter::buildWidget()
{
    return buildTree();
}

QTreeView * medSourceModelPresenter::buildTree(QSortFilterProxyModel *proxy)
{
    QTreeView *treeViewRes = new QTreeView();
    QAbstractItemModel *model = nullptr;
    medSourceModel *inputModel = d->sourceItemModel; //Warning, we must pass by local variable, because lambda will capture by value (d) and presenter will be already deleted 

    if (proxy)
    {
        proxy->setParent(treeViewRes);
        proxy->setSourceModel(d->sourceItemModel);
        QObject::connect(treeViewRes, &QTreeView::pressed, [=](const QModelIndex &proxyIndex) {inputModel->itemPressed(proxy->mapToSource(proxyIndex));});
        model = proxy;

    }
    else
    {
        QObject::connect(treeViewRes, &QTreeView::pressed, inputModel, &medSourceModel::itemPressed);
        model = d->sourceItemModel;
    }
    treeViewRes->setModel(model);
    treeViewRes->setSortingEnabled(false);
    //auto selectionModel = new QItemSelectionModel();
    //selectionModel->setModel(model);
    //treeViewRes->setSelectionModel(selectionModel);
    //treeViewRes->setAlternatingRowColors(true);
    treeViewRes->setAnimated(true);
    //treeViewRes->sortByColumn(0, Qt::AscendingOrder);
//    treeViewRes->setSelectionBehavior(QAbstractItemView::SelectRows);

    treeViewRes->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
    treeViewRes->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeViewRes->setDragEnabled(true);
    treeViewRes->setAcceptDrops(true);
    treeViewRes->setDropIndicatorShown(true);

//    treeViewRes->setStyleSheet("QTreeView::item:selection{background-color:transparent}");
    treeViewRes->setItemDelegateForColumn(0, new medSourcesItemDelegate(treeViewRes));

    connect(model, &QObject::destroyed, treeViewRes, &QObject::deleteLater);

    return treeViewRes;
}

medSourceModel * medSourceModelPresenter::sourceModel() const
{
    return d->sourceItemModel;
}

