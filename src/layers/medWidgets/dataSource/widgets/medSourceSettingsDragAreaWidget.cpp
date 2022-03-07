/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 * 
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "medSourceSettingsDragAreaWidget.h"

#include <QScrollArea>
#include <QListWidget>

medSourceSettingsDragAreaWidget::medSourceSettingsDragAreaWidget(medSourcesLoader * pSourceLoader, 
                                                                 QTreeWidget * sourceInformation, 
                                                                 QWidget *parent)
    : QListWidget(parent)
{
    _pSourceLoader = pSourceLoader;
    _sourceInformation = sourceInformation;

    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setAutoScroll(true);
    setFixedSize(400, 550);

    // Change the color of the background when an item is selected to drag&drop
    setStyleSheet("QListWidget::item:selected {background:gray;}");

    for (auto pSource : pSourceLoader->sourcesList())
    {
        addNewSourceItem(pSource);
    }

    connect (this->model(), &QAbstractItemModel::rowsMoved, [=](const QModelIndex &parent, 
                                                                int start, 
                                                                int end, 
                                                                const QModelIndex &destination, 
                                                                int row)
    {
        rowMoved(parent, start, end, destination, row);
    });
}

/**
 * @brief Add a new source item in the list
 * 
 * @param pSource the new source
 */
void medSourceSettingsDragAreaWidget::addNewSourceItem(medAbstractSource * pSource)
{
    auto * newSourceWidgetItem = new medSourceSettingsWidget(_pSourceLoader, pSource, _sourceInformation, this);

    int newRowNumber = count();
    insertItem(newRowNumber, "");
    setItemWidget(item(newRowNumber), newSourceWidgetItem);

    // Initial size of item
    QSize initialSize = newSourceWidgetItem->size();
    newSourceWidgetItem->saveInitialSize(initialSize);
    item(newRowNumber)->setSizeHint(initialSize);

    setSpacing(2);

    connect(newSourceWidgetItem, &medSourceSettingsWidget::sourceItemChosen, [=](medAbstractSource * pSource)
    {
        setSelectedVisualisation(pSource);
        emit(sourceItemChosen(pSource)); // Sent to global source settings widget
    });

    connect(newSourceWidgetItem, &medSourceSettingsWidget::deletedWidget, [=](QString sourceName)
    {
        removeSourceItem(sourceName);
    });

    connect(newSourceWidgetItem, &medSourceSettingsWidget::minimizationAsked, [=](bool isMinimized)
    {
        switchMinimization(newSourceWidgetItem, isMinimized);
    });
}

/**
 * @brief Update graphic interfaces for items in the list: selected item is shown, 
 * non-selected ones are set to normal interfaces
 * 
 * @param pSource the selected source
 */
void medSourceSettingsDragAreaWidget::setSelectedVisualisation(medAbstractSource * pSource)
{
    for (int i = 0; i < this->count(); ++i)
    {
        auto * widget = itemWidget(item(i));
        if(widget)
        {
            auto sourceWidget = dynamic_cast<medSourceSettingsWidget*>(widget);
            if (sourceWidget)
            {
                if (sourceWidget->getInstanceName() == pSource->getInstanceName())
                {
                    sourceWidget->setSelectedVisualisation(true);
                }
                else
                {
                    sourceWidget->setSelectedVisualisation(false);
                }
            }
        }
    }
}

/**
 * @brief Update default state to every items: only one default item can be set
 * 
 * @param pSource the selected source
 */
void medSourceSettingsDragAreaWidget::setSourceToDefault(medAbstractSource * pSource)
{
    for (int i = 0; i < this->count(); ++i)
    {
        auto * widget = itemWidget(item(i));
        if(widget)
        {
            auto sourceWidget = dynamic_cast<medSourceSettingsWidget*>(widget);
            if (sourceWidget)
            {
                if (sourceWidget->getInstanceName() == pSource->getInstanceName())
                {
                    sourceWidget->setToDefault(true);
                }
                else
                {
                    sourceWidget->setToDefault(false);
                }
            }
        }
    }
}

/**
 * @brief Check the list of source items, and send new connection state to the chosen source item
 * 
 * @param pSource the selected source
 * @param connection boolean: is it a connection or not
 */
void medSourceSettingsDragAreaWidget::setConnectionIcon(medAbstractSource * pSource, bool connection)
{
    for (int i = 0; i < this->count(); ++i)
    {
        auto * widget = itemWidget(item(i));
        if(widget)
        {
            auto sourceWidget = dynamic_cast<medSourceSettingsWidget*>(widget);
            if (sourceWidget)
            {
                if (sourceWidget->getInstanceName() == pSource->getInstanceName())
                {
                    sourceWidget->switchConnectionIcon(connection);
                }
            }
        }
    }
}

/**
 * @brief When a source widget is deleted, remove the item in the list
 * 
 * @param sourceName the deleted source name
 */
void medSourceSettingsDragAreaWidget::removeSourceItem(QString sourceName)
{
    for (int i = 0; i < this->count(); ++i)
    {
        auto * widget = itemWidget(item(i));
        if(widget)
        {
            auto sourceWidget = dynamic_cast<medSourceSettingsWidget*>(widget);
            if (sourceWidget)
            {
                if (sourceWidget->getInstanceName() == sourceName)
                {
                    delete takeItem(i);
                }
            }
        }
    }
}

void medSourceSettingsDragAreaWidget::switchMinimization(medSourceSettingsWidget* sourceWidget, bool isMinimized)
{
    for (int i = 0; i < this->count(); ++i)
    {
        auto * currentItem = item(i);
        auto * widget = itemWidget(currentItem);
        if(widget)
        {
            auto comparedSourceWidget = dynamic_cast<medSourceSettingsWidget*>(widget);
            if (comparedSourceWidget && (comparedSourceWidget == sourceWidget))
            {
                QSize initialSize = sourceWidget->getInitialSize();
                if (isMinimized)
                {
                    currentItem->setSizeHint(QSize(initialSize.width(), 39));
                }
                else
                {
                    currentItem->setSizeHint(initialSize);
                }
            }
        }
    }
}

void medSourceSettingsDragAreaWidget::rowMoved(const QModelIndex &parent, 
                                               int start, 
                                               int end, 
                                               const QModelIndex &destination, 
                                               int row)
{
    // todo update the json file

    // After the move, the moved item is selected
    auto * movedItem = item(row-1);
    auto * widget = itemWidget(movedItem);
    if(widget)
    {
        auto sourceWidget = dynamic_cast<medSourceSettingsWidget*>(widget);
        if (sourceWidget)
        {
            setSelectedVisualisation(sourceWidget->getInstanceSource());
        }
    }
}