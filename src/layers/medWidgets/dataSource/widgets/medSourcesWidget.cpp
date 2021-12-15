#include "medSourcesWidget.h"

#include <medDataModel.h>
#include <medDataModelElement.h> //TODO must be renamed by medSourceItemModel
#include <medSourceItemModelPresenter.h>

#include <QWidget>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>

class medSortFilterProxyModel : public QSortFilterProxyModel
{
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override
    {
        return source_column == 0;
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