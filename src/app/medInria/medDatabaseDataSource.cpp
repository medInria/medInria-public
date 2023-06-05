/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseDataSource.h"

#include <QLabel>
#include <QLineEdit>

#include <medApplicationContext.h>
#include <medDataManager.h>
#include <medSourcesLoader.h>
#include <medDataHub.h>
#include <medSourceModelPresenter.h>
#include <medDataHubPresenter.h>
#include <medSourcesWidget.h>
#include <medToolBox.h>
#include <medVirtualRepresentationWidget.h>

#include <medVirtualRepresentationPresenter.h>

class medDatabaseDataSourcePrivate
{
public:
    QPointer<QWidget> mainWidget;

    medSourcesWidget *compactView;
    medVirtualRepresentationWidget *virtualWidget;

    QList<medToolBox*> toolBoxes;
    medDataHubPresenter *multiSources_tree;
    int currentSource;
};

medDatabaseDataSource::medDatabaseDataSource( QWidget* parent ): medAbstractDataSource(parent), d(new medDatabaseDataSourcePrivate)
{
    d->multiSources_tree = new medDataHubPresenter(medDataHub::instance());
    d->currentSource = 0;
}

medDatabaseDataSource::~medDatabaseDataSource()
{
    qDeleteAll(d->toolBoxes.begin(), d->toolBoxes.end());
    d->toolBoxes.clear();

    delete d->model;

    delete d;
    d = nullptr;
}

QWidget* medDatabaseDataSource::buildSourcesBrowser()
{
    auto *widget = new QWidget();

    auto *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QStackedWidget *mainViewWidgetRes = d->multiSources_tree->buildBrowser();
    connect(this, SIGNAL(changeSource(int)), mainViewWidgetRes, SLOT(setCurrentIndex(int)));

    layout->addWidget(mainViewWidgetRes);


    widget->setLayout(layout);

    return widget;
}

QWidget* medDatabaseDataSource::buildSourcesTreeViewList()
{
    QWidget *compactViewWidgetRes = new QWidget();

    QVBoxLayout *layout = new QVBoxLayout();

    auto filterLineEdit = new QLineEdit();
    filterLineEdit->setMaximumHeight(20);
    filterLineEdit->setPlaceholderText("Search...");
    connect(filterLineEdit, SIGNAL(textChanged(const QString &)), d->multiSources_tree, SIGNAL(filterProxy(const QString &)));

    d->compactView = d->multiSources_tree->buildTree();
    d->compactView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    connect(d->compactView, SIGNAL(openOnDoubleClick(medDataIndex)), this, SIGNAL(openOnDoubleClick(medDataIndex)));
   
    d->virtualWidget = medVirtualRepresentationPresenter(medApplicationContext::instance()->getVirtualRepresentation()).buildTree();

    layout->addWidget(d->virtualWidget , 0, Qt::AlignTop);
    layout->addWidget(filterLineEdit, 0, Qt::AlignTop);
    layout->addWidget(d->compactView, 0, Qt::AlignTop);
    layout->addStretch(2);

    compactViewWidgetRes->setLayout(layout);

    return compactViewWidgetRes;
}

QWidget* medDatabaseDataSource::buildSourceListSelectorWidget()
{
    auto *widget = new QWidget();

    auto *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QListWidget *listSources = d->multiSources_tree->buildSourceList();
    connect(listSources, SIGNAL(currentRowChanged(int)), this, SLOT(currentSourceChanged(int)));

    layout->addWidget(listSources);

    widget->setLayout(layout);

    return widget;
}

void medDatabaseDataSource::currentSourceChanged(int current)
{
    d->currentSource = current;
    emit changeSource(current);
}

QString medDatabaseDataSource::tabName()
{
    return tr("Data Sources");
}

QList<medToolBox*> medDatabaseDataSource::getToolBoxes()
{
    if(d->toolBoxes.isEmpty())
    {        
        auto * pToolBox = new medToolBox();
        auto * pFiltersStackBySource = d->multiSources_tree->buildFilters();
        connect(this, SIGNAL(changeSource(int)), pFiltersStackBySource, SLOT(setCurrentIndex(int)));
        pToolBox->addWidget(pFiltersStackBySource);
        d->toolBoxes.push_back(pToolBox);
    }
    return d->toolBoxes;
}

QWidget * medDatabaseDataSource::mainViewWidget()
{
    return buildSourcesBrowser();
}

QWidget * medDatabaseDataSource::sourceSelectorWidget()
{
    return buildSourceListSelectorWidget();
}

QString medDatabaseDataSource::description() const
{
    return QString();
}
