/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <cstdlib>
#include <cstdarg>
#include <sys/stat.h>
#include <sys/types.h>

#include <medCompositeDataSetsWriter.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <dtkZip/dtkZipWriter.h>

#include <medCompositeDataSetsBase.h>
#include<dirTools.h>

static bool error(const QString& dirname) {
    RemoveDirectory(QDir(dirname));
    return false;
}

bool medCompositeDataSetsWriter::write(const QString& path) {

    //  Write the data in a temporary directory.

    const QString& zipdirn = zip_dirname(path);
    QDir systemTmpDir = QDir::temp();
    const QString& uuid = QUuid::createUuid().toString().replace("{","").replace("}","");
    const QString& tmpDirn = "medcds"+uuid;
    const QString& tmpDirPath =QDir::cleanPath(systemTmpDir.absolutePath()+QDir::separator()+tmpDirn);
    const QString& zipDirPath = tmpDirPath+QDir::separator()+zipdirn;

    if (!systemTmpDir.mkpath(zipDirPath)) {
        qWarning() << tr("medCompositeDataSets: cannot create directory ") << zipDirPath;
        return error(tmpDirPath);
    }

    const QString descname(zipDirPath+QDir::separator()+"Description.txt");
    QFile desc(descname);
    if (!desc.open(QIODevice::WriteOnly)) {
        qWarning() << tr("medCompositeDataSets: cannot open file ") << descname.toLocal8Bit().constData();
        return error(tmpDirPath);
    }
    QTextStream out(&desc);

    writer = MedInria::medCompositeDataSetsBase::find(data());
    if (!writer) {
        //emit showError(this, tr ("Could not write this data type: ")+data()->description(), 5000);
        qWarning() << tr("Could not write this data type: ")+data()->identifier();
        return error(tmpDirPath);
    }

    out << "# MEDINRIA COMPOSITE DATA: " << writer->tag() << ' ' << writer->version() << '\n';

    writer->write_description(out);
    out.flush();
    desc.close();
    writer->write_data(zipDirPath);

    //  Zip the temporary directory.

    dtkZipWriter zip(path);
    if (zip.status()!=dtkZipWriter::NoError)
        return false;

    foreach(QFileInfo file,QDir(zipDirPath).entryInfoList()) {

        if (file.isDir()) {
            if (file.fileName()!="..") {
                const QFile::Permissions& perms = zip.creationPermissions();
                zip.setCreationPermissions(QFile::ReadOwner|QFile::WriteOwner|QFile::ExeOwner);
                zip.addDirectory(file.dir().dirName());
                zip.setCreationPermissions(perms);
            }
            continue;
        }

        QFile inFile(zipDirPath+QDir::separator()+file.fileName());
        if (!inFile.open(QIODevice::ReadOnly)) {
            qWarning("medCompositeDataSets: inFile.open(): %s",inFile.errorString().toLocal8Bit().constData());
            return error(tmpDirPath);
        }

        zip.addFile(zipdirn+QDir::separator()+file.fileName(),&inFile);
        inFile.close();
    }
    zip.close();

    //RemoveDirectory(QDir(tmpDirPath));
    return true;
}
