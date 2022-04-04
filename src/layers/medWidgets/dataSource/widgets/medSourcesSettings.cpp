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

medSourcesSettings::medSourcesSettings(medSourcesLoader * pSourceLoader, QWidget *parent)
    : QWidget(parent)
{
    selectedSource = nullptr; // init
    sourceLoader = pSourceLoader;

    auto * pMainLayout = new QVBoxLayout();
    this->setLayout(pMainLayout);

    //--- Fill Sources selection combobox
    auto * selectionLayout = new QVBoxLayout();

    for (auto pSourceType : pSourceLoader->sourcesTypeAvailables())
    {
        sourceTypeList.append(std::get<0>(pSourceType)); // type
        sourceNameList.append(std::get<1>(pSourceType)); // name
        sourceDescriptionList.append(std::get<2>(pSourceType)); // description
    }
    int currentIndex = 0;

    // List of source names
    auto * selectionComboCreateLayout = new QHBoxLayout();
    selectionLayout->addLayout(selectionComboCreateLayout);
    listCombobox = new QComboBox();
    listCombobox->addItems(sourceNameList);
    listCombobox->setCurrentIndex(currentIndex);
    selectionComboCreateLayout->addWidget(listCombobox);

    // Create new source button
    auto * createButton = new QPushButton(tr("Create new source"));
    createButton->setToolTip(tr("Create a new source"));
    selectionComboCreateLayout->addWidget(createButton); 

    // Description of current source
    auto * selectionDescriptionLayout = new QHBoxLayout();
    selectionLayout->addLayout(selectionDescriptionLayout);
    textEdit = new QTextEdit();
    textEdit->setHtml(sourceDescriptionList.at(currentIndex));
    textEdit->setReadOnly(true);
    textEdit->setFocusPolicy(Qt::NoFocus);
    textEdit->setMaximumHeight(50);
    selectionDescriptionLayout->addWidget(textEdit);

    pMainLayout->addLayout(selectionLayout);

    //--- Fill Sources panel
    auto * sourceLayout = new QHBoxLayout();
    pMainLayout->addLayout(sourceLayout);

    auto * sourceInformation = new QTreeWidget(); // Declare it to pass to source item widget

    // The drag&drop area handles the creation and move of source item widgets
    dragDropAreaWidget = new medSourceSettingsDragAreaWidget(pSourceLoader, sourceInformation, this);
    sourceLayout->addWidget(dragDropAreaWidget);

    //--- Sources details and actions
    auto * sourceSettingsLayout = new QVBoxLayout();
    sourceLayout->addLayout(sourceSettingsLayout);

    auto * detailsLabel = new QLabel("\nSelect a source to use actions:\n");
    sourceSettingsLayout->addWidget(detailsLabel);

    auto * setDefaultButton = new QPushButton(tr("Set as default"));
    setDefaultButton->setToolTip(tr("Set the selected source as default"));
    sourceSettingsLayout->addWidget(setDefaultButton);
    // todo get the default source and dragDropAreaWidget->setSourceToDefault(source);
    // or directly in medSourceSettingsWidget constructor

    connectButton = new QPushButton(tr("Connect"));
    connectButton->setToolTip(tr("Switch ON or OFF the selected source"));
    sourceSettingsLayout->addWidget(connectButton);

    sourceInformation->setColumnCount(2);
    sourceInformation->setAlternatingRowColors(true);
    sourceInformation->setHeaderLabels(QStringList()<<"Information"<<"Result");
    sourceInformation->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    sourceInformation->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    sourceInformation->header()->setStretchLastSection(false);
    sourceInformation->setSortingEnabled(true);
    // Init tree
    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < 3; ++i)
    {
        items.append(new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList(QString(""))));
    }
    sourceInformation->insertTopLevelItems(0, items);
    sourceInformation->setFixedHeight(98);
    sourceSettingsLayout->addWidget(sourceInformation);

    sourceSettingsLayout->addStretch();

    //--- Now that Qt widgets are set: create connections
    connect(listCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int currentIndex)
    {
        updateSelectedSourceDescription(currentIndex);
    });

    connect(createButton, &QPushButton::clicked, [=]()
    {
        createNewSource();
    });

    connect(pSourceLoader, &medSourcesLoader::sourceAdded, [=](medAbstractSource* pDataSource)
    {
        dragDropAreaWidget->addNewSourceItem(pDataSource); // Create new source item in the widget list
    });

    connect(dragDropAreaWidget, &medSourceSettingsDragAreaWidget::sourceItemChosen, [=](medAbstractSource * pSource)
    {
        updateWhenASourceIsSelected(pSource);
    });

    connect(setDefaultButton, &QPushButton::clicked, [=](bool checked)
    {
        updateWhenDefaultIsPushed();
    });

    connect(connectButton, &QPushButton::clicked, [=](bool checked)
    {
        updateWhenConnectIsPushed();
    });
}

/**
 * @brief Launch the creation of a new medAbstractSource and display it in the sources settings widget
 * 
 */
void medSourcesSettings::createNewSource()
{
    auto indexComboBox = listCombobox->currentIndex();
    QString generatedInstanceId = "";
    auto bRes = sourceLoader->createCnx(generatedInstanceId, sourceTypeList[indexComboBox]);

    if (!bRes)
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
    textEdit->setHtml(sourceDescriptionList.at(currentIndex));
}

/**
 * @brief When the user clicks on a source item, update the current source and the 
 * Connect/Disconnect button to match this source
 * 
 * @param pSource 
 */
void medSourcesSettings::updateWhenASourceIsSelected(medAbstractSource * pSource)
{
    selectedSource = pSource;

    //todo:
    // A - verify which method is used to know the connection status of a source.
    // B - if connection switched elsewhere, catch a signal to switch off/on ? (not coded yet?)
    if (selectedSource->isOnline())
    {
        connectButton->setText("Disconnect");
    }
    else
    {
        connectButton->setText("Connect");
    }
}

/**
 * @brief Send chosen default source to the source item list
 * 
 */
void medSourcesSettings::updateWhenDefaultIsPushed()
{
    if (selectedSource)
    {
        dragDropAreaWidget->setSourceToDefault(selectedSource);
    }
}

/**
 * @brief When the [dis]connect button is pushed, update the button text, apply [dis]connection, 
 * and send information about it to the source item list
 * 
 */
void medSourcesSettings::updateWhenConnectIsPushed()
{
    if (selectedSource)
    {
        if (connectButton->text() == "Connect")
        {
            selectedSource->connect(true); // TODO check signal (not coded yet)
            dragDropAreaWidget->setConnectionIcon(selectedSource, true);
            connectButton->setText("Disconnect");
        }
        else
        {
            selectedSource->connect(false);
            dragDropAreaWidget->setConnectionIcon(selectedSource, false);
            connectButton->setText("Connect");
        }
    }
}
