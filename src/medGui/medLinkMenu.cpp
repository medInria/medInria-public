/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medLinkMenu.h>

#include <medListWidget.h>


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
    d->paramList->setMouseTracking(true);

    d->newGroupitem = new QListWidgetItem("New Group...");
    d->groupList->addItem(d->newGroupitem);
    d->newGroupEdit = new QLineEdit("New Group...");
    d->groupList->setItemWidget(d->newGroupitem, d->newGroupEdit);

    QVBoxLayout *popUpLayout = new QVBoxLayout(d->popupWidget);
    popUpLayout->setContentsMargins(0,0,0,0);
    popUpLayout->addWidget(d->groupList);

    connect(this, SIGNAL(clicked()), this, SLOT(showPopup()));
    connect(d->newGroupEdit, SIGNAL(returnPressed()), this, SLOT(createNewGroup()));
    connect(d->groupList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectItem(QListWidgetItem*)));
    connect(d->paramList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectItem(QListWidgetItem*)));
    connect(d->groupList, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(showSubMenu(QListWidgetItem*)));
    connect(d->paramList, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(highLightParam(QListWidgetItem*)));

    QVBoxLayout *subPopUpLayout = new QVBoxLayout(d->subPopupWidget);
    subPopUpLayout->setContentsMargins(0,0,0,0);
    subPopUpLayout->addWidget(d->paramList);

    d->groupList->installEventFilter(this);
    d->paramList->installEventFilter(this);
    d->newGroupEdit->installEventFilter(this);
    qApp->activeWindow()->installEventFilter(this);
}

void medLinkMenu::createNewGroup()
{
    QListWidgetItem * item = new QListWidgetItem();
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
    connect(groupWidget, SIGNAL(deletionRequested()), this, SLOT(deleteGroup()));
}

void medLinkMenu::updateGroupEditOnFocusIn()
{
    d->newGroupEdit->blockSignals(true);
    if(d->newGroupEdit->text() == "New Group...")
        d->newGroupEdit->setText("");
    d->newGroupEdit->blockSignals(false);
}

void medLinkMenu::updateGroupEditOnFocusOut()
{
    d->newGroupEdit->blockSignals(true);
    if(d->newGroupEdit->text() == "")
        d->newGroupEdit->setText("New Group...");
    d->newGroupEdit->blockSignals(false);
}

void medLinkMenu::selectItem(QListWidgetItem *item)
{
    if(item == d->newGroupitem)
        return;

    if(item->checkState() == Qt::Checked)
        item->setCheckState(Qt::Unchecked);
    else item->setCheckState(Qt::Checked);

    if(item->listWidget() == d->groupList)
        showSubMenu(item);
}

void medLinkMenu::showPopup ()
{
    d->groupList->setFocus();

    if(!d->popupWidget->isVisible())
    {
        QPoint globalPos = mapToGlobal(QPoint(0,0));

        d->popupWidget->move( globalPos.x(), globalPos.y() + this->height());

        d->popupWidget->show();
    }
}

bool medLinkMenu::eventFilter(QObject *object, QEvent *event)
{
    if(object == d->newGroupEdit )
    {
        if( event->type() == QEvent::FocusIn )
            hideSubMenu();
        else if( event->type() == QEvent::MouseButtonPress )
            updateGroupEditOnFocusIn();
        else if( event->type() == QEvent::FocusOut )
            updateGroupEditOnFocusOut();
    }

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
            hideMenus();
        }
    }
    else if(object == qApp->activeWindow() && event->type() == QEvent::Move)
    {
        hideMenus();
    }

    return false;

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

void medLinkMenu::hideMenus()
{
    d->popupWidget->hide();
    d->subPopupWidget->hide();
}

void medLinkMenu::showSubMenu(QListWidgetItem *item)
{
    d->subPopupWidget->show();

    QWidget *w = dynamic_cast<QWidget*>(d->groupList->itemWidget(item));
    QPoint globalPosItem = w->mapToGlobal(QPoint(0,0));
    QPoint globalPosButton = mapToGlobal(QPoint(0,0));

    d->subPopupWidget->move( globalPosButton.x() - d->subPopupWidget->width(), globalPosItem.y());

    item->setSelected(true);
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
    d->subPopupWidget->hide();
}

void medLinkMenu::paintEvent(QPaintEvent *ev)
{
    QPushButton::paintEvent(ev);

    d->groupList->setFixedWidth(this->width());
    d->popupWidget->setFixedWidth(this->width());

    updateListsPosition();
}

void medLinkMenu::updateListsPosition()
{
    QPoint globalPos = mapToGlobal(QPoint(0,0));
    d->popupWidget->move( globalPos.x(), globalPos.y() + this->height());

    QPoint currentPos = d->subPopupWidget->mapToGlobal(QPoint(0,0));

    d->subPopupWidget->move( globalPos.x() - d->subPopupWidget->width(), currentPos.y());
}

void medLinkMenu::highLightParam(QListWidgetItem *item)
{
    item->setSelected(true);
}

void medLinkMenu::deleteGroup()
{
    // TODO: maybe consider the other way around,
    // delete the pool and update the list according the pool still available

    d->groupList->blockSignals(true);

    medGroupWidget *w = dynamic_cast<medGroupWidget*>(this->sender());
    QListWidgetItem *itemToRemove = NULL;

    for(int i=0; i<d->groupList->count(); i++)
    {
        QListWidgetItem *item = d->groupList->item(i);
        if( w == d->groupList->itemWidget(item) )
        {
            itemToRemove = item;
        }
    }

    if(itemToRemove)
    {
        d->groupList->model()->removeRow(d->groupList->row(itemToRemove));
        hideSubMenu();
    }

    d->groupList->blockSignals(false);
}



/*=========================================================================
    medGroupWidget
=========================================================================*/

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

    connect(removeButton, SIGNAL(clicked()), this, SIGNAL(deletionRequested()));
}

void medGroupWidget::enterEvent(QEvent *)
{
    emit enterEvent();
}

void medGroupWidget::leaveEvent(QEvent *)
{
    emit leaveEvent();
}



/*=========================================================================
    medLeftArrow
=========================================================================*/

void medLeftArrow::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);

    QStyleOptionFrame opt;
    opt.init(this);

    style()->drawPrimitive( QStyle::PE_IndicatorArrowLeft, &opt, &p, this);
}

