#include "medSourcesWidget.h"

#include <medDataModel.h>
#include <medSourceItemModel.h>
#include <medSourceItemModelPresenter.h>

#include <medDataInfoWidget.h>

#include <QPushButton>
#include <QTreeView>
#include <QAction>

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
     m_layout.setAlignment(Qt::AlignTop);
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
    QPushButton *plusButton = new QPushButton(QIcon(":/pixmaps/plus.png"), "");
    plusButton->setToolTip("Expand All");
    plusButton->setMaximumSize(QSize(20,20));
    plusButton->setCheckable(true);
    plusButton->setFlat(true);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(sourceTreeTitle);
    hLayout->addWidget(plusButton, 0, Qt::AlignRight);


    QTreeView   *sourceTreeView  = sourcePresenter->buildTree(new medSortFilterProxyModel());

    connect(plusButton, &QPushButton::toggled, [=](bool checked) {
        if (checked)
        {
            plusButton->setIcon(QIcon(":/pixmaps/minus.png"));
            dataModel->expandAll(sourceInstanceId);
            sourceTreeView->expandAll();
        }
        else
        {
            plusButton->setIcon(QIcon(":/pixmaps/plus.png"));
            sourceTreeView->collapseAll();
        }
    });

    sourceTreeView->setDragEnabled(true);
    sourceTreeView->viewport()->setAcceptDrops(false);
    //sourceTreeView->setDropIndicatorShown(true);
    sourceTreeView->setDragDropMode(QAbstractItemView::DragOnly);

    //context menu code
    auto pMenu = new medSourceContextMenu(sourceTreeView);
    sourceTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_TreeviewByMenuMap[pMenu] = sourceTreeView;
    QAction *pushAction    = new QAction(tr("Push"),          pMenu);
    QAction *refreshAction = new QAction(tr("Refresh"),       pMenu);
    QAction *saveAction    = new QAction(tr("Save on disk"),  pMenu);
    QAction *removeAction  = new QAction(tr("Remove"),        pMenu);
    QAction *fetchAction   = new QAction(tr("Fetch"),         pMenu);
    QAction *preloadAction = new QAction(tr("Pre-Load"),      pMenu);
    QAction *readerAction  = new QAction(tr("Change reader"), pMenu);
    QAction *unloadAction  = new QAction(tr("Unload"),        pMenu);
    QAction *infoAction    = new QAction(tr("Information"),   pMenu);
    pMenu->addAction(pushAction);
    pMenu->addAction(refreshAction);
    pMenu->addAction(saveAction);
    pMenu->addAction(removeAction);
    pMenu->addAction(fetchAction);
    pMenu->addAction(preloadAction);
    pMenu->addAction(readerAction);
    pMenu->addAction(unloadAction);
    pMenu->addAction(infoAction);
    //connect(pushAction,    &QAction::triggered, [=]() {  emit infoActionSignal(this->itemFromMenu(pMenu)); });
    //connect(refreshAction, &QAction::triggered, [=]() {  emit infoActionSignal(this->itemFromMenu(pMenu)); });
    //connect(saveAction,    &QAction::triggered, [=]() {  emit infoActionSignal(this->itemFromMenu(pMenu)); });
    //connect(removeAction,  &QAction::triggered, [=]() {  emit infoActionSignal(this->itemFromMenu(pMenu)); });
    //connect(fetchAction,   &QAction::triggered, [=]() {  emit infoActionSignal(this->itemFromMenu(pMenu)); });
    connect(preloadAction, &QAction::triggered, [=]() {  
        QModelIndex index = this->indexFromMenu(pMenu);
        if (index.isValid())
        {
            const_cast<medSourceItemModel*>(static_cast<const medSourceItemModel*>(index.model()))->fetchData(index);
        }
    });

    //connect(readerAction,  &QAction::triggered, [=]() {  emit infoActionSignal(this->itemFromMenu(pMenu)); });
    //connect(unloadAction,  &QAction::triggered, [=]() {  emit infoActionSignal(this->itemFromMenu(pMenu)); });
    connect(infoAction,    &QAction::triggered, [=]() {
        QMap<QString, QString> mandatoriesAttributes;

        QModelIndex index = this->indexFromMenu(pMenu);
        if (index.isValid())
        {
            mandatoriesAttributes = sourceModel->getMendatoriesMetaData(index);
            auto popupDataInfo = new medDataInfoWidget(mandatoriesAttributes);
            popupDataInfo->show();
        }

    });



    connect(sourceTreeView, &QTreeView::customContextMenuRequested, [=](QPoint const& point) { onCustomContextMenu(point, pMenu); });

    m_layout.addLayout(hLayout);
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

void medSourcesWidget::onCustomContextMenu(QPoint const &point, QMenu *pi_pMenu)
{
    auto pTreeView = m_TreeviewByMenuMap[pi_pMenu];
    QModelIndex index = pTreeView->indexAt(point);
    if (index.isValid())
    {
        QPoint pos = pTreeView->viewport()->mapToGlobal(point);
        pi_pMenu->exec(pos);
    }
}

QModelIndex medSourcesWidget::indexFromMenu(QMenu * pi_pMenu)
{
    QModelIndex indexRes;

    auto sourceTreeView = m_TreeviewByMenuMap[pi_pMenu];    
    auto proxy = static_cast<medSortFilterProxyModel*>(sourceTreeView->model());
    auto pos = sourceTreeView->viewport()->mapFromGlobal(pi_pMenu->pos());
    indexRes = proxy->mapToSource(sourceTreeView->indexAt(pos));

    return indexRes;
}


