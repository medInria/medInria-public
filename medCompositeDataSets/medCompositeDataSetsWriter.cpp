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
    const QString& tmpDirn = "medcdsXXXXXX/";
    const QString& tmpDirPath =QDir::cleanPath(
            systemTmpDir.absolutePath()+"/"+tmpDirn);
    const QString& zipDirPath = tmpDirPath+"/"+zipdirn;

    if (!systemTmpDir.mkpath(zipDirPath))
    {
        qWarning() << tr("medCompositeDataSets: cannot create directory ")
                      << zipDirPath;
        return error(tmpDirPath);
    }

    const QString descname(zipDirPath+"/Description.txt");
    QFile desc(descname);
    if (!desc.open(QIODevice::WriteOnly)) {
        qWarning() << tr("medCompositeDataSets: cannot open file ") << descname.toLocal8Bit().constData();
        return error(tmpDirPath);
    }
    QTextStream out(&desc);

    writer = MedInria::medCompositeDataSetsBase::find(data());
    if (!writer) {
        //emit showError(this, tr ("Could not write this data type: ")+data()->description(), 5000);
        qWarning() << tr ("Could not write this data type: ")+data()->identifier();
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

        if (!file.isFile())
            continue;

        QFile inFile(zipDirPath+"/"+file.fileName());
        if(!inFile.open(QIODevice::ReadOnly)) {
            qWarning("medCompositeDataSets: inFile.open(): %s",inFile.errorString().toLocal8Bit().constData());
            return error(tmpDirPath);
        }

        zip.addFile(zipdirn+"/"+file.fileName(),&inFile);
        inFile.close();
    }
    zip.close();

    RemoveDirectory(QDir(tmpDirPath));
    return true;
}
