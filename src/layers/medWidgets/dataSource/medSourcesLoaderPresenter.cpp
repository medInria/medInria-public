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

#include "medSourcesLoaderPresenter.h"

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTextEdit>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <medAbstractParameterPresenter.h>

#include <QListWidget>
#include <QListWidgetItem>

class medSourcesLoaderPresenterPrivate
{
public:
    medSourcesLoader * pSourceLoader;
};

medSourcesLoaderPresenter::medSourcesLoaderPresenter(medSourcesLoader * parent)
    : d(new medSourcesLoaderPresenterPrivate())
{
    d->pSourceLoader = parent;
}

medSourcesLoaderPresenter::~medSourcesLoaderPresenter()
{
    delete d;
}

QWidget * medSourcesLoaderPresenter::buildWidget()
{
    QWidget * pWidgetRes = new QWidget;
    auto * pMainLayout = new QVBoxLayout();

    //--- Fill Sources selection combobox
    auto * selectionLayout = new QVBoxLayout();

    QStringList sourceTypeList;
    QStringList sourceNameList;
    QStringList sourceDescriptionList;
    for (auto pSourceType : d->pSourceLoader->sourcesTypeAvailables())
    {
        sourceTypeList.append(std::get<0>(pSourceType)); // type
        sourceNameList.append(std::get<1>(pSourceType)); // name
        sourceDescriptionList.append(std::get<2>(pSourceType)); // description
    }
    int currentIndex = 0;

    // List of source names
    auto * selectionComboCreateLayout = new QHBoxLayout();
    selectionLayout->addLayout(selectionComboCreateLayout);
    auto * listCombobox = new QComboBox();
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
    QTextEdit * textEdit = new QTextEdit();
    textEdit->setHtml(sourceDescriptionList.at(currentIndex));
    textEdit->setReadOnly(true);
    textEdit->setFocusPolicy(Qt::NoFocus);
    textEdit->setMaximumHeight(100);
    selectionDescriptionLayout->addWidget(textEdit);

    pMainLayout->addLayout(selectionLayout);

    //--- Fill Sources panel
    auto * sourceInformation = new QTreeWidget(); // Declare it to pass to source item widget

    // The drag&drop area handles the creation and move of source item widgets
    auto * dragDropAreaWidget = new medSourceSettingsDragAreaWidget(d->pSourceLoader, sourceInformation, pWidgetRes);
    pMainLayout->addWidget(dragDropAreaWidget);

    //--- Sources details
    auto * sourceSettingsLayout = new QVBoxLayout();
    pMainLayout->addLayout(sourceSettingsLayout);

    sourceInformation->setColumnCount(2);
    sourceInformation->setAlternatingRowColors(true);
    sourceInformation->setHeaderLabels(QStringList()<<"Select a source to display details"<<"Result");
    sourceInformation->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    sourceInformation->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    sourceInformation->header()->setStretchLastSection(false);
    sourceInformation->setSortingEnabled(true);

    // Init tree
    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < 4; ++i)
    {
        items.append(new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList(QString(""))));
    }
    sourceInformation->insertTopLevelItems(0, items);
    sourceInformation->setFixedHeight(98);
    sourceSettingsLayout->addWidget(sourceInformation);

    //--- Save
    auto * saveButton = new QPushButton(tr("Save"));
    saveButton->setToolTip(tr("Save current settings"));
    sourceSettingsLayout->addWidget(saveButton);

    sourceSettingsLayout->addStretch();

    //--- Now that Qt widgets are set: create connections
    connect(listCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int currentIndex)
    {
        textEdit->setHtml(sourceDescriptionList.at(currentIndex));
    });

    auto * sourceLoader = d->pSourceLoader;
    connect(createButton, &QPushButton::clicked, [=]()
    {
        createNewSource(listCombobox, sourceTypeList, sourceLoader, dragDropAreaWidget);
    });

    // Catch the signal emitted when a new source is added in the source loader
    connect(sourceLoader, &medSourcesLoader::sourceAdded, [=](medAbstractSource* pDataSource)
    {
        dragDropAreaWidget->addNewSourceItem(pDataSource);
    });

    connect(saveButton, &QPushButton::clicked, [=]()
    {
        saveSourcesSettings();
    });

    pWidgetRes->setLayout(pMainLayout);
    return pWidgetRes;
}

/**
 * @brief Launch the creation of the new medAbstractSource and display it in the sources settings widget 
 * 
 * @param listCombobox 
 * @param sourceLoader 
 */
void medSourcesLoaderPresenter::createNewSource(QComboBox * listCombobox,
                                                QStringList sourceTypeList,
                                                medSourcesLoader * sourceLoader,
                                                medSourceSettingsDragAreaWidget * dragDropAreaWidget)
{
        auto indexComboBox = listCombobox->currentIndex();
        QString generatedInstanceId = "";
        auto bRes = sourceLoader->createCnx(generatedInstanceId, sourceTypeList[indexComboBox]);

        if (!bRes)
        {
            qDebug()<<"A problem occured creating a new source.";
        }
}

/**
 * @brief Save current sources settings
 * 
 */
void medSourcesLoaderPresenter::saveSourcesSettings()
{
    qDebug()<<"### medSourcesLoaderPresenter::saveSourcesSettings";
    // TODO save (saveToDisk ?) (not coded yet)
    // Save settings (order, default, connected ?)
}