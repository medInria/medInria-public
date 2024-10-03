#include "medVirtualRepresentationWidget.h"
#include "medVirtualRepresentationItemDelegate.h"

QModelIndex menuGetIndexHelper(QTreeView *pTreeView, QMenu *pMenu)
{
    QModelIndex indexRes;

    auto pos = pTreeView->viewport()->mapFromGlobal(pMenu->pos());
    indexRes = pTreeView->indexAt(pos);

    return indexRes;
}	


medVirtualRepresentationWidget::medVirtualRepresentationWidget()
{
    this->setLayout(&m_layout);
    m_layout.setAlignment(Qt::AlignTop);
    m_layout.setContentsMargins(0, 0, 0, 0);
}

medVirtualRepresentationWidget::~medVirtualRepresentationWidget()
{


}

void medVirtualRepresentationWidget::create(medVirtualRepresentation *virtualRepresentation)
{
    QToolButton *arrowButton = new QToolButton();
    arrowButton->setIcon(QIcon::fromTheme("arrow-bot"));
    QLabel      *title = new QLabel("Quick Access");

    arrowButton->setToolTip("Show/Hide");
    arrowButton->setMaximumSize(QSize(20,20));

    titleLayout = new QHBoxLayout;
    titleLayout->addWidget(arrowButton);
    titleLayout->addWidget(title);

    virtualTree = new QTreeView();
    
    virtualTree->setModel(virtualRepresentation);
    virtualTree->setEnabled(true);
    virtualTree->setVisible(true);
	
    virtualTree->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
    virtualTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    virtualTree->setDragEnabled(true);
    virtualTree->setAcceptDrops(true);
    virtualTree->setDropIndicatorShown(true);

    // virtualTree->setItemDelegateForColumn(0, new medVirtualRepresentationItemDelegate());

	//context menu code
    auto pMenu = new QMenu(virtualTree);
    virtualTree->setContextMenuPolicy(Qt::CustomContextMenu);
	
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
	

    bool con1 = connect(virtualRepresentation, &medVirtualRepresentation::expandData, virtualTree, &QTreeView::setExpanded);
    bool con2 = connect(virtualRepresentation, SIGNAL(editIndex(QModelIndex const &)), virtualTree, SLOT(edit(const QModelIndex &)));
    connect(pinUnpinAction,    &QAction::triggered, [=]() {  virtualRepresentation->pinData(menuGetIndexHelper(virtualTree, pMenu)); });
    connect(createFolderAction, &QAction::triggered, [=]() {  virtualRepresentation->create(menuGetIndexHelper(virtualTree, pMenu), "tmp"); });
    connect(removeAction,  &QAction::triggered, [=]() {  virtualRepresentation->remove(menuGetIndexHelper(virtualTree, pMenu));  });


    connect(virtualTree, &QTreeView::customContextMenuRequested, [=](const QPoint & point)
    {
        QPoint translatedPoint = virtualTree->viewport()->mapToGlobal(point);
        //QModelIndex index = menuGetIndexHelper(widgetRes, pMenu);
        QModelIndex index = virtualTree->indexAt(point);

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

    bool state = virtualRepresentation->rowCount() == 0;
    virtualTree->setVisible(!state);
    if (state)
    {
         arrowButton->setIcon(QIcon::fromTheme("arrow-right")); 
    }
    else
    {
        arrowButton->setIcon(QIcon::fromTheme("arrow-bot"));
    }
    connect(arrowButton, &QToolButton::clicked, this, &medVirtualRepresentationWidget::expandTree);
    
    m_layout.addLayout(titleLayout);
    m_layout.addWidget(virtualTree);

    return;
}


void medVirtualRepresentationWidget::expandTree()
{
    if (titleLayout && virtualTree)
    {
        auto button = dynamic_cast<QToolButton *>(titleLayout->itemAt(0)->widget());
        if (virtualTree->isVisible())
        {
            button->setIcon(QIcon::fromTheme("arrow-right"));
            
        }
        else
        {
            button->setIcon(QIcon::fromTheme("arrow-bot"));
        }
        virtualTree->setVisible(!virtualTree->isVisible()); 
    }
}