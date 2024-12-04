/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medFileSystemDataSource.h"

#include <dtkGuiSupport/dtkFinder.h>

#include <medSettingsManager.h>
#include <medDataManager.h>

class medFileSystemDataSourcePrivate
{
public:
    QWidget *filesystemWidget;
    dtkFinder *finder;
    dtkFinderPathBar *path;
    dtkFinderSideView *side;
    dtkFinderToolBar *toolbar;
    QList<medToolBox*> toolBoxes;
    //medActionsToolBox* actionsToolBox;
    QLabel * infoText;
};

medFileSystemDataSource::medFileSystemDataSource( QWidget* parent ): medAbstractDataSource(parent), d(new medFileSystemDataSourcePrivate)
{
    d->filesystemWidget = new QWidget();

    // Get previous path or default one
    QString defaultPath = medSettingsManager::instance().value("Browser", "default_pipeline_import_path").toString();

    if (defaultPath.isEmpty())
    {
        defaultPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    }

    d->finder = new dtkFinder (d->filesystemWidget);
    d->finder->allowFileBookmarking(false);
    d->finder->allowMultipleSelection(true);
    d->finder->setPath(defaultPath);

    d->path = new dtkFinderPathBar (d->filesystemWidget);
    d->path->setPath(defaultPath);

    d->toolbar = new dtkFinderToolBar (d->filesystemWidget);
    d->toolbar->setObjectName("toolbarWidget");
    d->toolbar->setPath(defaultPath);

    d->infoText = new QLabel(d->filesystemWidget);
    d->infoText->setObjectName("fileSystemInfoText");
    d->infoText->setText("");
    d->infoText->setVisible(false);
    d->infoText->setTextFormat(Qt::RichText);


    //d->actionsToolBox = new medActionsToolBox(0, true);
    //d->toolBoxes.push_back(d->actionsToolBox);

    d->side = new dtkFinderSideView;

    QAction *importAction = new QAction(tr("Import"), this);
    importAction->setIconVisibleInMenu(true);
    importAction->setIcon(QIcon::fromTheme("download"));

    QAction *viewAction = new QAction(tr("View"), this);
    viewAction->setIconVisibleInMenu(true);
    viewAction->setIcon(QIcon::fromTheme("eye_opened"));

    d->finder->addContextMenuAction(importAction);
    d->finder->addContextMenuAction(viewAction);
    
    connect(importAction,      SIGNAL(triggered()), this, SLOT(onFileSystemImportRequested()));
    connect(viewAction,        SIGNAL(triggered()), this, SLOT(onFileSystemViewRequested()));

    QVBoxLayout *filesystem_layout = new QVBoxLayout(d->filesystemWidget);
    QHBoxLayout *toolbar_layout = new QHBoxLayout();
    QWidget * toolbarWidget = new QWidget;
    toolbarWidget->setLayout(toolbar_layout);
    toolbarWidget->setObjectName("toolbarWidget");

    toolbar_layout->setContentsMargins(0, 0, 0, 0);
    toolbar_layout->setSpacing(0);
    toolbar_layout->addWidget (d->toolbar);
    toolbar_layout->addWidget (d->path);

    filesystem_layout->setContentsMargins(0, 0, 0, 0);
    filesystem_layout->setSpacing(0);
    filesystem_layout->addWidget(toolbarWidget);
    filesystem_layout->addWidget(d->finder);
    filesystem_layout->addWidget(d->infoText);

    connect(d->finder, SIGNAL(changed(QString)), d->path, SLOT(setPath(QString)));
    connect(d->finder, SIGNAL(changed(QString)), d->side, SLOT(setPath(QString)));
    connect(d->finder, SIGNAL(changed(QString)), d->toolbar, SLOT(setPath(QString)));
    connect(d->finder, SIGNAL(fileDoubleClicked(const QString&)), this, SLOT(onFileDoubleClicked(const QString&)));
    connect(d->finder, SIGNAL(fileClicked(const QFileInfo&)), this, SLOT(onFileClicked(const QFileInfo&)));
    connect(d->finder, SIGNAL(nothingSelected()), this, SLOT(onNothingSelected()));

    connect(d->finder, SIGNAL(listView()), d->toolbar, SLOT(onListView()));
    connect(d->finder, SIGNAL(treeView()), d->toolbar, SLOT(onTreeView()));
    connect(d->finder, SIGNAL(listView()), this, SLOT(saveListViewSettings()));
    connect(d->finder, SIGNAL(treeView()), this, SLOT(saveTreeViewSettings()));
    connect(d->finder, SIGNAL(showHiddenFiles(bool)), d->toolbar, SLOT(onShowHiddenFiles(bool)));
    connect(d->finder, SIGNAL(showHiddenFiles(bool)), this, SLOT(saveHiddenFilesSettings(bool)));

    connect(d->path, SIGNAL(changed(QString)), d->finder, SLOT(setPath(QString)));
    connect(d->path, SIGNAL(changed(QString)), d->side, SLOT(setPath(QString)));
    connect(d->path, SIGNAL(changed(QString)), d->toolbar, SLOT(setPath(QString)));

    connect(d->side, SIGNAL(changed(QString)), d->finder, SLOT(setPath(QString)));
    connect(d->side, SIGNAL(changed(QString)), d->path, SLOT(setPath(QString)));
    connect(d->side, SIGNAL(changed(QString)), d->toolbar, SLOT(setPath(QString)));

    connect(d->finder, SIGNAL(bookmarked(QString)), d->side, SLOT(addBookmark(QString)));

    connect (d->toolbar, SIGNAL(changed(QString)), d->finder, SLOT(setPath(QString)));
    connect (d->toolbar, SIGNAL(changed(QString)), d->path, SLOT(setPath(QString)));

    connect (d->toolbar, SIGNAL(treeView()), d->finder, SLOT(switchToTreeView()));
    connect (d->toolbar, SIGNAL(listView()), d->finder, SLOT(switchToListView()));

    //connect(d->finder, SIGNAL(selectionChanged(const QStringList&)), d->actionsToolBox, SLOT(selectedPathsChanged(const QStringList&)));
    //
    //connect(d->actionsToolBox, SIGNAL(bookmarkClicked()), d->finder, SLOT(onBookmarkSelectedItemsRequested()));
    //connect(d->actionsToolBox, SIGNAL(viewClicked()),   this, SLOT(onFileSystemViewRequested()));
    //connect(d->actionsToolBox, SIGNAL(importClicked()), this, SLOT(onFileSystemImportRequested()));
    //connect(d->actionsToolBox, SIGNAL(loadClicked()),   this, SLOT(onFileSystemLoadRequested()));

    connect (d->toolbar, SIGNAL(showHiddenFiles(bool)), d->finder, SLOT(onShowHiddenFiles(bool)));
    connect (d->toolbar, SIGNAL(showHiddenFiles(bool)), this, SLOT(saveHiddenFilesSettings(bool)));

    // set default values
    medSettingsManager &mng = medSettingsManager::instance();
    bool showHiddenFiles = mng.value("medFileSystemDataSource", "showHiddenFiles", false).toBool();
    bool listViewOn = mng.value("medFileSystemDataSource", "listView", true).toBool();

    d->toolbar->onShowHiddenFiles(showHiddenFiles);

    // our "list view" corresponds to the "tree view" of dtkFinder
    if(listViewOn)
      d->finder->switchToTreeView();
}

medFileSystemDataSource::~medFileSystemDataSource()
{
    delete d;
    d = nullptr;
}

QWidget* medFileSystemDataSource::mainViewWidget()
{
    return d->filesystemWidget;
}

QWidget* medFileSystemDataSource::sourceSelectorWidget()
{
    return d->side;
}

QString medFileSystemDataSource::tabName()
{
    return tr("Import files");
}

QList<medToolBox*> medFileSystemDataSource::getToolBoxes()
{
    return d->toolBoxes;
}

QString medFileSystemDataSource::description(void) const
{
    return tr("Browse the file system");
}

void medFileSystemDataSource::onFileSystemImportRequested()
{
    // remove paths that are subpaths of some other path in the list
    QStringList purgedList = removeNestedPaths(d->finder->selectedPaths());

    for(QString path : purgedList)
    {
        medDataManager::instance().importPath(path, false);
    }
}

void medFileSystemDataSource::onFileSystemLoadRequested()
{
    // remove paths that are subpaths of some other path in the list
    QStringList purgedList = removeNestedPaths(d->finder->selectedPaths());

    for(QString path : purgedList)
    {
        QFileInfo info(path);
        emit load(info.absoluteFilePath());
    }
}

void medFileSystemDataSource::onFileSystemViewRequested()
{
    // remove paths that are subpaths of some other path in the list
    QStringList purgedList = removeNestedPaths(d->finder->selectedPaths());

    for(QString path : purgedList)
    {
        QFileInfo info(path);
        emit open(info.absoluteFilePath());
    }
}

void medFileSystemDataSource::onFileDoubleClicked(const QString& filename)
{
    medDataManager::instance().importPath(filename, false);
}


QStringList medFileSystemDataSource::removeNestedPaths(const QStringList& paths)
{
    QStringList toRemove;

    for(int i = 0; i < paths.size(); i++)
    {
        for(int j = 0; j < paths.size(); j++)
        {
            if(j != i)
            {
                QString path_i = paths.at(i);
                QString path_j = paths.at(j);

                // if path_i is contained in path_j we remove it
                if(path_i.startsWith(path_j))
                    toRemove << path_i;
            }
        }
    }

    QStringList purgedList = paths;

    for(QString path : toRemove)
    {
        purgedList.removeAll(path);
    }

    return purgedList;
}

void medFileSystemDataSource::onFileClicked(const QFileInfo& info)
{
    d->infoText->setVisible(true);
    if (info.isDir()) {
        d->infoText->setText("Directory <b>" + info.fileName() + "</b> selected");
    }
    else {
        d->infoText->setText("<b>" + info.fileName() + "</b> selected - <i>" + this->formatByteSize(info.size()) + "</i>");
    }

    medSettingsManager::instance().setValue("Browser", "default_pipeline_import_path", info.path());
}

QString medFileSystemDataSource::formatByteSize(qint64 bytes)
{
    qint64 b = 1;
    qint64 kb = 1024 * b;
    qint64 mb = 1024 * kb;
    qint64 gb = 1024 * mb;

    if (bytes > gb)
        return QString::number(qIntCast((qreal)(bytes) / gb )) + " GB";
    else if (bytes > mb)
        return QString::number(qIntCast((qreal)(bytes) / mb )) + " MB";
    else if (bytes > kb)
        return QString::number(qIntCast((qreal)(bytes) / kb )) + " KB";
    else
        return QString::number(qIntCast((qreal)(bytes))) + " Bytes";
}

void medFileSystemDataSource::onNothingSelected(void)
{
    d->infoText->setVisible(false);
    d->infoText->setText("");
}

void medFileSystemDataSource::saveHiddenFilesSettings(bool show)
{
    medSettingsManager::instance().setValue("medFileSystemDataSource", "showHiddenFiles", show);
}

void medFileSystemDataSource::saveListViewSettings()
{
    medSettingsManager::instance().setValue("medFileSystemDataSource", "listView", false);
}

void medFileSystemDataSource::saveTreeViewSettings()
{
    medSettingsManager::instance().setValue("medFileSystemDataSource", "listView", true);
}
