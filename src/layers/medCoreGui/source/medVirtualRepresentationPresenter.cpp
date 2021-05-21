#include "medVirtualRepresentationPresenter.h"

#include <medVirtualRepresentation.h>

#include <QAbstractItemView>
#include <QAction>
#include <QMenu>
#include <QTreeView>

class medVirtualRepresentationPresenterPrivate
{
public:
    medVirtualRepresentation *virtualRepresentation;
};

QModelIndex menuGetIndexHelper(QTreeView *pTreeView, QMenu *pMenu)
{
    QModelIndex indexRes;

    auto pos = pTreeView->viewport()->mapFromGlobal(pMenu->pos());
    indexRes = pTreeView->indexAt(pos);

    return indexRes;
}	
	
medVirtualRepresentationPresenter::medVirtualRepresentationPresenter(medVirtualRepresentation *parent) : d(new medVirtualRepresentationPresenterPrivate())
{
    d->virtualRepresentation = parent;
}

medVirtualRepresentationPresenter::~medVirtualRepresentationPresenter()
{
}

QWidget * medVirtualRepresentationPresenter::buildWidget()
{
    return buildTree();
}

QTreeView * medVirtualRepresentationPresenter::buildTree()
{
    auto widgetRes = new QTreeView();
    
    widgetRes->setModel(d->virtualRepresentation);
    widgetRes->setEnabled(true);
    widgetRes->setVisible(true);
	
    widgetRes->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
    widgetRes->setSelectionMode(QAbstractItemView::ExtendedSelection);
    widgetRes->setDragEnabled(true);
    widgetRes->setAcceptDrops(true);
    widgetRes->setDropIndicatorShown(true);
	
	
	
	 //context menu code
    auto pMenu = new QMenu(widgetRes);
    widgetRes->setContextMenuPolicy(Qt::CustomContextMenu);
	
    QAction *pinUnpinAction     = new QAction(tr("pin / unpin"),   pMenu);
    QAction *createFolderAction = new QAction(tr("Create Folder"), pMenu);
    QAction *removeAction       = new QAction(tr("Remove"),        pMenu);
    QAction *fetchAction        = new QAction(tr("Fetch"),         pMenu);
    QAction *infoAction         = new QAction(tr("Information"),   pMenu);
    pMenu->addAction(pinUnpinAction);
    pMenu->addAction(createFolderAction);
    pMenu->addAction(removeAction);
    pMenu->addAction(fetchAction);
    pMenu->addAction(infoAction);
	

    auto * pVirt = d->virtualRepresentation;
    bool con1 = connect(pVirt, &medVirtualRepresentation::expandData, widgetRes, &QTreeView::setExpanded);
    bool con2 = connect(pVirt, SIGNAL(editIndex(QModelIndex const &)), widgetRes, SLOT(edit(const QModelIndex &)));
    connect(pinUnpinAction,    &QAction::triggered, [=]() {  pVirt->pinData(menuGetIndexHelper(widgetRes, pMenu)); });
    connect(createFolderAction, &QAction::triggered, [=]() {  pVirt->create(menuGetIndexHelper(widgetRes, pMenu), "tmp"); });
    connect(removeAction,  &QAction::triggered, [=]() {  pVirt->remove(menuGetIndexHelper(widgetRes, pMenu));  });


    connect(widgetRes, &QTreeView::customContextMenuRequested, [=](const QPoint & point)
    {
        QPoint translatedPoint = widgetRes->viewport()->mapToGlobal(point);
        //QModelIndex index = menuGetIndexHelper(widgetRes, pMenu);
        QModelIndex index = widgetRes->indexAt(point);

        int iCol = index.column();

        if (index.isValid())
        {            
            pinUnpinAction->setVisible(true);
            if (index.parent().isValid())
            {
                pinUnpinAction->setDisabled(true);
            }
            createFolderAction->setVisible(true);
            removeAction->setVisible(true);
            fetchAction->setVisible(true);
            infoAction->setVisible(true);
        }
        else
        {
            pinUnpinAction->setVisible(false);
            removeAction->setVisible(false);
            fetchAction->setVisible(false);
            infoAction->setVisible(false);
        }
        pMenu->exec(translatedPoint);
    });
	
    return widgetRes;
}

