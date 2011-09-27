#include <string>
#include <sstream>

#include <IOUtils.H>
#include <dirTools.h>

#include <dtkZip/dtkZip.h>
#include <dtkZip/dtkZipFile.h>

#include <medCompositeDataSetsReader.h>

bool medCompositeDataSetsReader::canRead(const QString& path) {

    if (desc!=0) {
        if (reader!=0)
            return true;
        cleanup();
    }

    //  Check if the file is a zip file.

    dtkZip zipfile;

    zipfile.setZipName(path);
    const QString dname = zip_dirname(path)+"/Description.txt";

    QString descname(path);
    if (zipfile.open(dtkZip::mdUnzip) && zipfile.setCurrentFile(dname)) {

        //  OK this is a zip archive and the file Description.txt is in the zip archive.
        //  Uncompress the archive in a temporary directory.

        dtkZipFile file(&zipfile);
        tmpdir = QString(mkdtemp((QDir::tempPath()+"/"+"medcdsXXXXXXX").toAscii().data()))+"/";
        is_zip_file = true;
        for (bool more=zipfile.goToFirstFile();more;more=zipfile.goToNextFile()) {

            if (!file.open(QIODevice::ReadOnly)) {
                qWarning("medCompositeDataSets: file.open(): %d",file.getZipError());
                cleanup();
                return false;
            }

            QString name = file.getActualFileName();

            if (file.getZipError()!=UNZ_OK) {
                qWarning("medCompositeDataSets: file.getFileName(): %d",file.getZipError());
                cleanup();
                return false;
            }

            const QString& dirn = tmpdir+name;

            if (name.contains('/'))
                QDir().mkpath(QFileInfo(dirn).dir().path());

            QFile out;
            out.setFileName(dirn);
            out.open(QIODevice::WriteOnly);

            char buf[4096];
            int len = 0;

            char c;
            while (file.getChar(&c)) {
                buf[len++] = c;

                if (len >= 4096) {
                    out.write(buf,len);
                    len = 0;
                }
            }

            if (len>0)
                out.write(buf,len);

            out.close();

            if (file.getZipError()!=UNZ_OK) {
                qWarning("medCompositeDataSets: file.getFileName(): %d", file.getZipError());
                cleanup();
                return false;
            }

            if(!file.atEnd()) {
                qWarning("medCompositeDataSets: read all but not EOF");
                cleanup();
                return false;
            }

            file.close();

            if(file.getZipError()!=UNZ_OK) {
                qWarning("medCompositeDataSets: file.close(): %d", file.getZipError());
                cleanup();
                return false;
            }
        }

        zipfile.close();
        descname = tmpdir+dname;
    }
        
    basedir = QFileInfo(descname).dir().path();

    //  Assume a simple text file.

    desc = new QFile(descname);
    if (!desc->open(QIODevice::ReadOnly)) {
        cleanup();
        return false;
    }

    //  Read the first line of the description.

    char header[1024];
    desc->readLine(header,sizeof(header));

    //  Verify the header and extract the file type and version.

    std::string type;
    unsigned    major,minor;

    std::istringstream iss(header);
    iss >> io_utils::match("# MEDINRIA COMPOSITE DATA:") >> type >> major >> io_utils::match('.') >> minor;

    //  Verify that there is a manager form this type and version.

    reader = MedInria::medCompositeDataSetsBase::known(type,major,minor);

    setData(reader);

    return reader!=0;
}

void medCompositeDataSetsReader::readInformation(const QString&) {
    QByteArray buffer = desc->readAll();
    in_error = reader->read_description(buffer);
}

bool medCompositeDataSetsReader::read(const QString& path) {

    //  Read the type information.

    this->setProgress(0);
    this->readInformation(path);

    if (in_error)
        return false;

    this->setProgress(20);

    //  Create the final data object.
    //  How to set progress in read_data ??

    return reader->read_data(basedir);
}

void medCompositeDataSetsReader::setProgress(const int value) {
    qDebug() << "setProgress" << value;
}
