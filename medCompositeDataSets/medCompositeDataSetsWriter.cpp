#include <cstdlib>
#include <cstdarg>
#include <sys/stat.h>
#include <sys/types.h>

#include <medCompositeDataSetsWriter.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <dtkZip/dtkZip.h>
#include <dtkZip/dtkZipFile.h>

#include <medCompositeDataSetsBase.h>
#include <dirTools.h>

static bool error(const QString& dirname) {
    RemoveDirectory(QDir(dirname));
    return false;
}

bool medCompositeDataSetsWriter::write(const QString& path) {

    //  Write the data in a temporary directory.

    const QString& dirn   = zip_dirname(path);
    const QString& tmpnam = QDir::tempPath()+"/"+"medcdsXXXXXX";
    const QString& tmpdir = QString(mkdtemp(tmpnam.toAscii().data()))+"/"+dirn;
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
        qWarning() << tr ("Could not write this data type: ")+data()->description();
        return error(tmpnam);
    }

    out << "# MEDINRIA COMPOSITE DATA: " << writer->tag() << ' ' << writer->version() << '\n';

    writer->write_description(out);
    out.flush();
    desc.close();
    writer->write_data(tmpdir);

    //  Zip the temporary directory.

    QFile zipFile(path);
    dtkZip zip(&zipFile);

    if (!zip.open(dtkZip::mdCreate)) {
        qWarning("CompositeDatasets: zip.open(): %d",zip.getZipError());
        return error(tmpnam);
    }
    zip.setComment("Composite data set");

    dtkZipFile outFile(&zip);

    foreach(QFileInfo file,QDir(tmpdir).entryInfoList()) {

        if (!file.isFile())
            continue;

        QFile inFile(tmpdir+"/"+file.fileName());
        if(!inFile.open(QIODevice::ReadOnly)) {
            qWarning("medCompositeDataSets: inFile.open(): %s",inFile.errorString().toLocal8Bit().constData());
            return error(tmpnam);
        }

        if(!outFile.open(QIODevice::WriteOnly,dtkZipNewInfo(dirn+"/"+file.fileName(),inFile.fileName()))) {
            qWarning("medCompositeDataSets: outFile.open(): %d",outFile.getZipError());
            return error(tmpnam);
        }

        qint64 len = inFile.size();
        qint64 pos = 0;

        char c;
        while (inFile.getChar(&c) && outFile.putChar(c)) {

            char buf[4096];
            qint64 l = inFile.read(buf,4096);

            if (l<0) {
                qWarning("read(): %s",inFile.errorString().toUtf8().constData());
                break;
            }

            if (l==0)
                break;

            if (outFile.write(buf,l)!=l) {
                qWarning("write(): %d",outFile.getZipError());
                break;
            }

            pos += l;

            if (pos%1048576==0)
                qDebug("%.1f",(float) pos/len*100.0f);
        }

        if(outFile.getZipError()!=UNZ_OK) {
            qWarning("medCompositeDataSets: outFile.putChar(): %d",outFile.getZipError());
            return error(tmpnam);
        }

        outFile.close();

        if(outFile.getZipError()!=UNZ_OK) {
            qWarning("medCompositeDataSets: outFile.close(): %d",outFile.getZipError());
            return error(tmpnam);
        }

        inFile.close();
    }

    zip.close();

    RemoveDirectory(QDir(tmpnam));
    if(zip.getZipError()!=0) {
        qWarning("medCompositeDataSets: zip.close(): %d",zip.getZipError());
        return false;
    }

    return true;
}
