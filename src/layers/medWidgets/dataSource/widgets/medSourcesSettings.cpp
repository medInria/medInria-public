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
#include "medSourcesSettings.h"

#include <medSourceSettingsWidget.h>
#include <medSettingsManager.h>

#include <QVBoxLayout>

medSourcesSettings::medSourcesSettings(medSourcesLoader * pSourceLoader, QWidget *parent)
    : QWidget(parent)
{
    m_sourceLoader = pSourceLoader;

    auto * pMainLayout = new QVBoxLayout();
    this->setLayout(pMainLayout);

    //--- Fill Sources selection combobox
    auto * selectionLayout = new QVBoxLayout();

    auto sourcesTypeAvailables = m_sourceLoader->sourcesTypeAvailables();
    QStringList sourcesTypesNames;
    for (auto pSourceType : sourcesTypeAvailables)
    {
        sourcesTypesNames.append(std::get<1>(pSourceType)); // name
    }

    // List of type of source names
    auto * selectionComboCreateLayout = new QHBoxLayout();
    selectionLayout->addLayout(selectionComboCreateLayout);
    m_sourceTypeCombobox = new QComboBox();
    m_sourceTypeCombobox->addItems(sourcesTypesNames);
    m_sourceTypeCombobox->setCurrentIndex(0);
    selectionComboCreateLayout->addWidget(m_sourceTypeCombobox);

    // Create new source button
    auto * createButton = new QPushButton(tr("Create new source"));
    createButton->setToolTip(tr("Create a new source"));
    selectionComboCreateLayout->addWidget(createButton); 

    // Description of current type of source
    auto * selectionDescriptionLayout = new QHBoxLayout();
    selectionLayout->addLayout(selectionDescriptionLayout);
    m_sourceDescriptionWidget = new QTextEdit();
    if (!sourcesTypeAvailables.isEmpty())
    {
        m_sourceDescriptionWidget->setHtml(std::get<1>(sourcesTypeAvailables[0]));
    }
    m_sourceDescriptionWidget->setReadOnly(true);
    m_sourceDescriptionWidget->setFocusPolicy(Qt::NoFocus);
    m_sourceDescriptionWidget->setMaximumHeight(50);
    selectionDescriptionLayout->addWidget(m_sourceDescriptionWidget);

    pMainLayout->addLayout(selectionLayout);

    //--- Fill Sources panel
    auto * sourceLayout = new QHBoxLayout();
    pMainLayout->addLayout(sourceLayout);


    // The drag&drop area handles the creation and move of source item widgets
    m_sourceListWidget = new QListWidget(this);
    sourceLayout->setAlignment(Qt::AlignTop);
    sourceLayout->addWidget(m_sourceListWidget);


    m_sourceListWidget->setAcceptDrops(true);
    m_sourceListWidget->setDragDropMode(QAbstractItemView::InternalMove);
    m_sourceListWidget->setAutoScroll(true);
    //m_sourceListWidget->setFixedSize(400, 550);
    m_sourceListWidget->setResizeMode(QListView::Adjust);
    m_sourceListWidget->setMinimumWidth(400);
    // Change the color of the background when an item is selected to drag&drop
    m_sourceListWidget->setStyleSheet("QListWidget::item:selected {background:gray;}");

    for (auto pSource : pSourceLoader->sourcesList())
    {
        sourceCreated(pSource);
    }

    auto * pDefaultWorkingSource = m_sourceLoader->getDefaultWorkingSource();
    if (pDefaultWorkingSource)
    {
        auto *pItem = m_sourceToItem.value(pDefaultWorkingSource);
        auto *pWidget = static_cast<medSourceSettingsWidget*>(m_sourceListWidget->itemWidget(pItem));
        pWidget->setToDefault(true);
    }

    //--- Sources details and actions
    auto * sourceSettingsLayout = new QVBoxLayout();
    sourceLayout->addLayout(sourceSettingsLayout);

    auto * detailsLabel = new QLabel("\nSelect a source to use actions:\n");
    sourceSettingsLayout->addWidget(detailsLabel);

    m_setDefaultButton = new QPushButton(tr("Set as default"));
    m_setDefaultButton->setToolTip(tr("Set the selected source as default"));
    sourceSettingsLayout->addWidget(m_setDefaultButton);

    m_connectButton = new QPushButton(tr("Connect"));
    m_connectButton->setToolTip(tr("Switch ON or OFF the selected source"));
    sourceSettingsLayout->addWidget(m_connectButton);

    m_sourceInformation = new QTreeWidget();
    m_sourceInformation->setColumnCount(2);
    m_sourceInformation->setAlternatingRowColors(true);
    m_sourceInformation->setHeaderLabels(QStringList()<<"Information"<<"Result");
    m_sourceInformation->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_sourceInformation->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_sourceInformation->header()->setStretchLastSection(false);
    // Init tree
    for (int i = 0; i < 3; ++i)
    {
        m_sourceInformation->insertTopLevelItem(i, new QTreeWidgetItem(m_sourceInformation, {"",""}));
    }
    m_sourceInformation->setFixedHeight(98);
    sourceSettingsLayout->addWidget(m_sourceInformation);

    m_removeButton = new QPushButton(tr("Remove source"));
    m_removeButton->setToolTip(tr("Remove the selected source"));
    sourceSettingsLayout->addWidget(m_removeButton);

    sourceSettingsLayout->addStretch();

    //--- Now that Qt widgets are set: create connections
    connect(m_sourceTypeCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &medSourcesSettings::updateSelectedSourceDescription);
    connect(createButton,       &QPushButton::clicked, this, &medSourcesSettings::createSource);
    connect(m_setDefaultButton, &QPushButton::clicked, this, &medSourcesSettings::setAsDefault ); // Change default source
    connect(m_removeButton,     &QPushButton::clicked, this, &medSourcesSettings::removeSource ); // Ask to remove source
    connect(m_sourceLoader,     &medSourcesLoader::sourceAdded,  this, &medSourcesSettings::sourceCreated);   // Signal to slot to indicate a new source
    connect(m_sourceLoader,     &medSourcesLoader::sourceRemoved, this, &medSourcesSettings::sourceRemoved);   // Signal to slot to indicate a new source
    connect(m_sourceListWidget, &QListWidget::currentRowChanged, this, &medSourcesSettings::selectedSourceChange);
    connect(m_sourceListWidget->model(), &QAbstractItemModel::rowsMoved, this, &medSourcesSettings::sourceMoved);


    if (m_sourceListWidget->count())
    {
        m_sourceListWidget->setCurrentRow(0);
    }
    else
    {
        selectedSourceChange(-1);
    }

    m_sourceListWidget->setAutoScroll(true);

    QWidget * pConfFileWidget = new QWidget();
    QHBoxLayout * pConfFileLayout = new QHBoxLayout();
    QLabel * pPathLabel = new QLabel("Sources configuration file");
    QLineEdit * pPathLineEdit = new QLineEdit();
    QPushButton * pPathExplorButton = new QPushButton("...");
    //QFileDialog
    pConfFileLayout->addWidget(pPathLabel);
    pConfFileLayout->addWidget(pPathLineEdit);
    pConfFileLayout->addWidget(pPathExplorButton);
    pConfFileWidget->setLayout(pConfFileLayout);
    pMainLayout->addWidget(pConfFileWidget);
}

void medSourcesSettings::removeSource()
{
    auto * pItem = m_sourceListWidget->currentItem();    
    if (pItem)
    {
        auto *pSource = m_sourceToItem.key(pItem);
        m_sourceLoader->removeCnx(pSource->getInstanceId());
    }
}

void medSourcesSettings::setAsDefault()
{
    auto sources = m_sourceLoader->sourcesList();
    auto * previousDefault = m_sourceLoader->getDefaultWorkingSource();
    auto * currentSource = sources[m_sourceListWidget->currentRow()];
    if (currentSource)
    {
        if (m_sourceLoader->setDefaultWorkingSource(currentSource->getInstanceId()))
        {
            m_removeButton->setDisabled(true);
            if (previousDefault)
            {
                auto *pItem = m_sourceToItem.value(previousDefault);
                auto *pWidget = static_cast<medSourceSettingsWidget*>(m_sourceListWidget->itemWidget(pItem));
                pWidget->setToDefault(false);
            }
            auto *pItem = m_sourceToItem.value(currentSource);
            auto *pWidget = static_cast<medSourceSettingsWidget*>(m_sourceListWidget->itemWidget(pItem));
            pWidget->setToDefault(true);
            m_setDefaultButton->setDisabled(true);
        }
    }
}

/**
 * @brief This method fills the information widget with info from this selected source
 *
 */
void medSourcesSettings::updateSourceInformation(medAbstractSource * pi_pSource)
{
    m_sourceInformation->setVisible(false);
    QTreeWidgetItem *hasCache = m_sourceInformation->topLevelItem(2);;
    hasCache->setText(0, "Has cache: ");
    hasCache->setText(1, pi_pSource->isCached() ? "true" : "false");

    QTreeWidgetItem *isLocal = m_sourceInformation->topLevelItem(1);
    isLocal->setText(0, "Is local: ");
    isLocal->setText(1, pi_pSource->isLocal() ? "true" : "false");

    QTreeWidgetItem *isWritable = m_sourceInformation->topLevelItem(0);
    isWritable->setText(0, "Is writable: ");
    isWritable->setText(1, pi_pSource->isWritable() ? "true" : "false");
    m_sourceInformation->setVisible(true);
}

/**
 * @brief Launch the creation of a new medAbstractSource and display it in the sources settings widget
 * 
 */
void medSourcesSettings::createSource()
{
    auto indexComboBox = m_sourceTypeCombobox->currentIndex();
    QString generatedInstanceId = "";
    bool bOk = m_sourceLoader->createCnx(generatedInstanceId, std::get<0>(m_sourceLoader->sourcesTypeAvailables()[indexComboBox]));

    if (!bOk)
    {
        qDebug()<<"A problem occurred creating a new source.";
    }
}

/**
 * @brief Display the description of the chosen source type in the GUI
 * 
 * @param currentIndex 
 */
void medSourcesSettings::updateSelectedSourceDescription(int currentIndex)
{
    m_sourceDescriptionWidget->setHtml(std::get<1>(m_sourceLoader->sourcesTypeAvailables()[currentIndex]));
}










void medSourcesSettings::selectedSourceChange(int pi_index)
{
    auto *pItem = m_sourceListWidget->item(pi_index);
    if (pItem != nullptr)
    {
        auto *pWidget = static_cast<medSourceSettingsWidget*>(m_sourceListWidget->itemWidget(pItem));
        auto *pSource = pWidget->getSource();
        bool bIsDefaultWorkingSource = pSource == m_sourceLoader->getDefaultWorkingSource();

        updateSourceInformation(pSource);
        updateConnectButton(pSource);
        m_removeButton->setDisabled(bIsDefaultWorkingSource);
        m_setDefaultButton->setDisabled(bIsDefaultWorkingSource);
    }
    else
    {
        m_connectButton->setDisabled(true);
        m_setDefaultButton->setDisabled(true);
        m_removeButton->setDisabled(true);
    }
}

void medSourcesSettings::updateConnectButton(medAbstractSource * pi_pSource)
{
    connectButtonUpdateText();
    disconnect(this, SLOT(updateConnectButton));
    disconnect(m_connectButton);
    connect(pi_pSource, &medAbstractSource::connectionStatus, this, &medSourcesSettings::connectButtonUpdateText);
    connect(m_connectButton, &QPushButton::clicked, this, &medSourcesSettings::updateSourceConnection);
}

void medSourcesSettings::connectButtonUpdateText()
{
    auto * pItem = m_sourceListWidget->currentItem();
    if (pItem)
    {
        auto *pSource = m_sourceToItem.key(pItem);
        if (pSource->isOnline())
        {
            m_connectButton->setText("Disconnect");
        }
        else
        {
            m_connectButton->setText("Connect");
        }
    }
}


void medSourcesSettings::sourceCreated(medAbstractSource * pi_pSource)
{
    QListWidgetItem *widgetItem = new QListWidgetItem();
    auto * newSourceWidgetItem = new medSourceSettingsWidget(pi_pSource, widgetItem, this);
    m_sourceToItem[pi_pSource] = widgetItem;
    m_sourceListWidget->addItem(widgetItem);
    m_sourceListWidget->setItemWidget(widgetItem, newSourceWidgetItem);

    // Initial size of item
    QSize initialSize = newSourceWidgetItem->size();
    newSourceWidgetItem->saveInitialSize(initialSize);
    widgetItem->setSizeHint(initialSize);

    m_sourceListWidget->setSpacing(2);

    m_sourceListWidget->setCurrentItem(widgetItem);

    connect(newSourceWidgetItem, &medSourceSettingsWidget::sourceRename, m_sourceLoader, &medSourcesLoader::renameSource);
    connect(m_sourceLoader, &medSourcesLoader::sourcesUpdated, newSourceWidgetItem, &medSourceSettingsWidget::titleChanged);
    connect(m_sourceListWidget, &QListWidget::currentItemChanged, newSourceWidgetItem, &medSourceSettingsWidget::currentItemChanged);
    connect(newSourceWidgetItem, &medSourceSettingsWidget::minimizationAsked, [=](bool isMinimized)
    {
        switchMinimization(newSourceWidgetItem, isMinimized);
    });
}

void medSourcesSettings::sourceRemoved(medAbstractSource * pi_pSource)
{
    delete m_sourceToItem.take(pi_pSource);
}

void medSourcesSettings::updateSourceConnection()
{
    auto *pItem = m_sourceListWidget->currentItem();
    if (pItem != nullptr)
    {
        auto *pSource = m_sourceToItem.key(pItem);
        pSource->connect(!pSource->isOnline());
    }
}

void medSourcesSettings::sourceMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    m_sourceLoader->changeSourceOrder(start, row);
}
