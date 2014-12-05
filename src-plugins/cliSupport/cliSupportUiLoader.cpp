/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <cliSupportUiLoader.h>
#include <ctkCmdLineModuleXslTransform.h>
#include <ctkCmdLineModuleReference.h>
#include <ctkCmdLineModuleDescription.h>
#include <ctkCmdLineModuleParameter.h>

#include <medAbstractDataFactory.h>
#include <medAbstractDataReader.h>
#include <medDataManager.h>
#include <medAbstractWorkspace.h>
#include <medTabbedViewContainers.h>
#include <medAbstractData.h>
#include <medAbstractView.h>

#include <QDebug>
#include <QScopedPointer>
#include <QComboBox>

bool removeDir(const QString & dirName)
{
    bool result = false;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        foreach(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

// ------------------------- UiLoader ----------------------------------------

cliSupportUiLoader::cliSupportUiLoader(cliSupportFrontendQtGui * frontend, QObject *parent)
    : ctkCmdLineModuleQtUiLoader(parent)
    , _frontend(frontend)
{
}


cliSupportUiLoader::~cliSupportUiLoader()
{
}


QWidget * cliSupportUiLoader::createWidget(const QString &className, QWidget *parent, const QString &name)
{
    if ( ! name.startsWith("parameter:"))
        return ctkCmdLineModuleQtUiLoader::createWidget(className, parent, name);


    ctkCmdLineModuleDescription modDescription = _frontend->moduleReference().description();
    QString paramName = name.mid(10);
    if ( ! modDescription.hasParameter(paramName)) {
        qDebug() << "cliSupport : Parameter" << paramName << "doesn't exist !";
        return ctkCmdLineModuleQtUiLoader::createWidget(className, parent, name);
    }

    ctkCmdLineModuleParameter param = modDescription.parameter(paramName);
    qDebug() << name << param.tag();

    if (param.tag() == "image" && param.channel() == "input") {
        cliDataInputWidget * w = new cliDataInputWidget(param, parent);
        _frontend->addDataInput(w);
        return w;
    } else if (param.tag() == "image" && param.channel() == "output") {
        cliDataOutputWidget * w = new cliDataOutputWidget(param, parent);
        _frontend->addDataOutput(w);
        return w;
    }

    return ctkCmdLineModuleQtUiLoader::createWidget(className, parent, name);
}

// ---------------------------- Frontend -------------------------------------

cliSupportFrontendQtGui::cliSupportFrontendQtGui(const ctkCmdLineModuleReference & moduleRef, medAbstractWorkspace *workspace)
    : ctkCmdLineModuleFrontendQtGui(moduleRef)
    , _workspace(workspace)
{
    ctkCmdLineModuleXslTransform * t = this->xslTransform();

    QStringList paramTagsList;
    paramTagsList << "imageInput" << "imageOutput"
                  << "geometryInput" << "geometryOutput"
                  << "transformInput" << "transformOutput"
                  << "tableInput" << "tableOutput"
                  << "measurementInput" << "measurementOutput";

    foreach(const QString& xslParamName, paramTagsList) {
        t->bindVariable(QString("%1Widget").arg(xslParamName), "cliDataSelector");
        t->bindVariable(QString("%1SetProperty").arg(xslParamName), "");
        t->bindVariable(QString("%1ValueProperty").arg(xslParamName), "filePath");
    }
}


cliSupportFrontendQtGui::~cliSupportFrontendQtGui()
{
}


void cliSupportFrontendQtGui::preRun()
{
    // create temp dir for input files
    int count = 0;
    _runDir = QDir::temp();
    while ( ! _runDir.mkdir(QString("medInria_cli_run_%1").arg(count)))
        count++;
    _runDir.cd(QString("medInria_cli_run_%1").arg(count));

    // no clear standard yet on whether extensions are formatted as :
    // "*.ext", ".ext" or "ext", so we handle all 3 possiblities.
    QRegExp extensionRegex("\\s*(?:\\*?\\.)?([^,]+?)\\s*");
    foreach(cliDataInputWidget * w, _inputList) {
        medAbstractData * data = medDataManager::instance()->retrieveData(w->index());
        if ( ! data) continue;

        QStringList possibleExtensions = w->parameter().fileExtensions().replaceInStrings(extensionRegex, ".\\1");
        QString finalPath = _runDir.absoluteFilePath(w->parameter().name());

        cliFileHandler exporter;
        QString exportPath = exporter.exportToFile(data, finalPath, possibleExtensions);

        w->setFilePath(exportPath);
        qDebug() << "======" << exportPath << possibleExtensions << w->parameter().fileExtensions();
    }

    foreach(cliDataOutputWidget * w, _outputList) {
        QStringList possibleExtensions = w->parameter().fileExtensions().replaceInStrings(extensionRegex, ".\\1");
        QString ext = cliFileHandler::compatibleImportExtension(possibleExtensions);
        // TODO error handling on ext being empty
        QString outputPath = _runDir.absoluteFilePath(w->parameter().name()) + ext;
        w->setFilePath(outputPath);
    }
}

void cliSupportFrontendQtGui::postRun()
{
    foreach(cliDataOutputWidget * w, _outputList) {
        cliFileHandler importer;
        medAbstractData * data = qobject_cast<medAbstractData*>(importer.importFromFile(w->filePath()));
        if (data)
            _workspace->open(data->dataIndex());
    }

    //removeDir(_runDir.absolutePath());
}


void cliSupportFrontendQtGui::addDataInput(cliDataInputWidget * input)
{
    _inputList.append(input);
}


void cliSupportFrontendQtGui::addDataOutput(cliDataOutputWidget * output)
{
    _outputList.append(output);
}


QUiLoader * cliSupportFrontendQtGui::uiLoader() const
{
    if (_loader == NULL) {
      _loader.reset(new cliSupportUiLoader(const_cast<cliSupportFrontendQtGui*>(this)));
    }
    return _loader.data();
}

// ------------------------ Widgets ------------------------------------------

cliDataInputWidget::cliDataInputWidget(ctkCmdLineModuleParameter param, QWidget * parent)
    : medDropSite(parent)
    , _param(param)
{
}


cliDataInputWidget::~cliDataInputWidget()
{

}


QString cliDataInputWidget::filePath() const
{
    return _filePath;
}


void cliDataInputWidget::setFilePath(QString path)
{
    _filePath = path;
}


ctkCmdLineModuleParameter cliDataInputWidget::parameter() const
{
    return _param;
}


cliDataOutputWidget::cliDataOutputWidget(ctkCmdLineModuleParameter param, QWidget * parent)
    : QWidget(parent)
    , _param(param)
{
    this->setLayout(new QHBoxLayout());
    _targetList = new QComboBox(this);
    _targetList->addItem("Open in new view", cliDataOutputWidget::OpenInNewView);
    _targetList->addItem("Open in new tab", cliDataOutputWidget::OpenInNewTab);
    _targetList->addItem("Output to file...", cliDataOutputWidget::OutputToFile);
    this->layout()->addWidget(_targetList);
}


cliDataOutputWidget::~cliDataOutputWidget()
{
}


QString cliDataOutputWidget::filePath() const
{
    return _filePath;
}


void cliDataOutputWidget::setFilePath(QString path)
{
    _filePath = path;
}


ctkCmdLineModuleParameter cliDataOutputWidget::parameter() const
{
    return _param;
}


cliDataOutputWidget::OutputTarget cliDataOutputWidget::target() const
{
    return (cliDataOutputWidget::OutputTarget)_targetList->itemData(_targetList->currentIndex()).toInt();
}

// --------------------- File exporter ------------------------------------

cliFileHandler::cliFileHandler(QObject * parent)
    : QObject(parent)
{

}

cliFileHandler::~cliFileHandler()
{

}

QString cliFileHandler::compatibleImportExtension(QStringList supportedExtensions)
{
    QList<QString> readers = medAbstractDataFactory::instance()->readers();
    QScopedPointer<medAbstractDataReader> reader;
    foreach(QString currentExtension, supportedExtensions) {
        for (int i=0; i<readers.size(); i++) {
            reader.reset(qobject_cast<medAbstractDataReader*>(medAbstractDataFactory::instance()->reader(readers[i])));

            if ( ! reader || ! reader->supportedFileExtensions().contains(currentExtension))
                continue;

            return currentExtension;
        }
    }
    return QString();
}

medAbstractData * cliFileHandler::importFromFile(QString file)
{
    qDebug() << "asdffffffffff";
    connect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex,QUuid)),
            this, SLOT(dataImported(medDataIndex,QUuid)));
    //TODO, no more failedToImport signal in MDM
    // We connect fail to import on the same slot, as failure will end up with NULL _data
//    connect(medDataManager::instance(), SIGNAL((medDataIndex)),
//            this, SLOT(dataImported()));
    _expectedUuid = medDataManager::instance()->importPath(file,false,false);
    QTimer timeout;
    connect(&timeout, SIGNAL(timeout()), &_loopyLoop, SLOT(quit()));
//    timeout.start(10000);
    qDebug() << file;
    _loopyLoop.exec(QEventLoop::ExcludeUserInputEvents);
    qDebug() << "dsfsdfipsdfpdsof";
    return _data;
}

QString cliFileHandler::exportToFile(medAbstractData * data, QString filePath, QStringList formats)
{
    QList<QString> writers = medAbstractDataFactory::instance()->writers();
    bool written = false;
    QString finalFullPath;
    QScopedPointer<dtkAbstractDataWriter> dataWriter;
    qDebug() << "000000000000000000" << writers;
    foreach(QString currentFormat, formats) {
        for (int i=0; i<writers.size(); i++) {
            dataWriter.reset(medAbstractDataFactory::instance()->writer(writers[i]));

            qDebug() << dataWriter->identifier();
            if (! dataWriter->handled().contains(data->identifier()) ||
                ! dataWriter->supportedFileExtensions().contains(currentFormat))
                continue;

            dataWriter->setData (data);

            finalFullPath = filePath+currentFormat;
            if (dataWriter->canWrite(finalFullPath) && dataWriter->write(finalFullPath)) {
                written = true;
                break;
            }
        }
        if (written)
            break;
    }
    return finalFullPath;
}


void cliFileHandler::dataImported(medDataIndex index, QUuid uuid)
{
    qDebug() << "HEEEEEEEEEEEElllllooooooooooooooooooo";
    if (uuid != _expectedUuid)
        return;
    _expectedUuid = QUuid();
    _data = medDataManager::instance()->retrieveData(index);
    qDebug() << "Looooooooooooopyyyy";
    _loopyLoop.quit();
    qDebug() << "Looooooooooooopyyyy looooooooop";
}
