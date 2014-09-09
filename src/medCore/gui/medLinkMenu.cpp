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
#include <medSettingsManager.h>

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

    QPushButton *saveAsPresetButton;

    medListWidget *presetList;

    QHash<QString, medAbstractParameterGroup*> currentGroups;
    QHash<QString, QStringList> presets;
};

medLinkMenu::medLinkMenu(QWidget * parent) : QPushButton(parent), d(new medLinkMenuPrivate)
{
    this->setIcon(QIcon(":icons/link.svg"));

    d->popupWidget = new QWidget(this);
    d->popupWidget->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint );
    d->popupWidget->setContentsMargins(0,0,4,4);
    d->popupWidget->setObjectName("popupWidget");

    d->subPopupWidget = new QWidget(this);
    d->subPopupWidget->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint );
    d->subPopupWidget->setContentsMargins(0,0,4,4);
    d->subPopupWidget->setObjectName("subPopupWidget");

    d->popupWidget->setAttribute(Qt::WA_TranslucentBackground);
    d->subPopupWidget->setAttribute(Qt::WA_TranslucentBackground);

#ifdef Q_OS_LINUX
    QGraphicsDropShadowEffect *shadowEffect1 = new QGraphicsDropShadowEffect(this);
    shadowEffect1->setOffset(2);
    shadowEffect1->setBlurRadius(8);
    shadowEffect1->setColor(QColor(33, 33, 33, 200));

    QGraphicsDropShadowEffect *shadowEffect2 = new QGraphicsDropShadowEffect(this);
    shadowEffect2->setOffset(2);
    shadowEffect2->setBlurRadius(8);
    shadowEffect2->setColor(QColor(33, 33, 33, 200));

    d->popupWidget->setGraphicsEffect(shadowEffect1);
    d->subPopupWidget->setGraphicsEffect(shadowEffect2);
#endif

    d->groupList = new medListWidget;
    d->groupList->setMouseTracking(true);
    d->groupList->setAlternatingRowColors(true);

    d->paramList = new medListWidget;
    d->paramList->setMouseTracking(true);
    d->paramList->setAlternatingRowColors(true);

    d->newGroupitem = new QListWidgetItem("Add new Group...");
    d->groupList->addItem(d->newGroupitem);
    d->newGroupEdit = new QLineEdit("Add new Group...");
    d->groupList->setItemWidget(d->newGroupitem, d->newGroupEdit);

    d->saveAsPresetButton = new QPushButton("Save as preset");

    d->presetList = new medListWidget;
    d->presetList->setMouseTracking(true);
    d->presetList->setAlternatingRowColors(true);
    d->presetList->hide();

    QVBoxLayout *popUpLayout = new QVBoxLayout(d->popupWidget);
    popUpLayout->setContentsMargins(0,0,0,0);
    popUpLayout->addWidget(d->groupList);

    connect(this, SIGNAL(clicked()), this, SLOT(showPopup()));
    connect(d->newGroupEdit, SIGNAL(returnPressed()), this, SLOT(createNewGroup()));
    connect(d->groupList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectGroup(QListWidgetItem*)));
    connect(d->paramList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(selectParam(QListWidgetItem*)));
    connect(d->groupList, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(showSubMenu(QListWidgetItem*)));
    connect(d->paramList, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(highlightItem(QListWidgetItem*)));
    connect(d->paramList, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(selectItem(QListWidgetItem*)));
    connect(d->saveAsPresetButton, SIGNAL(clicked()), this, SLOT(saveAsPreset()));
    connect(d->presetList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(applyPreset(QListWidgetItem*)));
    connect(d->presetList, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(highlightItem(QListWidgetItem*)));
    connect(d->presetList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editPreset(QListWidgetItem*)));
    connect(d->presetList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectItem(QListWidgetItem*)));


    QWidget *internalSubPopWidget = new QWidget;
    internalSubPopWidget->setObjectName("internalSubPopWidget");
    QVBoxLayout *layout = new QVBoxLayout(d->subPopupWidget);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(internalSubPopWidget);

    QVBoxLayout *subPopUpLayout = new QVBoxLayout(internalSubPopWidget);
    subPopUpLayout->setContentsMargins(3,3,3,3);
    subPopUpLayout->setSpacing(3);
    subPopUpLayout->addWidget(d->paramList);
    subPopUpLayout->addWidget(d->saveAsPresetButton);
    subPopUpLayout->addWidget(d->presetList);

    d->groupList->installEventFilter(this);
    d->paramList->installEventFilter(this);
    d->newGroupEdit->installEventFilter(this);
    d->saveAsPresetButton->installEventFilter(this);
    d->presetList->installEventFilter(this);
    if(qApp->activeWindow())
      qApp->activeWindow()->installEventFilter(this);
}

medLinkMenu::~medLinkMenu()
{
    delete d;
}


void medLinkMenu::setAvailableParameters(QStringList parameters)
{
    d->availableParams = parameters;

    for(int i=0; i<d->paramList->count();i++)
        d->paramList->model()->removeRow(i);

    QListWidgetItem * item = new QListWidgetItem("All");
    item->setSizeHint(QSize(item->sizeHint().width(), 20));
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    d->paramList->insertItem(0,item);

    int i = 1;
    foreach(QString param, parameters)
    {
        QListWidgetItem * item = new QListWidgetItem(param);
        item->setSizeHint(QSize(item->sizeHint().width(), 20));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        d->paramList->insertItem(i,item);
        i++;
    }

    loadPreset();
}

void medLinkMenu::addGroup(medAbstractParameterGroup * group, bool selected)
{
    QString groupName = group->name();

    d->currentGroups.insert(groupName, group);

    QListWidgetItem * item = new QListWidgetItem();
    item->setSizeHint(QSize(item->sizeHint().width(), 20));
    item->setData(Qt::UserRole, groupName);
    item->setToolTip(groupName);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag

    d->groupList->insertItem(0,item);

    medGroupWidget *groupWidget = new medGroupWidget(groupName);
    groupWidget->setColor(group->color());
    groupWidget->setFocus();
    d->groupList->setItemWidget(item, groupWidget);

    connect(groupWidget, SIGNAL(enterEvent()), this, SLOT(showSubMenu()));
    connect(groupWidget, SIGNAL(deletionRequested()), this, SLOT(deleteGroup()));
    connect(groupWidget, SIGNAL(colorChanged(QColor)), this, SLOT(emitGroupColorChangeRequest(QColor)));

    d->popupWidget->resize(d->popupWidget->width(), d->groupList->sizeHint().height());

    if(selected)
      selectGroup(item);
    else item->setCheckState(Qt::Unchecked);
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

        QStringList params = group->parametersToLink();

        d->paramList->blockSignals(true);
        // update param items
        for(int i=0; i<d->paramList->count(); i++)
        {
            QListWidgetItem *item = d->paramList->item(i);
            if( params.contains( item->text()) || group->linkAll() )
                item->setCheckState(Qt::Checked);
        }
        d->paramList->blockSignals(false);
    }

    d->popupWidget->resize(d->popupWidget->width(), d->groupList->sizeHint().height());
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
    d->newGroupEdit->setText("Add new Group...");
    d->newGroupEdit->blockSignals(false);
}

void medLinkMenu::updateGroupEditOnFocusIn()
{
    d->newGroupEdit->blockSignals(true);
    if(d->newGroupEdit->text() == "Add new Group...")
        d->newGroupEdit->setText("");
    d->newGroupEdit->blockSignals(false);
}

void medLinkMenu::updateGroupEditOnFocusOut()
{
    d->newGroupEdit->blockSignals(true);
    if(d->newGroupEdit->text() == "")
        d->newGroupEdit->setText("Add new Group...");
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

        if(!d->currentGroups.value(group)->parametersToLink().contains(param))
        {
            d->currentGroups.value(group)->addParameterToLink(param);
        }
    }
    else
    {
        emit parameterUnchecked(param, group, groupChecked);

        if(d->currentGroups.value(group)->parametersToLink().contains(param))
        {
            d->currentGroups.value(group)->removeParameter(param);
        }

        // a param has been unchecked, uncheck 'All' item
        d->paramList->blockSignals(true);
        d->paramList->item(0)->setCheckState(Qt::Unchecked);
        d->paramList->blockSignals(false);
    }

    uncheckAllPresets();
}

void medLinkMenu::showPopup ()
{
    d->groupList->setFocus();

    if(!d->popupWidget->isVisible())
    {
        QPoint globalPos = mapToGlobal(QPoint(0,0));

        d->popupWidget->move( globalPos.x(), globalPos.y() + this->height());

        d->popupWidget->resize(d->popupWidget->width(), d->groupList->sizeHint().height());
        d->popupWidget->show();
        d->newGroupEdit->setFocus();
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

    QWidget *w = dynamic_cast<QWidget*>(d->groupList->itemWidget(groupItem));
    QPoint globalPosItem = w->mapToGlobal(QPoint(0,0));
    QPoint globalPosButton = mapToGlobal(QPoint(0,0));

    d->subPopupWidget->layout()->invalidate();
    d->subPopupWidget->update();
    d->subPopupWidget->layout()->activate();

    d->subPopupWidget->move( globalPosButton.x() - d->subPopupWidget->width() + d->subPopupWidget->contentsMargins().right(), globalPosItem.y());

    d->paramList->blockSignals(true);
    checkAllParams(false);
    d->paramList->blockSignals(false);
    updateParamCheckState( group );

    d->subPopupWidget->show();

    // [Hack] Sometimes, the subPopupWidget doesn't appear at the right place
    // a second move seems to correct the position...
    d->subPopupWidget->move( globalPosButton.x() - d->subPopupWidget->width() + d->subPopupWidget->contentsMargins().right(), globalPosItem.y());

    d->popupWidget->raise();
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
        QList<QString> params = d->currentGroups.value(group)->parametersToLink();
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
    //d->popupWidget->setFixedWidth(this->width());

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
    else if(object == d->presetList && event->type() == QEvent::Leave)
    {
        d->presetList->clearSelection();
    }
    else if(object == d->paramList && event->type() == QEvent::Leave)
    {
        d->paramList->clearSelection();
    }

    return false;

}

void medLinkMenu::updateListsPosition()
{
    QPoint globalPos = mapToGlobal(QPoint(0,0));
    d->popupWidget->move( globalPos.x(), globalPos.y() + this->height());

    QPoint currentPos = d->subPopupWidget->mapToGlobal(QPoint(0,0));

    d->subPopupWidget->move( globalPos.x() - d->subPopupWidget->width() + d->subPopupWidget->contentsMargins().right(), currentPos.y());
}

void medLinkMenu::highlightItem(QListWidgetItem *item)
{
    item->setSelected(true);
}

void medLinkMenu::deleteGroup()
{
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

    d->popupWidget->resize(d->popupWidget->width(), d->groupList->sizeHint().height());
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

void medLinkMenu::saveAsPreset()
{
    QString group = d->groupList->currentItem()->data(Qt::UserRole).toString();

    QStringList params;
    for(int i=0; i<d->paramList->count(); i++)
    {
        QListWidgetItem *item = d->paramList->item(i);
        if( item->checkState() == Qt::Checked )
            params.append(item->text());
    }

    int i = 1;
    QString newgroup = group;
    //check if there is a preset with the same name
    while(d->presets.keys().contains(newgroup))
    {
        newgroup = group + "_" + QString::number(i);
        i++;
    }
    group = newgroup;

    medSettingsManager::instance()->setValue("GroupPresets", group, params);
    d->presets.insert(group, params);

    addPresetItem(group);

    //TODO: shouldn't have to do that...
    d->subPopupWidget->resize(d->subPopupWidget->width(), d->presetList->sizeHint().height() +
                              d->paramList->sizeHint().height()+30);

    d->presetList->resize(d->presetList->width(), d->presetList->sizeHint().height());
    d->presetList->show();
}

void medLinkMenu::loadPreset()
{
    QStringList presets = medSettingsManager::instance()->keys("GroupPresets");
    foreach(QString preset, presets)
    {
        QStringList params = medSettingsManager::instance()->value("GroupPresets", preset).toStringList();
        bool ok = true;
        foreach(QString param, params)
        {
            if(!d->availableParams.contains(param))
            {
                ok = false;
                break;
            }
        }

        if( ok )
        {
            addPresetItem(preset);
            d->presetList->show();
            d->presets.insert(preset, params);
        }
    }
}

void medLinkMenu::applyPreset(QListWidgetItem* item)
{
    bool checked = (item->checkState() == Qt::Checked);

    // Uncheck all params
    checkAllParams(false);

    // Uncheck all presets except the one modified
    for(int i=0; i<d->presetList->count(); i++)
    {
        QListWidgetItem *presetItem = d->presetList->item(i);
        if(presetItem != item)
        {
            d->presetList->blockSignals(true);
            presetItem->setCheckState(Qt::Unchecked);
            d->presetList->blockSignals(false);
        }
    }

    QString preset  = item->text();
    QStringList presetParams;

    // check if the preset is actually in the preset list (it can have been modified)
    if(!d->presets.contains(preset))
    {
        QStringList presetsInListWidget;
        for(int i=0; i<d->presetList->count(); i++)
        {
            QListWidgetItem *presetItem = d->presetList->item(i);
            presetsInListWidget << presetItem->text();
        }

        // preset has been renamed, need to look for the renamed item
        foreach(QString key, d->presets.keys())
        {
            if(!presetsInListWidget.contains(key))
            {
                presetParams = d->presets[key];
                d->presets.remove(key);
                d->presets.insert(preset, presetParams);
                medSettingsManager::instance()->remove("GroupPresets", key);
                medSettingsManager::instance()->setValue("GroupPresets", preset, presetParams);
            }
        }
    }
    else  presetParams = d->presets[preset];

    foreach(QString presetParam, presetParams)
    {
        for(int i=1; i<d->paramList->count(); i++)
        {
            QListWidgetItem *paramItem = d->paramList->item(i);
            if(paramItem->text() == presetParam)
            {
                if( checked )
                  paramItem->setCheckState(Qt::Checked);
                else paramItem->setCheckState(Qt::Unchecked);
            }
        }
    }

    // Modifying the params check state has unckecked all the presets
    // so we need to restore the correct check state
    d->presetList->blockSignals(true);
    if( checked )
        item->setCheckState(Qt::Checked);
    else item->setCheckState(Qt::Unchecked);
    d->presetList->blockSignals(false);

    //see editPreset(QListWidgetItem* item) for explanation
    QWidget *w = d->presetList->itemWidget(item);
    if(!dynamic_cast<medListItemWidget*>(w))
        addRemovePresetButton(item);
}

void medLinkMenu::uncheckAllPresets()
{
    for(int i=0; i<d->presetList->count(); i++)
    {
        QListWidgetItem *presetItem = d->presetList->item(i);
        d->presetList->blockSignals(true);
        presetItem->setCheckState(Qt::Unchecked);
        d->presetList->blockSignals(false);
    }
}

void medLinkMenu::highlightPreset()
{
    QWidget *w = dynamic_cast<QWidget*>(this->sender());

    for(int i=0; i<d->presetList->count(); i++)
    {
        QListWidgetItem *item = d->presetList->item(i);
        if( w == d->presetList->itemWidget(item) )
        {
            item->setSelected(true);
        }
    }
}

void medLinkMenu::selectItem(QListWidgetItem *item)
{
    if(item->checkState() == Qt::Checked)
    {
        item->setCheckState(Qt::Unchecked);
    }
    else
    {
        item->setCheckState(Qt::Checked);
    }
}

void medLinkMenu::deletePreset()
{
    d->presetList->blockSignals(true);

    QWidget *deleteButton = dynamic_cast<QWidget*>(this->sender());
    QWidget *itemWidget = deleteButton->parentWidget();

    QListWidgetItem *itemToRemove = NULL;

    for(int i=0; i<d->presetList->count(); i++)
    {
        QListWidgetItem *item = d->presetList->item(i);
        if( itemWidget == d->presetList->itemWidget(item) )
        {
            itemToRemove = item;
        }
    }

    if(itemToRemove)
    {
        QString preset = itemToRemove->text();
        d->presetList->model()->removeRow(d->presetList->row(itemToRemove));

        d->presets.remove(preset);
        medSettingsManager::instance()->remove("GroupPresets", preset);
    }

    d->presetList->blockSignals(false);

    d->presetList->resize(d->presetList->width(), d->presetList->sizeHint().height());

}

void medLinkMenu::addPresetItem(QString preset)
{
    QListWidgetItem * item = new QListWidgetItem(preset);
    item->setSizeHint(QSize(item->sizeHint().width(), 20));
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    item->setCheckState(Qt::Unchecked);
    d->presetList->insertItem(0,item);

    addRemovePresetButton(item);
}

void medLinkMenu::editPreset(QListWidgetItem* item)
{
    // It seems that you cannot edit an item if there is a widget asociated
    // removing the widget enables to edit the item
    // the widget will be re-added in applyPreset
    d->presetList->removeItemWidget(item);
    d->presetList->editItem(item);
}

void medLinkMenu::addRemovePresetButton(QListWidgetItem* item)
{
    medListItemWidget *itemWidget = new medListItemWidget;
    itemWidget->setFocusPolicy(Qt::NoFocus);
    itemWidget->setAttribute(Qt::WA_TranslucentBackground);
    QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
    itemLayout->setContentsMargins(0,0,0,0);
    itemLayout->setSpacing(0);

    QPushButton *removeButton = new QPushButton;
    removeButton->setIcon(QIcon(":/icons/cross.svg"));
    removeButton->setIconSize(QSize(12,12));
    removeButton->setStyleSheet("margin:0px;border:0;border-radius: 0px;padding: 0px;");
    removeButton->setFixedSize(12,12);
    removeButton->setFlat(true);

    itemLayout->addStretch();
    itemLayout->addWidget(removeButton);
    d->presetList->setItemWidget(item, itemWidget);

    connect(itemWidget, SIGNAL(enterEvent()), this, SLOT(highlightPreset()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(deletePreset()));
}

/*=========================================================================
    medGroupWidget
=========================================================================*/

medGroupWidget::medGroupWidget(QString groupName, QWidget * parent): medListItemWidget(parent)
{
    QHBoxLayout *groupLayout = new QHBoxLayout(this);
    groupLayout->setContentsMargins(0,0,0,0);
    groupLayout->setSpacing(0);
    this->setLayout(groupLayout);

    QFont myFont;
    QFontMetrics fm(myFont);
    QString text = fm.elidedText(groupName, Qt::ElideRight, 100);
    //QLabel *groupLabel = new QLabel("<font color='Black'>"+text+"</font>", this);
    QLabel *groupLabel = new QLabel(text, this);
    groupLabel->setToolTip(groupName);

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


/*=========================================================================
    medListWidgetItem
=========================================================================*/

medListItemWidget::medListItemWidget(QWidget *parent):QWidget(parent)
{

}

void medListItemWidget::enterEvent(QEvent *)
{
    emit enterEvent();
}

void medListItemWidget::leaveEvent(QEvent *)
{
    emit leaveEvent();
}
