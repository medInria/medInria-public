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
    QStringList availableGroups;

    medListWidget *paramList;

    QHash<QString, medAbstractParameterGroup*> currentGroups;
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
    connect(d->groupList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectGroup(QListWidgetItem*)));
    connect(d->paramList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(selectParam(QListWidgetItem*)));
    connect(d->groupList, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(showSubMenu(QListWidgetItem*)));
    connect(d->paramList, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(highlightParam(QListWidgetItem*)));

    QVBoxLayout *subPopUpLayout = new QVBoxLayout(d->subPopupWidget);
    subPopUpLayout->setContentsMargins(0,0,0,0);
    subPopUpLayout->addWidget(d->paramList);

    d->groupList->installEventFilter(this);
    d->paramList->installEventFilter(this);
    d->newGroupEdit->installEventFilter(this);
    qApp->activeWindow()->installEventFilter(this);
}

medLinkMenu::~medLinkMenu()
{
}


void medLinkMenu::setAvailableParameters(QStringList parameters)
{
    d->availableParams = parameters;

    for(int i=0; i<d->paramList->count();i++)
        d->paramList->model()->removeRow(i);

    QListWidgetItem * item = new QListWidgetItem("All");
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    d->paramList->insertItem(0,item);

    int i = 1;
    foreach(QString param, parameters)
    {
        QListWidgetItem * item = new QListWidgetItem(param);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        d->paramList->insertItem(i,item);
        i++;
    }
}

void medLinkMenu::addGroup(medAbstractParameterGroup * group)
{
    QString groupName = group->name();

    d->currentGroups.insert(groupName, group);

    QListWidgetItem * item = new QListWidgetItem();
    item->setData(Qt::UserRole, groupName);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
    item->setCheckState(Qt::Unchecked);
    d->groupList->insertItem(0,item);

    medGroupWidget *groupWidget = new medGroupWidget(groupName);
    groupWidget->setColor(group->color());
    groupWidget->setFocus();
    d->groupList->setItemWidget(item, groupWidget);

    connect(groupWidget, SIGNAL(enterEvent()), this, SLOT(showSubMenu()));
    connect(groupWidget, SIGNAL(deletionRequested()), this, SLOT(deleteGroup()));
    connect(groupWidget, SIGNAL(colorChanged(QColor)), this, SLOT(emitGroupColorChangeRequest(QColor)));

    d->popupWidget->resize(d->groupList->sizeHint());
}

void medLinkMenu::setGroups(QList<medAbstractParameterGroup*> groups)
{
    d->currentGroups.clear();

    d->groupList->blockSignals(true);
    for(int i = d->groupList->count()-2; i >= 0; i--)
        d->groupList->model()->removeRow(i);
    d->groupList->blockSignals(false);

    foreach(medAbstractParameterGroup *group, groups)
    {
        addGroup(group);

        QStringList params = group->parameters();

        d->paramList->blockSignals(true);
        // update param items
        for(int i=0; i<d->paramList->count(); i++)
        {
            QListWidgetItem *item = d->paramList->item(i);
            if( params.contains( item->text()) || group->linkAll() ) //TODO GPR: à reprendre group->linkAll
                item->setCheckState(Qt::Checked);
        }
        d->paramList->blockSignals(false);
    }

    d->popupWidget->resize(d->groupList->sizeHint());
}

void medLinkMenu::setSelectedGroups(QStringList groups)
{
    foreach(QString group, groups)
    {
        for(int i=0; i<d->groupList->count(); i++)
        {
            QListWidgetItem *item = d->groupList->item(i);
            if( item->data(Qt::UserRole) == group )
            {
                item->setCheckState(Qt::Checked);
                break;
            }
        }
    }
}

void medLinkMenu::setPartiallySelectedGroups(QStringList groups)
{
    foreach(QString group, groups)
    {
        for(int i=0; i<d->groupList->count(); i++)
        {
            QListWidgetItem *item = d->groupList->item(i);
            if( item->data(Qt::UserRole) == group )
            {
                item->setCheckState(Qt::PartiallyChecked);
                break;
            }
        }
    }
}

void medLinkMenu::createNewGroup()
{
     emit groupCreated(d->newGroupEdit->text());

    d->newGroupEdit->blockSignals(true);
    d->newGroupEdit->setText("New Group...");
    d->newGroupEdit->blockSignals(false);
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

void medLinkMenu::selectGroup(QListWidgetItem *item)
{
    if(item == d->newGroupitem)
        return;

    QString group = item->data(Qt::UserRole).toString();

    if(item->checkState() == Qt::Checked)
    {
        item->setCheckState(Qt::Unchecked);
        emit groupUnchecked(group);
    }
    else
    {
        item->setCheckState(Qt::Checked);
        emit groupChecked(group);
    }

    showSubMenu(item);
}

void medLinkMenu::selectParam(QListWidgetItem *item)
{
    QString group = d->groupList->currentItem()->data(Qt::UserRole).toString();
    bool groupChecked = d->groupList->currentItem()->checkState() == Qt::Checked;
    QString param = item->text();

    if(param == "All")
    {
        if(item->checkState() == Qt::Checked)
          checkAllParams(true);
        else checkAllParams(false);
        return;
    }

    if(item->checkState() == Qt::Checked)
    {
        emit parameterChecked(param, group, groupChecked);

        if(!d->currentGroups.value(group)->parameters().contains(param))
        {
            d->currentGroups.value(group)->addParameterToLink(param);
            d->currentGroups.value(group)->update();
        }
    }
    else
    {
        emit parameterUnchecked(param, group, groupChecked);

        if(d->currentGroups.value(group)->parameters().contains(param))
        {
            d->currentGroups.value(group)->removeParameter(param);
            d->currentGroups.value(group)->update();
        }
    }
}

void medLinkMenu::showPopup ()
{
    d->groupList->setFocus();

    if(!d->popupWidget->isVisible())
    {
        QPoint globalPos = mapToGlobal(QPoint(0,0));

        d->popupWidget->move( globalPos.x(), globalPos.y() + this->height());

        d->popupWidget->resize(d->groupList->sizeHint());
        d->popupWidget->show();
    }
    else
    {
        d->popupWidget->hide();
        d->subPopupWidget->hide();

    }
}

void medLinkMenu::hideMenus()
{
    d->popupWidget->hide();
    d->subPopupWidget->hide();
}

void medLinkMenu::showSubMenu(QListWidgetItem *groupItem)
{ 
    QString group = groupItem->data(Qt::UserRole).toString();

    d->groupList->setCurrentItem(groupItem);
    groupItem->setSelected(true);

    d->subPopupWidget->show();

    QWidget *w = dynamic_cast<QWidget*>(d->groupList->itemWidget(groupItem));
    QPoint globalPosItem = w->mapToGlobal(QPoint(0,0));
    QPoint globalPosButton = mapToGlobal(QPoint(0,0));

    d->subPopupWidget->move( globalPosButton.x() - d->subPopupWidget->width(), globalPosItem.y());

    d->paramList->blockSignals(true);
    checkAllParams(false);
    d->paramList->blockSignals(false);
    updateParamCheckState( group );
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

void medLinkMenu::updateParamCheckState(QString group)
{
    if(group=="")
        return;

    //TODO GPR: not sure this is the good way
    if(d->currentGroups.value(group)->linkAll())
    {
        for(int i=1; i<d->paramList->count(); i++)
        {
            QListWidgetItem *item = d->paramList->item(i);
                item->setCheckState(Qt::Checked);
        }
    }
    else
    {
        QList<QString> params = d->currentGroups.value(group)->parameters();
        foreach(QString param, params)
        {
            for(int i=1; i<d->paramList->count(); i++)
            {
                QListWidgetItem *item = d->paramList->item(i);
                if(item->text() == param)
                    item->setCheckState(Qt::Checked);
            }
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

void medLinkMenu::updateListsPosition()
{
    QPoint globalPos = mapToGlobal(QPoint(0,0));
    d->popupWidget->move( globalPos.x(), globalPos.y() + this->height());

    QPoint currentPos = d->subPopupWidget->mapToGlobal(QPoint(0,0));

    d->subPopupWidget->move( globalPos.x() - d->subPopupWidget->width(), currentPos.y());
}

void medLinkMenu::highlightParam(QListWidgetItem *item)
{
    item->setSelected(true);
}

void medLinkMenu::deleteGroup()
{
    // TODO: maybe consider the other way round,
    // delete the pool and update the list according to the pools still available

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
        QString group = itemToRemove->data(Qt::UserRole).toString();
        bool res = d->groupList->model()->removeRow(d->groupList->row(itemToRemove));
        if(res)
            emit groupDeleted(group);
        hideSubMenu();
    }

    d->groupList->blockSignals(false);

    d->popupWidget->resize(d->groupList->sizeHint());
}

void medLinkMenu::emitGroupColorChangeRequest(QColor color)
{
    medGroupWidget *w = dynamic_cast<medGroupWidget*>(this->sender());

    for(int i=0; i<d->groupList->count(); i++)
    {
        QListWidgetItem *item = d->groupList->item(i);
        if( w == d->groupList->itemWidget(item) )
        {
            QString group = item->data(Qt::UserRole).toString();
            emit groupColorChangeRequest(group, color);
            break;
        }
    }
}

void medLinkMenu::checkAllParams(bool check)
{
    for(int i=0; i<d->paramList->count(); i++)
    {
        QListWidgetItem *item = d->paramList->item(i);
        if(check)
        {
            item->setCheckState(Qt::Checked);
        }
        else
        {
            item->setCheckState(Qt::Unchecked);
        }
    }
}

/*=========================================================================
    medGroupWidget
=========================================================================*/

medGroupWidget::medGroupWidget(QString groupName, QWidget * parent): QWidget(parent)
{
    QHBoxLayout *groupLayout = new QHBoxLayout(this);
    groupLayout->setContentsMargins(0,0,0,0);
    groupLayout->setSpacing(0);
    this->setLayout(groupLayout);

    QLabel *groupLabel = new QLabel(groupName);

    QPushButton *removeButton = new QPushButton;
    removeButton->setIcon(QIcon(":/icons/cross.svg"));
    removeButton->setIconSize(QSize(12,12));
    removeButton->setStyleSheet("margin:0px;border:0;border-radius: 0px;padding: 0px;");
    removeButton->setFixedSize(12,12);
    removeButton->setFlat(true);

    medLeftArrow *arrow = new medLeftArrow(this);

    m_color = QColor::fromHsv(qrand()%360, 255, 210);

    m_labelColorWidget = new QPushButton();
    m_labelColorWidget->setToolTip(tr("Current group color"));
    m_labelColorWidget->setFlat(true);
    m_labelColorWidget->setStyleSheet("margin:0px;border:0;border-radius: 0px;padding: 0px;");
    m_labelColorWidget->setCheckable(false);
    m_labelColorWidget->setText("");
    QPixmap pix(10,10);
    pix.fill(QColor(m_color));
    m_labelColorWidget->setIcon(QIcon(pix));
    m_labelColorWidget->resize(10,10);

    groupLayout->addWidget(arrow);
    groupLayout->addWidget(groupLabel);
    groupLayout->addStretch();
    groupLayout->addWidget(m_labelColorWidget);
    groupLayout->addWidget(removeButton);

    connect(m_labelColorWidget, SIGNAL(clicked()), this, SLOT(setGroupColor()));
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

void medGroupWidget::setColor(QColor color)
{
    QPixmap pix(10,10);
    pix.fill(QColor(color));
    m_labelColorWidget->setIcon(QIcon(pix));
    m_color = color;

    emit colorChanged(m_color);
}

void medGroupWidget::setGroupColor()
{
    QColor newColor = QColorDialog::getColor(m_color);

    if (newColor.isValid() && newColor!=m_color)
        setColor(newColor);
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

