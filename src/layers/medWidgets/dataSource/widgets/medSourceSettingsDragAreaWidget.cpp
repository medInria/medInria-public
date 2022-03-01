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
#include <medSourceSettingsDragAreaWidget.h>

#include <QMimeData>
#include <QRegularExpression>
#include <QScrollArea>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QWidget>

#include <medSourceSettingsWidget.h>

medSourceSettingsDragAreaWidget::medSourceSettingsDragAreaWidget(medSourcesLoader * pSourceLoader, 
                                                                 QTreeWidget * sourceInformation, 
                                                                 QWidget *parent)
    : QWidget(parent)
{
    _pSourceLoader = pSourceLoader;
    _sourceInformation = sourceInformation;

    auto * pSourceLayout = new QVBoxLayout();
    this->setLayout(pSourceLayout);

    auto * scrollAreaWidget = new QWidget();
    sourceColumnLayout = new QVBoxLayout();
    scrollAreaWidget->setLayout(sourceColumnLayout);
    auto * scrollAreaSources = new QScrollArea();
    scrollAreaSources->setWidgetResizable(true);
    scrollAreaSources->setWidget(scrollAreaWidget);
    pSourceLayout->addWidget(scrollAreaSources);
    
    for (auto pSource : pSourceLoader->sourcesList())
    {
        auto * pSourceParametersBox = new medSourceSettingsWidget(pSourceLoader, pSource, sourceInformation, this);
        sourceColumnLayout->addWidget(pSourceParametersBox);
        sourceColumnLayout->addStretch();

        // TODO add methods
        // Catch the signal emitted when a new source is added in the source loader
        connect(pSourceParametersBox, &medSourceSettingsWidget::defaultChosen, [=](QString instanceName)
        {
            // Remove default state to others
            for (int i = 0; i < sourceColumnLayout->count(); ++i)
            {
                QWidget *widget = sourceColumnLayout->itemAt(i)->widget();
                if(widget != nullptr)
                {
                    auto sourceWidget = dynamic_cast<medSourceSettingsWidget*>(widget);
                    if (sourceWidget && sourceWidget->getInstanceName() != instanceName)
                    {
                        sourceWidget->setToDefault(false);
                    }
                }
            }
        });

        // Catch the signal emitted when a new source item is deleted
        connect(pSourceParametersBox, &medSourceSettingsWidget::deletedWidget, [=]()
        {
            for (int i = 0; i < sourceColumnLayout->count(); ++i)
            {
                QWidget *widget = sourceColumnLayout->itemAt(i)->widget();
                if(widget != nullptr)
                {
                    // Remove the old spacer to add a new one at the end, in order
                    //to keep the widgets the same size
                    auto stretchWidget = dynamic_cast<QSpacerItem*>(widget);
                    if (stretchWidget)
                    {
                        sourceColumnLayout->removeWidget(widget);
                    }
                }
            }
            sourceColumnLayout->addStretch();
        });
    }

    setAcceptDrops(true);
}

void medSourceSettingsDragAreaWidget::dragEnterEvent(QDragEnterEvent *event)
{
}

void medSourceSettingsDragAreaWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
}

void medSourceSettingsDragAreaWidget::dragMoveEvent(QDragMoveEvent *event)
{
}

void medSourceSettingsDragAreaWidget::dropEvent(QDropEvent *event)
{

}

void medSourceSettingsDragAreaWidget::mousePressEvent(QMouseEvent *event)
{
}

/**
 * @brief Add a new source item in the list
 * 
 * @param pSource 
 */
void medSourceSettingsDragAreaWidget::addNewSourceItem(medAbstractSource * pSource)
{
    auto * pSourceParametersBox = new medSourceSettingsWidget(_pSourceLoader, pSource, _sourceInformation, this);
    sourceColumnLayout->addWidget(pSourceParametersBox);
    sourceColumnLayout->addStretch();
}