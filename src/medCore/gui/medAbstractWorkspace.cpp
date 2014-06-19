/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractWorkspace.h>

#include <QUuid>
#include <QWidgetAction>

#include <medDataIndex.h>
#include <medSettingsManager.h>
#include <medToolBox.h>
#include <medViewContainer.h>
#include <medViewContainerManager.h>
#include <medTabbedViewContainers.h>
#include <medAbstractView.h>
#include <medAbstractLayeredView.h>
#include <medToolBoxHeader.h>
#include <medAbstractInteractor.h>
#include <medMetaDataKeys.h>
#include <medBoolParameter.h>
#include <medStringListParameter.h>
#include <medParameterPool.h>
#include <medParameterPoolManager.h>
#include <medAbstractParameter.h>
#include <medDataManager.h>
#include <medPoolIndicator.h>



class medLinkMenuPrivate
{
public :
    QWidget *popupWidget;
    QWidget *subPopupWidget;

    medListWidget *groupList;
    QListWidgetItem * newGroupitem;
    QLineEdit *newGroupEdit;
    QStringList availableParams;

    medListWidget *paramList;
};

medLinkMenu::medLinkMenu(QWidget * parent) : QPushButton(parent), d(new medLinkMenuPrivate)
{
    this->setIcon(QIcon(":icons/link.svg"));

    d->popupWidget = new QWidget(this);
    d->popupWidget->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint );

    d->subPopupWidget = new QWidget(this);
    d->subPopupWidget->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint );

    d->groupList = new medListWidget;
    d->groupList->setMouseTracking(true);

    d->paramList = new medListWidget;

    d->newGroupitem = new QListWidgetItem("New Group...");
    d->groupList->addItem(d->newGroupitem);
    d->newGroupEdit = new QLineEdit("New Group...");
    d->groupList->setItemWidget(d->newGroupitem, d->newGroupEdit);

    QVBoxLayout *popUpLayout = new QVBoxLayout(d->popupWidget);
    popUpLayout->setContentsMargins(0,0,0,0);
    popUpLayout->addWidget(d->groupList);

    connect(this, SIGNAL(clicked()), this, SLOT(showPopup()));
    connect(d->newGroupEdit, SIGNAL(returnPressed()), this, SLOT(createNewGroup()));
    connect(d->newGroupEdit, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(test()));
    connect(d->groupList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectItem(QListWidgetItem*)));
    connect(d->groupList, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(showSubMenu(QListWidgetItem*)));


    QVBoxLayout *subPopUpLayout = new QVBoxLayout(d->subPopupWidget);
    subPopUpLayout->setContentsMargins(0,0,0,0);
    subPopUpLayout->addWidget(d->paramList);

    d->groupList->installEventFilter(this);
    d->paramList->installEventFilter(this);
}

void medLinkMenu::createNewGroup()
{
    QListWidgetItem * item = new QListWidgetItem(/*d->newGroupEdit->text()*/);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
    item->setCheckState(Qt::Unchecked);
    d->groupList->insertItem(0,item);

    medGroupWidget *groupWidget = new medGroupWidget(d->newGroupEdit->text());
    groupWidget->setFocus();
    d->groupList->setItemWidget(item, groupWidget);

    d->newGroupEdit->blockSignals(true);
    d->newGroupEdit->setText("New Group...");
    d->newGroupEdit->blockSignals(false);

    connect(groupWidget, SIGNAL(enterEvent()), this, SLOT(showSubMenu()));
  //  connect(groupWidget, SIGNAL(leaveEvent()), this, SLOT(hideSubMenu()));

}

void medLinkMenu::test()
{
    if(d->newGroupEdit->text() == "New Group...")
        d->newGroupEdit->setText("");

}

void medLinkMenu::selectItem(QListWidgetItem *item)
{
    if(item == d->newGroupitem)
        return;

    if(item->checkState() == Qt::Checked)
        item->setCheckState(Qt::Unchecked);
    else item->setCheckState(Qt::Checked);
}

void medLinkMenu::showPopup ()
{
    d->groupList->setFocus();

    if(!d->popupWidget->isVisible())
    {
        qDebug() << "showPopup show ";
        QPoint globalPos = mapToGlobal(QPoint(0,0));

        d->popupWidget->move( globalPos.x(), globalPos.y() + this->height());

        d->popupWidget->show();
        d->groupList->show();
    }
}

bool medLinkMenu::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::FocusOut)
    {
        QPoint cursor = QCursor::pos();

        QPoint p1 = d->popupWidget->mapToGlobal(QPoint(0,0));
        QRect rect1( p1, d->popupWidget->size() );

        QPoint p2 = d->subPopupWidget->mapToGlobal(QPoint(0,0));
        QRect rect2( p2, d->subPopupWidget->size() );

        QPoint p3 = this->mapToGlobal(QPoint(0,0));
        QRect rect3( p3, this->size() );

        if( !rect1.contains(cursor) && !rect2.contains(cursor) && !rect3.contains(cursor) )
        {
            qDebug()  << "eventFilter hide";
            d->popupWidget->hide();
            d->subPopupWidget->hide();
        }
    }
}

void medLinkMenu::setAvailableParameters(QStringList parameters)
{
    d->availableParams = parameters;

    foreach(QString param, parameters)
    {
        QListWidgetItem * item = new QListWidgetItem(param);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        item->setCheckState(Qt::Unchecked);
        d->paramList->insertItem(0,item);
    }
}

void medLinkMenu::showSubMenu(QListWidgetItem *item)
{
    d->subPopupWidget->show();
    d->paramList->show();

    QWidget *w = dynamic_cast<QWidget*>(d->groupList->itemWidget(item));    
    QPoint globalPosItem = w->mapToGlobal(QPoint(0,0));
    QPoint globalPosButton = mapToGlobal(QPoint(0,0));

    d->subPopupWidget->move( globalPosButton.x() - d->subPopupWidget->width(), globalPosItem.y());
}

void medLinkMenu::showSubMenu()
{
    QWidget *w = dynamic_cast<QWidget*>(this->sender());

    for(int i=0; i<d->groupList->count(); i++)
    {
        QListWidgetItem *item = d->groupList->item(i);
        if( w == d->groupList->itemWidget(item) )
        {
            showSubMenu(item);
        }
    }
}

void medLinkMenu::hideSubMenu()
{
    qDebug() <<  "hideSubMenu";
    d->subPopupWidget->hide();
    d->paramList->hide();
}


void medLinkMenu::moveEvent(QMoveEvent *)
{
    qDebug() << "moveEvent";

}

void medLinkMenu::resizeEvent(QResizeEvent *)
{
    qDebug() << "resizeEvent";
}

void medLinkMenu::paintEvent(QPaintEvent *ev)
{
    QPushButton::paintEvent(ev);

    QPoint globalPos = mapToGlobal(QPoint(0,0));
    d->popupWidget->move( globalPos.x(), globalPos.y() + this->height());

    QPoint currentPos = d->subPopupWidget->mapToGlobal(QPoint(0,0));

    d->subPopupWidget->move( globalPos.x() - d->subPopupWidget->width(), currentPos.y());
}

/******
  * medGroupWidget
*******/

medGroupWidget::medGroupWidget(QString groupName, QWidget * parent): QWidget(parent)
{
    QHBoxLayout *groupLayout = new QHBoxLayout(this);
    groupLayout->setContentsMargins(0,0,0,0);
    this->setLayout(groupLayout);

    QLabel *label = new QLabel(groupName);

    QPushButton *removeButton = new QPushButton;
    removeButton->setIcon(QIcon(":/icons/cross.svg"));
    removeButton->setIconSize(QSize(12,12));
    removeButton->setFixedSize(12,12);

    medLeftArrow *arrow = new medLeftArrow(this);

    groupLayout->addWidget(arrow);
    groupLayout->addWidget(label);
    groupLayout->addStretch();
    groupLayout->addWidget(removeButton);

}

void medGroupWidget::enterEvent(QEvent *)
{
    emit enterEvent();
}

void medGroupWidget::leaveEvent(QEvent *)
{
    emit leaveEvent();
}


/******
  * medLeftArrow
*******/

void medLeftArrow::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);

    QStyleOptionFrame opt;
    opt.init(this);

    style()->drawPrimitive( QStyle::PE_IndicatorArrowLeft, &opt, &p, this);
}






class medAbstractWorkspacePrivate
{
public:
    QWidget *parent;

    QString name;

    bool databaseVisibility;
    bool toolBoxesVisibility;
    bool userLayerPoolable;
    bool userLayerClosable;
    bool userViewPoolable;

    medTabbedViewContainers * viewContainerStack;
    QHash <QListWidgetItem*, QUuid> containerForLayerWidgetsItem;

    QList<medToolBox*> toolBoxes;
    medToolBox *selectionToolBox;
    medToolBox *layerListToolBox;
    medToolBox *interactorToolBox;
    medToolBox *navigatorToolBox;
    medToolBox *mouseInteractionToolBox;
    medListWidget* layerListWidget;

    QList<QListWidgetItem*> selectedLayers;

    medParameterPool *temporaryPoolForInteractors;
};

medAbstractWorkspace::medAbstractWorkspace(QWidget *parent) : QObject(parent), d(new medAbstractWorkspacePrivate)
{
    d->parent = parent;

    d->selectionToolBox = new medToolBox;
    d->selectionToolBox->setTitle("Selection");
    d->selectionToolBox->header()->hide();
    d->selectionToolBox->hide();

    d->viewContainerStack = new medTabbedViewContainers(this, parent);

    connect(d->viewContainerStack, SIGNAL(containersSelectedChanged()), this, SLOT(updateNavigatorsToolBox()));

    d->databaseVisibility = true;
    d->toolBoxesVisibility = true;

    d->mouseInteractionToolBox = new medToolBox;
    d->mouseInteractionToolBox->setTitle("Mouse Interaction");
    d->mouseInteractionToolBox->hide();
    d->selectionToolBox->addWidget(d->mouseInteractionToolBox);

    d->navigatorToolBox = new medToolBox;
    d->navigatorToolBox->setTitle("View settings");
    d->navigatorToolBox->hide();
    d->selectionToolBox->addWidget(d->navigatorToolBox);

    d->layerListToolBox = new medToolBox;
    d->layerListToolBox->setTitle("Layer settings");
    d->layerListToolBox->hide();
    d->selectionToolBox->addWidget(d->layerListToolBox);

    d->interactorToolBox = new medToolBox;
    d->interactorToolBox->setTitle("Interactors");
    d->interactorToolBox->header()->hide();
    d->interactorToolBox->hide();
    d->selectionToolBox->addWidget(d->interactorToolBox);

    d->layerListToolBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    d->layerListWidget = NULL;

    d->temporaryPoolForInteractors = new medParameterPool(this);

    this->setUserLayerClosable(true);
    this->setUserLayerPoolable(true);
    this->setUserViewPoolable(true);
}

medAbstractWorkspace::~medAbstractWorkspace(void)
{
    delete d;
    d = NULL;
}

void medAbstractWorkspace::addToolBox(medToolBox *toolbox)
{
    d->toolBoxes.append(toolbox);
}

void medAbstractWorkspace::removeToolBox(medToolBox *toolbox)
{
    d->toolBoxes.removeOne(toolbox);
}

QList<medToolBox*> medAbstractWorkspace::toolBoxes() const
{
    return d->toolBoxes;
}

medToolBox* medAbstractWorkspace::selectionToolBox() const
{
    return d->selectionToolBox;
}

void medAbstractWorkspace::setDatabaseVisibility(bool visibility)
{
    d->databaseVisibility = visibility;
}

bool medAbstractWorkspace::isDatabaseVisible(void) const
{
    return d->databaseVisibility;
}

medTabbedViewContainers* medAbstractWorkspace::stackedViewContainers() const
{
    return d->viewContainerStack;
}

void medAbstractWorkspace::clear()
{
    this->setupViewContainerStack();
    this->clearWorkspaceToolBoxes();
    return;
}

void medAbstractWorkspace::setToolBoxesVisibility (bool value)
{
    d->toolBoxesVisibility = value;
}

bool medAbstractWorkspace::areToolBoxesVisible() const
{
    return d->toolBoxesVisibility;
}


void medAbstractWorkspace::clearWorkspaceToolBoxes()
{
    foreach(medToolBox* tb,d->toolBoxes)
        tb->clear();
}

void medAbstractWorkspace::addNewTab()
{
    QString tabName = QString("%1 %2").arg(this->name()).arg(d->viewContainerStack->count());
    d->viewContainerStack->addContainerInTab(tabName);
}

void medAbstractWorkspace::updateNavigatorsToolBox()
{
    d->navigatorToolBox->clear();

    medAbstractView* view = NULL;
    QList<QWidget*>  navigators;
    QStringList viewType;
    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        // update the toolbox when the content of the view change
        view = container->view();
        // add nothing if the view is empty
        if(!view)
            continue;
        // get only one navigator per view type
        if(!viewType.contains(view->identifier()))
        {
            viewType << view->identifier();
            navigators << view->navigatorWidget();
        }
    }
    // add the navigators widgets
    d->navigatorToolBox->show();
    foreach(QWidget* navigator, navigators)
    {
        d->navigatorToolBox->addWidget(navigator);
        navigator->show();
    }

    // add link parameter
    if(view && d->userViewPoolable)
    {
        QWidget *linkWidget = new QWidget;
        QHBoxLayout* linkLayout = new QHBoxLayout(linkWidget);

        medLinkMenu *menu = new medLinkMenu(linkWidget);
        menu->setAvailableParameters(QStringList() << "Foo" << "Bar" << "Yeah");

        linkLayout->addWidget(new QLabel(tr("Link view properties: ")));
        linkLayout->addWidget(menu);

        d->navigatorToolBox->addWidget(linkWidget);

    }

    // update the mouse interaction and layer toolboxes according to the selected containers
    this->updateMouseInteractionToolBox();
    this->updateLayersToolBox();
}

void medAbstractWorkspace::updateMouseInteractionToolBox()
{
    d->mouseInteractionToolBox->clear();

    QList<QWidget*>  navigators;
    QStringList viewType;
    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        // update the toolbox when the content of the view change
        medAbstractView* view = container->view();
        // add nothing if the view is empty
        if(!view)
            continue;
        // get only one navigator per view type
        if(!viewType.contains(view->identifier()))
        {
            viewType << view->identifier();
            navigators << view->mouseInteractionWidget();
        }
    }
    // add the navigators widgets
    d->mouseInteractionToolBox->show();
    foreach(QWidget* navigator, navigators)
    {
        d->mouseInteractionToolBox->addWidget(navigator);
        navigator->show();
    }
}

void medAbstractWorkspace::updateLayersToolBox()
{
    d->layerListToolBox->clear();
    d->containerForLayerWidgetsItem.clear();
    d->selectedLayers.clear();

    delete d->layerListWidget;
    d->layerListWidget = new medListWidget;
    d->layerListWidget->setAlternatingRowColors(true);

    d->layerListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    d->layerListWidget->setFocusPolicy(Qt::NoFocus);
    d->layerListWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    connect(d->layerListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(changeCurrentLayer(int)));
    connect(d->layerListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateInteractorsToolBox()));

    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
    {
        // fill the layer widget
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        medAbstractLayeredView* layeredView = dynamic_cast<medAbstractLayeredView*>(container->view());
        if(layeredView)
        {
            if(d->layerListWidget->count() != 0)
            {
                // add an empty widget to separate layers from different views
                QListWidgetItem * item = new QListWidgetItem;
                item->setSizeHint(QSize(10, 10));
                item->setFlags(Qt::NoItemFlags);
                d->layerListWidget->addItem(item);
            }

            for(int layer = layeredView->layersCount() - 1; layer >= 0; --layer)
            {
                if(layer < 0)
                    break;

                medAbstractData *data = layeredView->layerData(layer);
                if(!data)
                    continue;

                QWidget *layerWidget = new QWidget;
                layerWidget->setObjectName("layerWidget");

                QString name = medMetaDataKeys::SeriesDescription.getFirstValue(data,"<i>no name</i>");

                QHBoxLayout* layout = new QHBoxLayout(layerWidget);
                layout->setContentsMargins(0,0,10,0);

                medBoolParameter* visibilityParam = dynamic_cast<medBoolParameter*>(layeredView->visibilityParameter(layer));
                QPushButton* thumbnailButton = visibilityParam->getPushButton();

                QFont myFont;
                QFontMetrics fm(myFont);
                //TODO: could be nice to elide according to current width (update when resize)
                QString text = fm.elidedText(name, Qt::ElideRight, 100);
                QLabel *layerName = new QLabel("<font color='Black'>"+text+"</font>", layerWidget);
                layerName->setToolTip(name);

                layout->addWidget(thumbnailButton);
                layout->addWidget(layerName);
                layout->addStretch();
                foreach (medAbstractInteractor *interactor, layeredView->interactors(layer))
                {
                    if(interactor->layerWidget())
                        layout->addWidget(interactor->layerWidget());
                }

                if(d->userLayerPoolable)
                {
                    medPoolIndicator *poolIndicator = new medPoolIndicator;
                    poolIndicator->setLinkParameter(layeredView->layerLinkParameter(layer));
                    layout->addWidget(poolIndicator);
                }

                if(d->userLayerClosable)
                {
                    QPushButton *removeButton = new QPushButton;
                    removeButton->setProperty("row", d->layerListWidget->count());
                    removeButton->setIcon(QIcon(":/icons/cross.svg"));
                    removeButton->setIconSize(QSize(15,15));
                    removeButton->setFixedSize(20,20);

                    layout->addWidget(removeButton);
                    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeLayer()));

                }

                layerWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
                layerWidget->resize(d->selectionToolBox->width(), 25);

                QListWidgetItem * item = new QListWidgetItem;
                item->setData(Qt::UserRole, layer);
                item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable | Qt::ItemIsEnabled); // set checkable flag
                item->setCheckState(Qt::Checked);
                d->containerForLayerWidgetsItem.insert(item, uuid);
                item->setSizeHint(QSize(layerWidget->width(), 25));
                d->layerListWidget->addItem(item);
                d->layerListWidget->setItemWidget(item, layerWidget);
                layerWidget->setFocusPolicy(Qt::NoFocus);

                d->layerListWidget->blockSignals(true);
                if(layeredView->currentLayer() == item->data(Qt::UserRole).toUInt())
                {
                    d->layerListWidget->setCurrentItem(item);
                    item->setSelected(true);
                }
                d->layerListWidget->blockSignals(false);

            }
        }
    }
    // add the layer widgets
    d->layerListToolBox->show();
    d->layerListToolBox->addWidget(d->layerListWidget);

    d->layerListWidget->show();

    this->updateInteractorsToolBox();
}

void medAbstractWorkspace::changeCurrentLayer(int row)
{
    QListWidgetItem* item = d->layerListWidget->item(row);
    QUuid uuid = d->containerForLayerWidgetsItem.value(item);
    medViewContainer* container = medViewContainerManager::instance()->container(uuid);
    if(!container)
        return;

    medAbstractLayeredView *layeredView = dynamic_cast<medAbstractLayeredView*>(container->view());
    if(!layeredView)
        return;

    int currentLayer = item->data(Qt::UserRole).toInt();
    layeredView->setCurrentLayer(currentLayer);

    d->selectedLayers = d->layerListWidget->selectedItems();

    this->updateInteractorsToolBox();
}

void medAbstractWorkspace::updateInteractorsToolBox()
{
    medViewContainerManager *containerMng =  medViewContainerManager::instance();
    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
    {
        containerMng->container(uuid)->highlight();
    }
    d->interactorToolBox->hide();
    d->interactorToolBox->clear();

    if(!d->layerListWidget)
        return;

    // Hack to always have something selected in the layerListWidget
    // TODO: try to find a better solution...
    if(d->layerListWidget->selectedItems().count() == 0)
    {
        foreach(QListWidgetItem* item, d->selectedLayers)
        {
            d->layerListWidget->blockSignals(true);
            item->setSelected(true);
            d->layerListWidget->blockSignals(false);
        }
    }
    else d->selectedLayers = d->layerListWidget->selectedItems();

    QListWidgetItem* item = d->layerListWidget->currentItem();
    if(!item)
        return;

    int currentLayer = item->data(Qt::UserRole).toInt();
    QList<QString> interactorsIdentifier;
    QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
    medViewContainer *container = containerMng->container(containerUuid);
    container->highlight("#FF8844");

    medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView*>(container->view());

    // add link parameter
    if(view &&  d->userLayerPoolable)
    {
        QWidget *linkWidget = new QWidget;
        QHBoxLayout* linkLayout = new QHBoxLayout(linkWidget);

        view->layerLinkParameter(currentLayer)->getLabel()->setText(tr("Link layer properties: "));
        linkLayout->addWidget(view->layerLinkParameter(currentLayer)->getLabel());
        linkLayout->addWidget(view->layerLinkParameter(currentLayer)->getComboBox());

        d->interactorToolBox->addWidget(linkWidget);

        view->layerLinkParameter(currentLayer)->getLabel()->show();
        view->layerLinkParameter(currentLayer)->getComboBox()->show();

        d->interactorToolBox->show();
    }

    // If we have a multiSelection we just want to colorise the container that have at least one layer selected.
    if(d->layerListWidget->selectedItems().size() > 1)
    {
        foreach(QListWidgetItem* item, d->layerListWidget->selectedItems())
        {
            QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
            medViewContainer *container = containerMng->container(containerUuid);
            container->highlight("#FF8844");
        }

        buildTemporaryPool();

        return;
    }

    foreach (medAbstractInteractor* interactor, view->interactors(currentLayer))
    {
        QString interactorIdentifier = interactor->identifier();
        if(!interactorsIdentifier.contains(interactorIdentifier))
        {
            interactorsIdentifier << interactorIdentifier;
            QGroupBox *groupBox = new QGroupBox;
            QVBoxLayout *groupBoxLayout = new QVBoxLayout(groupBox);

            groupBoxLayout->setContentsMargins(0,0,0,0);
            QWidget *intercatorWidget = interactor->toolBoxWidget();
            if(intercatorWidget)
            {
                groupBoxLayout->addWidget(interactor->toolBoxWidget());
                d->interactorToolBox->addWidget(groupBox);
                interactor->toolBoxWidget()->show();
            }
        }
    }
    d->interactorToolBox->show();

    buildTemporaryPool();
}


void medAbstractWorkspace::removeLayer()
{
    QPushButton *button = dynamic_cast<QPushButton*>(this->sender());
    if(!button)
        return;

    int row = button->property("row").toInt();
    QListWidgetItem* item = d->layerListWidget->item(row);
    QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);

    int layer = item->data(Qt::UserRole).toInt();

    medAbstractLayeredView *layerView = dynamic_cast<medAbstractLayeredView *>(medViewContainerManager::instance()->container(containerUuid)->view());
    if(!layerView)
        return;

    layerView->removeLayer(layer);
    this->updateLayersToolBox();
}


void medAbstractWorkspace::buildTemporaryPool()
{
    medViewContainerManager *containerMng =  medViewContainerManager::instance();
    d->temporaryPoolForInteractors->clear();

    foreach(QListWidgetItem* item, d->layerListWidget->selectedItems())
    {
        QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
        medViewContainer *container = containerMng->container(containerUuid);
        int layer = item->data(Qt::UserRole).toInt();

        medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView*>(container->view());

        foreach (medAbstractInteractor* interactor, view->interactors(layer))
        {
            d->temporaryPoolForInteractors->append(interactor->linkableParameters());
        }

        d->temporaryPoolForInteractors->append(view->layerLinkParameter(layer));
    }
}

void medAbstractWorkspace::open(const medDataIndex &index)
{
    QList<QUuid> containersSelected = d->viewContainerStack->containersSelected();
    if(containersSelected.size() != 1)
        return;

    medViewContainer *container = medViewContainerManager::instance()->container(containersSelected.first());
    if(index.isValidForSeries())
        container->addData(medDataManager::instance()->data(index));
}

void medAbstractWorkspace::setUserLayerPoolable(bool poolable)
{
    d->userLayerPoolable = poolable;
    this->updateLayersToolBox();

}

void medAbstractWorkspace::setUserViewPoolable(bool poolable)
{
    d->userViewPoolable = poolable;
    this->updateNavigatorsToolBox();

}

void medAbstractWorkspace::setUserLayerClosable(bool Closable)
{
    d->userLayerClosable = Closable;
    this->updateLayersToolBox();
}

bool medAbstractWorkspace::isUserLayerPoolable() const
{
    return d->userLayerPoolable;
}

bool medAbstractWorkspace::isUserViewPoolable() const
{
    return d->userViewPoolable;
}


bool medAbstractWorkspace::isUserLayerClosable() const
{
    return d->userLayerClosable;
}
