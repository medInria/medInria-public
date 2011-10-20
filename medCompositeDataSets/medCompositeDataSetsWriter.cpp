#include <cstdlib>
#include <cstdarg>
#include <sys/stat.h>
#include <sys/types.h>

#include <medCompositeDataSetsWriter.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <dtkZip/dtkZipWriter.h>

#include <medCompositeDataSetsBase.h>
#include <dirTools.h>

static bool error(const QString& dirname) {
    RemoveDirectory(QDir(dirname));
    return false;
}

bool medCompositeDataSetsWriter::write(const QString& path) {

    //  Write the data in a temporary directory.

    const QString& dirn   = zip_dirname(path);
    const QString& tmpnam = mkdtemp((QDir::tempPath()+"/"+"medcdsXXXXXX").toAscii().data());
    const QString& tmpdir = tmpnam+"/"+dirn;
    if (mkdir(tmpdir.toAscii().data(),0700)) {
        qWarning() << tr("medCompositeDataSets: cannot create directory ") << tmpdir.toLocal8Bit().constData();
        return error(tmpnam);
    }

    const QString descname(tmpdir+"/Description.txt");
    QFile desc(descname);
    if (!desc.open(QIODevice::WriteOnly)) {
        qWarning() << tr("medCompositeDataSets: cannot open file ") << descname.toLocal8Bit().constData();
        return error(tmpnam);
    }
    QTextStream out(&desc);

    writer = MedInria::medCompositeDataSetsBase::find(data());
    if (!writer) {
        //emit showError(this, tr ("Could not write this data type: ")+data()->description(), 5000);
        qWarning() << tr ("Could not write this data type: ")+data()->identifier();
        return error(tmpnam);
    }

    out << "# MEDINRIA COMPOSITE DATA: " << writer->tag() << ' ' << writer->version() << '\n';

    writer->write_description(out);
    out.flush();
    desc.close();
    writer->write_data(tmpdir);

    //  Zip the temporary directory.

    dtkZipWriter zip(path);
    if (zip.status()!=dtkZipWriter::NoError)
        return false;

    foreach(QFileInfo file,QDir(tmpdir).entryInfoList()) {

        if (!file.isFile())
            continue;

        QFile inFile(tmpdir+"/"+file.fileName());
        if(!inFile.open(QIODevice::ReadOnly)) {
            qWarning("medCompositeDataSets: inFile.open(): %s",inFile.errorString().toLocal8Bit().constData());
            return error(tmpnam);
        }

        zip.addFile(dirn+"/"+file.fileName(),&inFile);
        inFile.close();
    }
    zip.close();

    RemoveDirectory(QDir(tmpnam));
    return true;
}
