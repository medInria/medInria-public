/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medPluginGenerator.h>

class medPluginGeneratorPrivate
{
public:
    QString plugin;
    QString output;
    QString name;
    pluginType type;
    QString description;
    QString license;
    bool generateClassOnly;
    
    QDir parent;
    QDir target;
};


medPluginGenerator::medPluginGenerator()
{
    this->d = new medPluginGeneratorPrivate;
    d->generateClassOnly = false;
}

medPluginGenerator::~medPluginGenerator()
{
    delete this->d;
    this->d = NULL;
}

void medPluginGenerator::setOutputDirectory(const QString& directory)
{
    d->output = directory;
}

void medPluginGenerator::setName(const QString& name)
{
    d->name = name;
}

void medPluginGenerator::setType(pluginType type)
{
    d->type = type;
}

void medPluginGenerator::setDescription(const QString& desc)
{
    d->description = desc;
}

void medPluginGenerator::setLicense(const QString& license)
{
    d->license = license;
}

void medPluginGenerator::setGenerateClassOnly(bool generateClassOnly)
{
    d->generateClassOnly = generateClassOnly;
}

bool medPluginGenerator::run()
{
    d->parent = QDir(d->output);
    
    if(!d->parent.exists()) {
        qWarning() << "medPluginGenerator: parent directory is not valid.";
        return false;
    }
    
    d->plugin = d->name.replace(0, 1, d->name.left(1).toLower());
    
    if(!d->parent.mkdir(d->plugin)) {
        qWarning() << "medPluginGenerator: unable to create target directory.";
        return false;
    }
    
    d->target = QDir(d->parent);
    
    if(!d->target.cd(d->plugin)) {
        qWarning() << "medPluginGenerator: unable to move to target directory.";
        return false;
    }

    QString baseName = QString(d->plugin).replace(0, 1, d->plugin.left(1).toUpper());
    bool res = false;

    QString CMakelistTemplate = ":template/cmake";
    QString pluginTemplate = ":/template/plugin";

    switch(d->type)
    {
    case DATA:
        res = generateFile(QString(d->plugin).append(".h"),":/template/data/data.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/data/data.cpp");
        break;
    case VIEW:
        res = generateFile(QString(d->plugin).append(".h"),":/template/view/view.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/view/view.cpp");
        break;
    case LAYERED_VIEW:
        res = generateFile(QString(d->plugin).append(".h"),":/template/view/layeredView.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/view/layeredView.cpp");
        break;
    case IMAGE_VIEW:
        res = generateFile(QString(d->plugin).append(".h"),":/template/view/imageView.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/view/imageView.cpp");
        break;
    case VIEW_INTERACTOR:
        res = generateFile(QString(d->plugin).append(".h"),":/template/view/interactors/viewInteractor.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/view/interactors/viewInteractor.cpp");
        break;
    case LAYERED_INTERACTOR:
        res = generateFile(QString(d->plugin).append(".h"),":/template/view/interactors/layeredViewInteractor.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/view/interactors/layeredViewInteractor.cpp");
        break;
    case IMAGE_INTERACTOR:
        res = generateFile(QString(d->plugin).append(".h"),":/template/view/interactors/imageViewInteractor.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/view/interactors/imageViewInteractor.cpp");
        break;
    case EXTRA_INTERACTOR:
        res = generateFile(QString(d->plugin).append(".h"),":/template/view/interactors/extraInteractor.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/view/interactors/extraInteractor.cpp");
        break;
    case VIEW_NAVIGATOR:
        res = generateFile(QString(d->plugin).append(".h"),":/template/view/navigators/viewNavigator.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/view/navigators/viewNavigator.cpp");
        break;
    case LAYERED_NAVIGATOR:
        res = generateFile(QString(d->plugin).append(".h"),":/template/view/navigators/layeredViewNavigator.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/view/navigators/layeredViewNavigator.cpp");
        break;
    case IMAGE_NAVIGATOR:
        res = generateFile(QString(d->plugin).append(".h"),":/template/view/navigators/imageViewNavigator.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/view/navigators/imageViewNavigator.cpp");
        break;
    case EXTRA_NAVIGATOR:
        res = generateFile(QString(d->plugin).append(".h"),":/template/view/navigators/extraNavigator.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/view/navigators/extraNavigator.cpp");
        break;
    case FILTERING:
        CMakelistTemplate = ":template/filtering/cmake";
        pluginTemplate = ":/template/filtering/plugin";
        res = generateFile(QString(d->plugin).append(".h"),":/template/filtering/type.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/filtering/type.cpp")
                && generateFile(QString(d->plugin).append("ToolBox.h"),":/template/filtering/typeToolBox.h")
                && generateFile(QString(d->plugin).append("ToolBox.cpp"),":/template/filtering/typeToolBox.cpp");
        break;
    case REGISTRATION:
        CMakelistTemplate = ":template/registration/cmake";
        pluginTemplate = ":/template/registration/plugin";
        res = generateFile(QString(d->plugin).append(".h"),":/template/registration/type.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/registration/type.cpp")
                && generateFile(QString("rpi"+ baseName).append(".h"),":/template/registration/rpiBase.h")
                && generateFile(QString("rpi"+ baseName).append(".hxx"),":/template/registration/rpiBase.hxx")
                && generateFile(QString(d->plugin).append("ToolBox.h"),":/template/registration/typeToolBox.h")
                && generateFile(QString(d->plugin).append("ToolBox.cpp"),":/template/registration/typeToolBox.cpp");
        break;
//    TODO
//    case DIFFUSION:
//        break;
    case DATA_READER:
        res = generateFile(QString(d->plugin).append(".h"),":/template/data/reader.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/data/reader.cpp");
        break;
    case DATA_WRITER:
        res = generateFile(QString(d->plugin).append(".h"),":/template/data/writer.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/data/writer.cpp");
        break;
    case WORKSPACE:
        res = generateFile(QString(d->plugin).append(".h"),":/template/workspace.h")
                && generateFile(QString(d->plugin).append(".cpp"),":/template/workspace.cpp");
        break;
    }

    if(res && !d->generateClassOnly)
    {
        return generateFile("CMakeLists.txt", CMakelistTemplate)
                && generateFile(QString(d->plugin).append("Plugin.h"), pluginTemplate +".h")
                && generateFile(QString(d->plugin).append("Plugin.cpp"), pluginTemplate +".cpp")
                && generateFile(QString(d->plugin).append("PluginExport.h"),":/template/export.h")
                && generateReadmeFile();
    }
    else return res;
}



// /////////////////////////////////////////////////////////////////
// README file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateReadmeFile()
{
    QFile targetFile(d->target.absoluteFilePath("README.txt"));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open README.txt for writing";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << d->description;
    
    targetFile.close();
    
    return true;
}


bool medPluginGenerator::generateFile(QString targetFilename, QString templateFilename)
{
    QFile targetFile(d->target.absoluteFilePath(targetFilename));

    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << targetFile.fileName() << "for writing";
        return false;
    }

    QFile templateFile(templateFilename);

    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }

    QTextStream stream(&targetFile);
    QString content = QString(templateFile.readAll())
            .replace("%1", d->plugin)
            .replace("%2", d->plugin.toUpper())
            .replace("%3", QString(d->plugin).replace(0, 1, d->plugin.left(1).toUpper()));

    if(d->generateClassOnly)
    {
        content.replace("#include <"+d->plugin+"PluginExport.h>", "")
               .replace(d->plugin.toUpper()+"PLUGIN_EXPORT", "");
    }
    stream << content;


    targetFile.close();
    templateFile.close();

    return true;
}
