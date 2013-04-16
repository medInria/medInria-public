/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medFileSystemDataSource.h"

#include <dtkCore/dtkGlobal.h>
#include <dtkGui/dtkFinder.h>

#include <medActionsToolBox.h>

class medFileSystemDataSourcePrivate
{
public:
    QWidget *filesystemWidget;
    dtkFinder *finder;
    dtkFinderPathBar *path;
    dtkFinderSideView *side;
    dtkFinderToolBar *toolbar;
    QList<medToolBox*> toolBoxes;
    medActionsToolBox* actionsToolBox;
    QLabel * infoText;
};

medFileSystemDataSource::medFileSystemDataSource( QWidget* parent /*= 0*/ ): medAbstractDataSource(parent), d(new medFileSystemDataSourcePrivate)
{
    d->filesystemWidget = new QWidget(parent);

    d->finder = new dtkFinder (d->filesystemWidget);
    d->finder->allowFileBookmarking(false);
    d->finder->allowMultipleSelection(true);
    d->finder->setPath(QDir::homePath());

    d->path = new dtkFinderPathBar (d->filesystemWidget);
    d->path->setPath(QDir::homePath());

    d->toolbar = new dtkFinderToolBar (d->filesystemWidget);
    d->toolbar->setPath(QDir::homePath());

    d->infoText = new QLabel(d->filesystemWidget);
    d->infoText->setText("");
    d->infoText->setVisible(false);
    d->infoText->setTextFormat(Qt::RichText);
    d->infoText->setStyleSheet(
                "font-size: 11px;"
                "color: #b2b8b2;"
                "border-bottom: 1px solid #a9a9a9;"
                "border-right: 1px solid #a9a9a9;"
                "border-left: 1px solid #a9a9a9;"
                "border-top: 0px;"
                "border-radius: 0px;"
                "padding: 0 8px;"
                "background: #4b4b4b;"
                "selection-background-color: #4b4b4b;"
                "selection-color: #b2b8b2;"
                );

    d->actionsToolBox = new medActionsToolBox(parent);
    d->toolBoxes.push_back(d->actionsToolBox);

    d->side = new dtkFinderSideView;
    d->side->setStyleSheet(
        "dtkFinderSideView {"
        "    color: #b2b2b2;"
        "    padding: 5px;"
        "    background: #494949;"
        "    show-decoration-selected: 1;"
        "    border-width: 0px"
        "}"
        ""
        "dtkFinderSideView::item {"
        "    margin-left: 0px;"
        "    border-top-color: transparent;"
        "    border-bottom-color: transparent;"
        "}"
        ""
        "dtkFinderSideView::item:selected {"
        "    border-top: 1px solid #567dbc;"
        "    border-bottom: 1px solid #567dbc;"
        "}"
        ""
        "dtkFinderSideView::item:selected:active{"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"
        ""
        "dtkFinderSideView::item:selected:!active {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
        "}"
        ""
        "dtkFinderSideView::branch:selected {"
        "    border-top: 1px solid #567dbc;"
        "    border-bottom: 1px solid #567dbc;"
        "}"
        ""
        "dtkFinderSideView::branch:selected:active{"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"
        ""
        "dtkFinderSideView::branch:selected:!active {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
        "}");

    QAction *importAction = new QAction(tr("Import"), this);
    importAction->setIconVisibleInMenu(true);
    importAction->setIcon(QIcon(":icons/import.png"));
    QAction *indexAction = new QAction(tr("Index"), this);
    indexAction->setIconVisibleInMenu(true);
    indexAction->setIcon(QIcon(":icons/finger.png"));
    QAction *loadAction = new QAction(tr("Load"), this);
    loadAction->setIconVisibleInMenu(true);
    loadAction->setIcon(QIcon(":icons/document-open.png"));
    QAction *viewAction = new QAction(tr("View"), this);
    viewAction->setIconVisibleInMenu(true);
    viewAction->setIcon(QIcon(":icons/eye.png"));

    d->finder->addContextMenuAction(importAction);
    d->finder->addContextMenuAction(indexAction);
    d->finder->addContextMenuAction(loadAction);
    d->finder->addContextMenuAction(viewAction);

    connect(importAction, SIGNAL(triggered()), this, SLOT(onFileSystemImportRequested()));
    connect(indexAction, SIGNAL(triggered()),  this, SLOT(onFileSystemIndexRequested()));
    connect(  loadAction, SIGNAL(triggered()), this, SLOT(onFileSystemLoadRequested()));
    connect(  viewAction, SIGNAL(triggered()), this, SLOT(onFileSystemViewRequested()));

    QHBoxLayout *toolbar_layout = new QHBoxLayout;
    toolbar_layout->setContentsMargins(0, 0, 0, 0);
    toolbar_layout->setSpacing(0);
    toolbar_layout->addWidget  (d->toolbar);
    toolbar_layout->addWidget  (d->path);

    QVBoxLayout *filesystem_layout = new QVBoxLayout(d->filesystemWidget);
    filesystem_layout->setContentsMargins(10, 10, 10, 10);
    filesystem_layout->setSpacing(0);
    filesystem_layout->addLayout (toolbar_layout);
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
    connect(d->finder, SIGNAL(showHiddenFiles(bool)), d->toolbar, SLOT(onShowHiddenFiles(bool)));

    connect(d->path, SIGNAL(changed(QString)), d->finder, SLOT(setPath(QString)));
    connect(d->path, SIGNAL(changed(QString)), d->side, SLOT(setPath(QString)));
    connect(d->path, SIGNAL(changed(QString)), d->toolbar, SLOT(setPath(QString)));

    connect(d->side, SIGNAL(changed(QString)), d->finder, SLOT(setPath(QString)));
    connect(d->side, SIGNAL(changed(QString)), d->path, SLOT(setPath(QString)));
    connect(d->side, SIGNAL(changed(QString)), d->toolbar, SLOT(setPath(QString)));

    connect(d->finder, SIGNAL(bookmarked(QString)), d->side, SLOT(addBookmark(QString)));

    connect (d->toolbar, SIGNAL(changed(QString)), d->finder, SLOT(setPath(QString)));
    connect (d->toolbar, SIGNAL(changed(QString)), d->path,   SLOT(setPath(QString)));

    connect (d->toolbar, SIGNAL(treeView()),       d->finder, SLOT(switchToTreeView()));
    connect (d->toolbar, SIGNAL(listView()),       d->finder, SLOT(switchToListView()));

    connect(d->finder, SIGNAL(selectionChanged(const QStringList&)), d->actionsToolBox, SLOT(selectedPathsChanged(const QStringList&)));

    connect(d->actionsToolBox, SIGNAL(bookmarkClicked()), d->finder, SLOT(onBookmarkSelectedItemsRequested()));
    connect(d->actionsToolBox, SIGNAL(viewClicked()), this, SLOT(onFileSystemViewRequested()));
    connect(d->actionsToolBox, SIGNAL(importClicked()), this, SLOT(onFileSystemImportRequested()));
    connect(d->actionsToolBox, SIGNAL(indexClicked()), this, SLOT(onFileSystemIndexRequested()));
    connect(d->actionsToolBox, SIGNAL(loadClicked()), this, SLOT(onFileSystemLoadRequested()));

    connect (d->toolbar, SIGNAL(showHiddenFiles(bool)), d->finder, SLOT(onShowHiddenFiles(bool)));
}

medFileSystemDataSource::~medFileSystemDataSource()
{
    delete d;
    d = NULL;
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
    return tr("File system");
}

QList<medToolBox*> medFileSystemDataSource::getToolBoxes()
{
    return d->toolBoxes;
}

QString medFileSystemDataSource::description(void) const
{
	return tr("Browse the file system");
}

void medFileSystemDataSource::onFileSystemImportRequested(void)
{
    // remove paths that are subpaths of some other path in the list
    QStringList purgedList = removeNestedPaths(d->finder->selectedPaths());

    foreach(QString path, purgedList)
    {
        QFileInfo info(path);
        emit dataToImportReceived(info.absoluteFilePath());
    }
}

void medFileSystemDataSource::onFileSystemIndexRequested(void)
{
    // remove paths that are subpaths of some other path in the list
    QStringList purgedList = removeNestedPaths(d->finder->selectedPaths());

    foreach(QString path, purgedList)
    {
        QFileInfo info(path);
        emit dataToIndexReceived(info.absoluteFilePath());
    }
}

void medFileSystemDataSource::onFileSystemLoadRequested()
{
    // remove paths that are subpaths of some other path in the list
    QStringList purgedList = removeNestedPaths(d->finder->selectedPaths());

    foreach(QString path, purgedList)
    {
        QFileInfo info(path);
        emit load(info.absoluteFilePath());
    }
}

void medFileSystemDataSource::onFileSystemViewRequested()
{
    // remove paths that are subpaths of some other path in the list
    QStringList purgedList = removeNestedPaths(d->finder->selectedPaths());

    foreach(QString path, purgedList)
    {
        QFileInfo info(path);
        emit open(info.absoluteFilePath());
    }
}

void medFileSystemDataSource::onFileDoubleClicked(const QString& filename)
{
    QFileInfo info(filename);
    if (info.isFile())
        emit open(info.absoluteFilePath());
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

    foreach(QString path, toRemove)
        purgedList.removeAll(path);

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
