#include "medSourcesWidget.h"

#include <medDataModel.h>
#include <medDataModelElement.h> //TODO must be renamed by medSourceItemModel
#include <medSourceItemModelPresenter.h>

#include <QPushButton>
#include <QTreeView>

class medSortFilterProxyModel : public QSortFilterProxyModel
{
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override
    {
        return source_column == 0;
    }

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        bool bRes = false;
        for (int col = 0; col < sourceModel()->columnCount(source_parent); col++)
        {
            QModelIndex index1 = sourceModel()->index(source_row, col, source_parent);
            if (sourceModel()->data(index1).toString().contains(filterRegExp()))
            {
                bRes = true;
            }
        }
        return bRes;
    }
};

medSourcesWidget::medSourcesWidget()
{
     this->setLayout(&m_layout);
}

medSourcesWidget::~medSourcesWidget()
{
}

void medSourcesWidget::addSources(medDataModel *dataModel)
{
    auto  sourceModels = dataModel->models();
    for (auto sourceModel : sourceModels)
    {
        addSource(dataModel, sourceModel->getSourceIntanceId());
    }
}

void medSourcesWidget::addSource(medDataModel *dataModel, QString sourceInstanceId)
{
    auto                        *sourceModel = dataModel->getModel(sourceInstanceId);
    QString                      instanceName = dataModel->getInstanceName(sourceInstanceId);
    medSourceItemModelPresenter *sourcePresenter = new medSourceItemModelPresenter(sourceModel);

    QPushButton *sourceTreeTitle = new QPushButton(instanceName);
    QTreeView   *sourceTreeView  = sourcePresenter->buildTree(new medSortFilterProxyModel());

    sourceTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
    sourceTreeView->setDragEnabled(true);
    sourceTreeView->viewport()->setAcceptDrops(false);
    //sourceTreeView->setDropIndicatorShown(true);
    sourceTreeView->setDragDropMode(QAbstractItemView::DragOnly);


    m_layout.addWidget(sourceTreeTitle);
    m_layout.addWidget(sourceTreeView);

    connect(sourceTreeView, &QTreeView::clicked, [=]() {
        if (!sourceTreeView->isEnabled())
        {
            dataModel->sourceIsOnline(sourceInstanceId);
        }
    });
    connect(sourceTreeTitle, &QPushButton::clicked, [=]() {
        if (!sourceTreeView->isEnabled())
        {
            dataModel->sourceIsOnline(sourceInstanceId);
        }
    });

    connect(sourceTreeTitle, &QPushButton::clicked, [=]() {sourceTreeView->setVisible(!sourceTreeView->isVisible()); });
    connect(sourceModel, &medSourceItemModel::online, sourceTreeView, &QTreeView::setEnabled);
    //connect(sourceModel, &medSourceItemModel::columnCountChange, [&](int iColumnCount) {
    //    for (int i = 1; i < iColumnCount; ++i)
    //    {
    //        sourceTreeView->setColumnHidden(i, true);
    //    }
    //});

    m_treeMap            [sourceInstanceId] = sourceTreeView;
    m_titleMap           [sourceInstanceId] = sourceTreeTitle;
    delete sourcePresenter;
}

void medSourcesWidget::removeSource(QString sourceInstanceId)
{
    if (m_treeMap.contains(sourceInstanceId))
    {
        auto widget = m_treeMap.take(sourceInstanceId);
        auto label = m_titleMap.take(sourceInstanceId);

        //m_layout.removeWidget(widget);
        //m_layout.removeWidget(label);

        //TODO made disconnection here
        
        delete widget;
        delete label;
    }
}

void medSourcesWidget::filter(const QString &text)
{
    for ( auto sourceTreeView : m_treeMap.values())
    {
        auto tree = dynamic_cast<QTreeView*>(sourceTreeView);
        auto proxy = dynamic_cast<medSortFilterProxyModel *>(tree->model());
        QRegExp regExp(text, Qt::CaseInsensitive, QRegExp::Wildcard);
        proxy->setFilterRegExp(regExp);
        proxy->setRecursiveFilteringEnabled(true);
    }
}
