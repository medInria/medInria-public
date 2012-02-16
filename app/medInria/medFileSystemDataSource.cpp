#include "medFileSystemDataSource.h"

#include <dtkCore/dtkGlobal.h>
#include <dtkGui/dtkFinder.h>

class medFileSystemDataSourcePrivate
{
public:
    QWidget *filesystem_widget;
    dtkFinder *finder;
    dtkFinderPathBar *path;
    dtkFinderSideView *side;
    dtkFinderToolBar *toolbar;
    QLabel * infoText;
};

medFileSystemDataSource::medFileSystemDataSource( QWidget* parent /*= 0*/ ): medAbstractDataSource(parent), d(new medFileSystemDataSourcePrivate)
{
    d->filesystem_widget = new QWidget(parent);

    d->finder = new dtkFinder (d->filesystem_widget);
    d->finder->allowFileBookmarking(false);
    d->finder->setPath(QDir::homePath());

    d->path = new dtkFinderPathBar (d->filesystem_widget);
    d->path->setPath(QDir::homePath());

    d->toolbar = new dtkFinderToolBar (d->filesystem_widget);
    d->toolbar->setPath(QDir::homePath());

    d->infoText = new QLabel(d->filesystem_widget);
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
    QAction *indexAction = new QAction(tr("Index"), this);
    QAction *loadAction = new QAction(tr("Load"), this);
    QAction *viewAction = new QAction(tr("View"), this);

    d->finder->addContextMenuAction(importAction);
    d->finder->addContextMenuAction(indexAction);
    d->finder->addContextMenuAction(loadAction);
    d->finder->addContextMenuAction(viewAction);

    connect(importAction, SIGNAL(triggered()), this, SLOT(onFileSystemImportClicked()));
    connect(indexAction, SIGNAL(triggered()),  this, SLOT(onFileSystemIndexClicked()));
    connect(  loadAction, SIGNAL(triggered()), this, SLOT(onFileSystemLoadClicked()));
    connect(  viewAction, SIGNAL(triggered()), this, SLOT(onFileSystemViewClicked()));

    QHBoxLayout *toolbar_layout = new QHBoxLayout;
    toolbar_layout->setContentsMargins(0, 0, 0, 0);
    toolbar_layout->setSpacing(0);
    toolbar_layout->addWidget  (d->toolbar);
    toolbar_layout->addWidget  (d->path);

    QVBoxLayout *filesystem_layout = new QVBoxLayout(d->filesystem_widget);
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
    connect (d->toolbar, SIGNAL(showHiddenFiles(bool)), d->finder, SLOT(onShowHiddenFiles(bool)));
}

medFileSystemDataSource::~medFileSystemDataSource()
{
    delete d;
    d = NULL;
}

QWidget* medFileSystemDataSource::mainViewWidget()
{
    return d->filesystem_widget;
}

QWidget* medFileSystemDataSource::sourceSelectorWidget()
{
    return d->side;
}

QString medFileSystemDataSource::tabName()
{
    return tr("File system");
}

QList<medToolBox*> medFileSystemDataSource::getToolboxes()
{
    return QList<medToolBox*>();
}

QString medFileSystemDataSource::description(void) const
{
	return tr("Browse the file system");
}

void medFileSystemDataSource::onFileSystemImportClicked(void)
{
    QFileInfo info(d->finder->selectedPath());
    emit dataToImportReceived(info.absoluteFilePath());
}

void medFileSystemDataSource::onFileSystemIndexClicked(void)
{
    QFileInfo info(d->finder->selectedPath());
    emit dataToIndexReceived(info.absoluteFilePath());
}

void medFileSystemDataSource::onFileSystemLoadClicked()
{
    QFileInfo info(d->finder->selectedPath());
    emit load(info.absoluteFilePath());
}

void medFileSystemDataSource::onFileSystemViewClicked()
{
    QFileInfo info(d->finder->selectedPath());
    emit open(info.absoluteFilePath());
}

void medFileSystemDataSource::onFileDoubleClicked(const QString& filename)
{
    QFileInfo info(filename);
    if (info.isFile())
        emit open(info.absoluteFilePath());
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
