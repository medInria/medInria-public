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
#include <medSourcesSettingsHandlerWidget.h>
#include <medSettingsManager.h>

#include <QComboBox>
#include <QFileDialog>
#include <QTextEdit>
#include <QVBoxLayout>

/**
 * @brief medSourcesSettings constructor.
 * 
 * @param [in] pi_pSourceLoader is pointer to medSourcesLoader instance that the widget represents.
 * @param [in] pi_parent is the parent widget.
 */
medSourcesSettings::medSourcesSettings(medSourcesLoader * pi_pSourceLoader, QWidget *pi_parent)
    : QWidget(pi_parent)
{
    m_sourceLoader = pi_pSourceLoader;

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
    m_sourceListWidget->setResizeMode(QListView::Adjust);
    m_sourceListWidget->setMinimumWidth(400);
    // Change the color of the background when an item is selected to drag&drop
    m_sourceListWidget->setStyleSheet("QListWidget::item:selected {background:gray;}");

    for (auto pSource : pi_pSourceLoader->sourcesList())
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

    m_settingsHandlerWidget = new medSourcesSettingsHandlerWidget(this);
    sourceLayout->addWidget(m_settingsHandlerWidget);
    //--- Now that Qt widgets are set: create connections
    connect(m_sourceTypeCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &medSourcesSettings::updateSelectedSourceDescription);
    connect(createButton,       &QPushButton::clicked, this, &medSourcesSettings::createSource);
    connect(m_sourceLoader,     &medSourcesLoader::sourceAdded,  this, &medSourcesSettings::sourceCreated);   // Signal to slot to indicate a new source
    connect(m_sourceLoader,     &medSourcesLoader::sourceRemoved, this, &medSourcesSettings::sourceRemoved);   // Signal to slot to indicate a new source
    connect(m_sourceLoader,      &medSourcesLoader::sourceHidden, this, &medSourcesSettings::hideSource);
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

    pMainLayout->addWidget(createConfPathWidget());
}



/**
 * @brief Slot to handle a change selected source.
 * @details This slot must be connected to m_sourceListWidget.
 * @param [in] pi_index of the new selected source.
 */
void medSourcesSettings::selectedSourceChange(int pi_index)
{
    auto *pItem = m_sourceListWidget->item(pi_index);
    if (pItem != nullptr)
    {
        auto *pSource = m_sourceToItem.key(pItem);
        bool bIsDefaultWorkingSource = pSource == m_sourceLoader->getDefaultWorkingSource();

        m_settingsHandlerWidget->sourceChange(pSource, bIsDefaultWorkingSource);
    }
    else
    {
        m_settingsHandlerWidget->sourceChange(nullptr, false);
    }
}

void medSourcesSettings::hideSource(QString sourceId, bool hide)
{
    auto * pItem = m_sourceListWidget->currentItem();
    if (pItem)
    {
        auto *pSource = m_sourceToItem.key(pItem);
        if (pSource->getInstanceId() == sourceId)
        {
            m_settingsHandlerWidget->updateHideButton(pSource);
        }
    }
}

/**
 * @brief Slot to handle a move of source.
 * @details This slot must be connected to the QAbstractItemModel of m_sourceListWidget.
 * @details Parameters and there names are made to perfect match with QAbstractItemModel::rowsMoved
 * @param [in] parent QModelIndex of the first selected source (not used here).
 * @param [in] start index order as integer of moved source.
 * @param [in] end (not used here).
 * @param [in] destination QModelIndex of the destination neighbor source (not used here).
 * @param [in] row future index order as integer of moved source.
 */
void medSourcesSettings::sourceMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    m_sourceLoader->changeSourceOrder(start, row);
}



/**
 * @brief Launch the creation of a new medAbstractSource.
 * @details This is the slot to call by the create button.
 */
void medSourcesSettings::createSource()
{
    auto indexComboBox = m_sourceTypeCombobox->currentIndex();
    QString generatedInstanceId = "";
    if (!m_sourceLoader->createCnx(generatedInstanceId, std::get<0>(m_sourceLoader->sourcesTypeAvailables()[indexComboBox])))
    {
        qDebug()<<"A problem occurred creating a new source.";
    }
}

/**
* @brief Launch the deletion of current medAbstractSource.
* @details This is the slot to call by the remove button.
*/
void medSourcesSettings::removeSource()
{
    auto * pItem = m_sourceListWidget->currentItem();    
    if (pItem)
    {
        auto *pSource = m_sourceToItem.key(pItem);
        m_sourceLoader->removeCnx(pSource->getInstanceId());
    }
}

/**
 * @brief Change the default working source by the current and update internals widgets.
 * @details This is the slot to call by the default button.
 * @return True only if the change can be done.
 */
bool medSourcesSettings::setAsDefault()
{
    bool bRes = false;

    auto sources = m_sourceLoader->sourcesList();
    auto * previousDefault = m_sourceLoader->getDefaultWorkingSource();
    auto * currentSource = sources[m_sourceListWidget->currentRow()];
    if (currentSource)
    {
        bRes = m_sourceLoader->setDefaultWorkingSource(currentSource->getInstanceId());
        if (bRes)
        {
            if (previousDefault)
            {
                auto *pItem = m_sourceToItem.value(previousDefault);
                auto *pWidget = static_cast<medSourceSettingsWidget*>(m_sourceListWidget->itemWidget(pItem));
                pWidget->setToDefault(false);
            }
            auto *pItem = m_sourceToItem.value(currentSource);
            auto *pWidget = static_cast<medSourceSettingsWidget*>(m_sourceListWidget->itemWidget(pItem));
            pWidget->setToDefault(true);
            m_settingsHandlerWidget->setAsDefault();
        }
    }

    return bRes;
}

bool medSourcesSettings::setAsInvisible()
{
    bool bRes = false;

    auto sources = m_sourceLoader->sourcesList();
    auto * currentSource = sources[m_sourceListWidget->currentRow()];
    if (currentSource)
    {
        auto sourceId = currentSource->getInstanceId();
        bool bHide = m_sourceLoader->isInvisible(sourceId);
        m_sourceLoader->hideSource(sourceId, !bHide);  
        bRes = true;
    }

    return bRes;
}



/**
 * @brief Display a new source and select it.
 * @details This is the slot to call by medSourceLoader.
 */
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
    connect(pi_pSource, &medAbstractSource::connectionStatus, newSourceWidgetItem, &medSourceSettingsWidget::switchConnectionIcon);
    connect(m_sourceLoader, &medSourcesLoader::sourcesUpdated, newSourceWidgetItem, &medSourceSettingsWidget::titleChanged);
    connect(m_sourceListWidget, &QListWidget::currentItemChanged, newSourceWidgetItem, &medSourceSettingsWidget::currentItemChanged);
}

/**
 * @brief Refresh the displaying of sources when a source is removed.
 * @details This is the slot to call by medSourceLoader.
 */
void medSourcesSettings::sourceRemoved(medAbstractSource * pi_pSource)
{
    delete m_sourceToItem.take(pi_pSource);
}

/**
 * @brief Refresh the displaying of current source.
 * @details This is the slot to call by medSourceLoader.
 */
void medSourcesSettings::updateSourceConnection()
{
    auto *pItem = m_sourceListWidget->currentItem();
    if (pItem != nullptr)
    {
        auto *pSource = m_sourceToItem.key(pItem);
        pSource->connect(!pSource->isOnline());
    }
}



/**
 * @brief Display the description of the chosen source type in the GUI.
 * @param [in] pi_index of current selected type of source.
 */
void medSourcesSettings::updateSelectedSourceDescription(int pi_index)
{
    m_sourceDescriptionWidget->setHtml(std::get<1>(m_sourceLoader->sourcesTypeAvailables()[pi_index]));
}

/**
 * @brief Create sub widget in charge of path to file to describe each connections.
 * @details Contains a label, a lineedit path, a button to explore file tree.
 */
QWidget * medSourcesSettings::createConfPathWidget()
{
    QWidget * pConfFileWidget = new QWidget();

    QHBoxLayout * pConfFileLayout = new QHBoxLayout();
    QLabel * pPathLabel = new QLabel("Config's directory");
    QLineEdit * pPathLineEdit = new QLineEdit(QDir::toNativeSeparators(m_sourceLoader->getPath()));
    QPushButton * pPathExplorButton = new QPushButton("...");

    pPathLabel->setToolTip(tr("Directory containing " MED_DATASOURCES_FILENAME));

    connect(pPathExplorButton, &QPushButton::clicked, [=]()
    {
        QString sourcesConfFilePath = QFileDialog::getExistingDirectory(this, tr("Select sources configuration directory"), m_sourceLoader->getPath());

        if (!sourcesConfFilePath.isEmpty() && m_sourceLoader->setPath(sourcesConfFilePath))
        {
                pPathLineEdit->setText(QDir::toNativeSeparators(sourcesConfFilePath));
        }
    });
    connect(pPathLineEdit, &QLineEdit::editingFinished, [=]()
    {
        if (!m_sourceLoader->setPath(QDir::fromNativeSeparators(pPathLineEdit->text())))
        {
            pPathLineEdit->setText(QDir::toNativeSeparators(m_sourceLoader->getPath()));
        }
    });

    //QFileDialog
    pConfFileLayout->addWidget(pPathLabel);
    pConfFileLayout->addWidget(pPathLineEdit);
    pConfFileLayout->addWidget(pPathExplorButton);
    pConfFileWidget->setLayout(pConfFileLayout);

    return pConfFileWidget;
}

