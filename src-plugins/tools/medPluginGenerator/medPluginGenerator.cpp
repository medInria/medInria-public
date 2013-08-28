/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medPluginGenerator.h"

class medPluginGeneratorPrivate
{
public:
    QString plugin;
    QString output;
    QString name;
    QString pluginFamily;
    QString type;
    QString description;
    QString license;
    
    QDir parent;
    QDir target;
};


medPluginGenerator::medPluginGenerator()
{
    this->d = new medPluginGeneratorPrivate;
}

medPluginGenerator::~medPluginGenerator()
{
    delete this->d;
    this->d = NULL;
}

void medPluginGenerator::setPluginFamily(const QString& family)
{
    d->pluginFamily = family;
}

void medPluginGenerator::setOutputDirectory(const QString& directory)
{
    d->output = directory;
}

void medPluginGenerator::setName(const QString& name)
{
    d->name = name;
}

void medPluginGenerator::setType(const QString& type)
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
    
    if (d->pluginFamily == "filtering")
        return generateCMakeLists()
        && generateTypeHeaderFile()
        && generateTypeSourceFile()
        && generateTypeToolBoxSourceFile()
        && generateTypeToolBoxHeaderFile()
        && generatePluginHeaderFile()
        && generatePluginSourceFile()
        && generateExportHeaderFile()
        && generateHelpCollectionFile()
        && generateHelpConfigurationFile()
        && generateReadmeFile()
        && generateCopyingFile();
    
    if (d->pluginFamily == "registration")
        return generateCMakeLists()
        && generateTypeHeaderFile()
        && generateTypeSourceFile()
        && generateRPIHeaderFile()
        && generateRPISourceFile()
        && generateTypeToolBoxSourceFile()
        && generateTypeToolBoxHeaderFile()
        && generatePluginHeaderFile()
        && generatePluginSourceFile()
        && generateExportHeaderFile()
        && generateHelpCollectionFile()
        && generateHelpConfigurationFile()
        && generateReadmeFile()
        && generateCopyingFile();
    
    return generateCMakeLists()
    && generateTypeHeaderFile()
    && generateTypeSourceFile()
    && generatePluginHeaderFile()
    && generatePluginSourceFile()
    && generateExportHeaderFile()
    && generateHelpCollectionFile()
    && generateHelpConfigurationFile()
    && generateReadmeFile()
    && generateCopyingFile();
}

QStringList medPluginGenerator::pluginFamilies()
{
    QStringList families;

    return families;
}

QStringList medPluginGenerator::pluginTypes()
{
    QStringList types;

    return types;
}

// /////////////////////////////////////////////////////////////////
// CMakeLists.txt
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateCMakeLists()
{
    QFile targetFile(d->target.absoluteFilePath("CMakeLists.txt"));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open CMakeLists.txt for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/cmake").arg(d->pluginFamily));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll()).arg(d->plugin);
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// Type header file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateTypeHeaderFile()
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append(".h")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append(".h") << "for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/type.h").arg(d->pluginFamily));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll())
    .arg(d->plugin)
    .arg(d->plugin.toUpper())
    .arg(d->type)
    .arg(QString(d->plugin).replace(0, 1, d->plugin.left(1).toUpper()));
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// Type source file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateTypeSourceFile()
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append(".cpp")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append(".cpp") << "for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/type.cpp").arg(d->pluginFamily));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    if (d->pluginFamily == "registration")
    {    
        stream << QString(templateFile.readAll())
        .arg(d->plugin)
        .arg(d->type)
        .arg(QString(d->plugin).replace(0, 1, d->plugin.left(1).toUpper()))
        .arg(QString(d->name).replace(0, 1, d->name.left(1).toUpper()));
    }
    else
    {
        stream << QString(templateFile.readAll())
        .arg(d->plugin)
        .arg(d->type)
        .arg(QString(d->plugin).replace(0, 1, d->plugin.left(1).toUpper()));
    }
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// RPI header file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateRPIHeaderFile()
{
    QString baseName = QString(d->plugin).replace(0, 1, d->plugin.left(1).toUpper());
    QFile targetFile(d->target.absoluteFilePath(QString("rpi" + baseName).append(".h")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString("rpi" + baseName).append(".h") << "for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/rpiBase.h").arg(d->pluginFamily));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll())
    .arg(QString(d->plugin).replace(0, 1, d->plugin.left(1).toUpper()));
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// RPI source file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateRPISourceFile()
{
    QString baseName = QString(d->plugin).replace(0, 1, d->plugin.left(1).toUpper());
    QFile targetFile(d->target.absoluteFilePath(QString("rpi" + baseName).append(".hxx")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString("rpi" + baseName).append(".hxx") << "for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/rpiBase.hxx").arg(d->pluginFamily));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll())
    .arg(QString(d->plugin).replace(0, 1, d->plugin.left(1).toUpper()));
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// Type ToolBox header file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateTypeToolBoxHeaderFile()
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append("ToolBox.h")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append("ToolBox.h") << "for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/typeToolBox.h").arg(d->pluginFamily));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll())
    .arg(d->plugin)
    .arg(d->plugin.toUpper());
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// Type ToolBox source file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateTypeToolBoxSourceFile()
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append("ToolBox.cpp")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append("ToolBox.cpp") << "for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/typeToolBox.cpp").arg(d->pluginFamily));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll())
    .arg(d->plugin)
    .arg(d->name);
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}


// /////////////////////////////////////////////////////////////////
// Plugin header file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generatePluginHeaderFile()
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append("Plugin.h")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append("Plugin.h") << "for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/plugin.h").arg(d->pluginFamily));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll())
    .arg(d->plugin)
    .arg(d->plugin.toUpper());
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// Plugin source file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generatePluginSourceFile()
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append("Plugin.cpp")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append("Plugin.cpp") << "for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/plugin.cpp").arg(d->pluginFamily));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll())
    .arg(d->plugin)
    .arg(d->plugin.toUpper())
    .arg(d->description);
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// Export file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateExportHeaderFile()
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append("PluginExport.h")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append("PluginExport.h") << "for writing";
        return false;
    }
    
    QFile templateFile(":template/export.h");
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll()).arg(d->plugin).arg(d->plugin.toUpper());
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// Help collection file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateHelpCollectionFile()
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append("Plugin.qhcp.in")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append("Plugin.qhcp.in") << "for writing";
        return false;
    }
    
    QFile templateFile(":template/qhcp");
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll());
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}


// /////////////////////////////////////////////////////////////////
// Help configuration file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateHelpConfigurationFile()
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append("Plugin.doxyfile.in")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append("Plugin.doxyfile.in") << "for writing";
        return false;
    }
    
    QFile templateFile(":template/doxyfile");
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll());
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// README file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateReadmeFile()
{
    QFile targetFile(d->target.absoluteFilePath("README.txt"));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open CMakeLists.txt for writing";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << d->description;
    
    targetFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// COPYING file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateCopyingFile()
{
    QFile targetFile(d->target.absoluteFilePath("COPYING.txt"));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open COPYING.txt for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/license/").append(d->license));
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll());
    
    targetFile.close();
    
    return true;
}

