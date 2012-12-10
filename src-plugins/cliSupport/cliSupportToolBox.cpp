#include "cliSupportToolBox.h"

#include <dtkCore/dtkAbstractView.h>

#include <ctkCmdLineModuleBackendLocalProcess.h>
#include <ctkCmdLineModuleFrontendFactoryQtGui.h>
#include <ctkCmdLineModuleFuture.h>
#include <ctkCmdLineModuleFutureWatcher.h>
#include <ctkCmdLineModuleManager.h>
#include <ctkCmdLineModuleReference.h>
#include <ctkCmdLineModuleRunException.h>
#include <ctkException.h>

#include <cliSupportUiLoader.h>

#include <medWorkspace.h>
#include <medTabbedViewContainers.h>

QStringList pathSplitter(const QString & path)
{
    QString paths = path;

#ifdef Q_WS_WIN
    QStringList pathList;
    QRegExp pathFilterRx("(([a-zA-Z]:)?[^:]+)");

    int pos = 0;

    while ((pos = pathFilterRx.indexIn(paths, pos)) != -1) {

        QString pathItem = pathFilterRx.cap(1);
        pathItem.replace( "\\" , "/" );

        if (!pathItem.isEmpty())
            pathList << pathItem;

        pos += pathFilterRx.matchedLength();
    }
#else
    QStringList pathList = paths.split(":", QString::SkipEmptyParts);
#endif

    return pathList;
}


class cliSupportToolBoxPrivate
{
public:
    cliSupportToolBoxPrivate()
        : view(NULL)
        , manager(NULL)
        , backend(NULL)
        , frontend(NULL)
        , futureWatcher(NULL)
        , gridLayout(NULL)
        , moduleList(NULL)
        , moduleGui(NULL)
        , moduleRun(NULL)
    {}

    ~cliSupportToolBoxPrivate()
    {
        delete frontend;
        delete backend;
        delete manager;
        delete futureWatcher;
        delete gridLayout;
        delete moduleList;
        delete moduleGui;
        delete moduleRun;
    }

    dtkAbstractView * view;
    medWorkspace * workspace;

    QString path;
    ctkCmdLineModuleManager * manager;
    ctkCmdLineModuleBackendLocalProcess * backend;
    cliSupportFrontendQtGui * frontend;
    ctkCmdLineModuleFutureWatcher * futureWatcher;
    QHash<QString, ctkCmdLineModuleReference> modules;

    QGridLayout * gridLayout;
    QComboBox * moduleList;
    QWidget * moduleGui;
    QProgressBar * moduleProgress;
    QPushButton * moduleRun;
};


cliSupportToolBox::cliSupportToolBox(QWidget * parent, medWorkspace * workspace)
    : medToolBox(parent)
    , d(new cliSupportToolBoxPrivate)
{
    d->workspace = workspace;
    this->setTitle("CLI Manager");
    this->init();
}


cliSupportToolBox::~cliSupportToolBox()
{
    delete d;
    d = NULL;
}


QString cliSupportToolBox::name()
{
    return "CTK CLI Support";
}


QString cliSupportToolBox::identifier()
{
    return "cliSupportToolBox";
}


QString cliSupportToolBox::description()
{
    return "CommandLine plugin support using CTK implementation";
}


void cliSupportToolBox::init()
{
    QString cacheLocation = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
    d->manager = new ctkCmdLineModuleManager(ctkCmdLineModuleManager::STRICT_VALIDATION, cacheLocation);
    d->backend = new ctkCmdLineModuleBackendLocalProcess();
    d->manager->registerBackend(d->backend);

    qApp->addLibraryPath(CTK_QTDESIGNERPLUGINS_DIR); // set in CMakeLists.txt

    QDir module_dir;
    QString defaultPath;
#ifdef Q_WS_MAC
    module_dir = qApp->applicationDirPath() + "/../Modules";
#else
    module_dir = qApp->applicationDirPath() + "/../modules";
#endif
    defaultPath = module_dir.absolutePath();

    const char MODULE_PATH_VAR_NAME[] = "MEDINRIA_MODULE_PATH";
    QByteArray moduleVarArray = qgetenv(MODULE_PATH_VAR_NAME);

    if ( ! moduleVarArray.isEmpty() )
    {
        d->path = QString::fromUtf8(moduleVarArray.constData());
    }
    else
    {
        QSettings settings;
        settings.beginGroup("modules");
        if (!settings.contains("path"))
        {
            qDebug()<<"Filling in empty path in settings with default path:"
                   << module_dir.absolutePath();
            settings.setValue("path", module_dir.absolutePath());
        }
        qDebug()<< "path:" << settings.value("path", defaultPath).toString();
        d->path = settings.value("path", defaultPath).toString();
        settings.endGroup();
    }

    if(d->path.isEmpty()) {
        qWarning() << "Your config does not seem to be set correctly.";
        qWarning() << "Please set modules.path.";
        qWarning() << "Default directory would be: " << defaultPath;
    }

    ctkCmdLineModuleReference moduleRef;

    if( ! d->path.isNull())
    {
        QStringList pathList = pathSplitter(d->path);
        foreach(QString path, pathList)
        {
            QDir dir(path);
            if (dir.exists())
            {
                dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
//                dir.setNameFilters(QStringList() << "ctkCmdLineModule*");
                dir.setSorting(QDir::Name);
                foreach (QFileInfo entry, dir.entryInfoList())
                {
                    try
                    {
                        moduleRef = d->manager->registerModule(QUrl::fromLocalFile(entry.absoluteFilePath()));
                        d->modules.insert(entry.absoluteFilePath(), moduleRef);
                    }
                    catch (const ctkInvalidArgumentException& e)
                    {
                        qDebug() << "Module could not be loaded:" << e;
                        continue;
                    }
                    catch (const ctkCmdLineModuleRunException& e)
                    {
                        qDebug() << "Module could not be loaded:" << e;
                        continue;
                    }
                }
            }
            else
            {
                qWarning() << "Failed to load modules from path " << path << ". Directory does not exist.";
            }
        }
    }

    // Main widget for the tool box content
    QWidget * modulePage = new QWidget(this);
    // We organise in a grid layout
    d->gridLayout = new QGridLayout(modulePage);

    // first row : combo box to select the current module to run
    QHBoxLayout * comboLayout = new QHBoxLayout;
    QLabel * moduleListLabel = new QLabel("Modules:", modulePage);
    d->moduleList = new QComboBox(modulePage);
    comboLayout->addWidget(moduleListLabel);
    comboLayout->addWidget(d->moduleList);

    foreach(const QString & key, d->modules.keys())
    {
        QFileInfo fInfo(key);
        d->moduleList->addItem(fInfo.fileName(), key); //TODO: handle identical names
    }
    connect(d->moduleList, SIGNAL(currentIndexChanged(int)), this, SLOT(moduleSelected(int)));
    d->gridLayout->addLayout(comboLayout, 0, 0);

    // Next row is for the module generated UI

    // Another row for progress bar
    d->moduleProgress = new QProgressBar(modulePage);
    d->moduleProgress->setValue(0);
    d->moduleProgress->setEnabled(false);
    d->gridLayout->addWidget(d->moduleProgress, 2, 0);

    // Another row for run button
    d->moduleRun = new QPushButton("Run CLI", modulePage);
    connect(d->moduleRun, SIGNAL(clicked()), this, SLOT(runCurrentModule()));
    d->gridLayout->addWidget(d->moduleRun, 3, 0, Qt::AlignCenter);

    this->addWidget(modulePage);


    if (d->moduleList->count() == 0)
    {
        d->moduleList->setEnabled(false);
        d->moduleRun->setEnabled(false);
    }
    else
    {
        d->moduleList->setCurrentIndex(0);
        this->moduleSelected(0);
    }
}


void cliSupportToolBox::setData(dtkAbstractData *data)
{
    qDebug() << "CLI setData called";
}


void cliSupportToolBox::clear(void)
{
    d->view = 0;
}


void cliSupportToolBox::update(dtkAbstractView * view)
{
    if (d->view == view)
        return;

    if (d->view)
        QObject::disconnect(d->view, SIGNAL(propertySet(QString, QString)), this, 0);

    qDebug() << "CLI update called";
    d->view = view;
}


void cliSupportToolBox::moduleSelected(int index)
{
    QString key = d->moduleList->itemData(index).toString();
    if ( ! d->modules.contains(key))
        return;

    ctkCmdLineModuleReference ref = d->modules[key];
    delete d->frontend;
    delete d->moduleGui;

    //TODO make medInria not suck.....
    d->frontend = new cliSupportFrontendQtGui(ref, d->workspace);
    d->moduleGui = qobject_cast<QWidget*>(d->frontend->guiHandle());
    d->gridLayout->addWidget(d->moduleGui, 1, 0);
}


void cliSupportToolBox::runCurrentModule()
{
    d->moduleProgress->setEnabled(true);
    d->moduleRun->setEnabled(false);
    d->moduleList->setEnabled(false);
    d->moduleGui->setEnabled(false);
    d->frontend->preRun();
    ctkCmdLineModuleFuture future = d->manager->run(d->frontend);
    d->futureWatcher = new ctkCmdLineModuleFutureWatcher();
    d->futureWatcher->setFuture(future);

    //FIXME : slight race condition here, the module could have already finished.
    connect(d->futureWatcher, SIGNAL(finished()), this, SLOT(moduleFinished()));
    connect(d->futureWatcher, SIGNAL(progressRangeChanged(int,int)), d->moduleProgress, SLOT(setRange(int,int)));
//        connect(d->futureWatcher, SIGNAL(progressTextChanged(QString)), d->moduleProgress, SLOT());
    connect(d->futureWatcher, SIGNAL(progressValueChanged(int)), d->moduleProgress, SLOT(setValue(int)));
}


void cliSupportToolBox::moduleFinished()
{
    d->frontend->postRun();
    d->moduleProgress->setEnabled(false);
    d->moduleRun->setEnabled(true);
    d->moduleList->setEnabled(true);
    d->moduleGui->setEnabled(true);
}
