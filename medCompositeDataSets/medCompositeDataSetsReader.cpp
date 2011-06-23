#include <string>
#include <sstream>

#include <IOUtils.H>

#include <medCompositeDataSetsReader.h>

QString dirname(const QString& name) {
    const unsigned ind1 = name.lastIndexOf('/')+1;
    const unsigned ind2 = name.lastIndexOf('.');
    return name.mid(ind1,ind2-ind1);
}

QString medCompositeDataSetsReader::description(void) const {
}

QStringList medCompositeDataSetsReader::handled(void) const {
}

dtkAbstractData* medCompositeDataSetsReader::data(void) {
}

void medCompositeDataSetsReader::setData(dtkAbstractData *data) {
}

bool medCompositeDataSetsReader::canRead(const QString& path) {

    //  Check if the file is a zip file.

    dtkZip zipfile;

    zipfile.setZipName(path);
    dname = dirname(path);
    QString descname(path);

    if (zipfile.open(dtkZip::mdUnzip) && zipfile.setCurrentFile(dname.append("/Description.txt"))) {

        //  OK this is a zip archive and the file Description.txt is in the zip archive.
        //  Uncompress the archive in a temporary directory.

        dtkZipFile file(&zipfile);
        const QString outdir = QString(mkdtemp((QDir::tempPath()+"/"+"medcdsXXXXXXX").toAscii().data()))+"/";
        for (bool more=zipfile.goToFirstFile();more;more=zipfile.goToNextFile()) {

            if (!file.open(QIODevice::ReadOnly)) {
                qWarning("medCompositeDataSet: file.open(): %d",file.getZipError());
                return false;
            }

            QString name = file.getActualFileName();

            if (file.getZipError()!=UNZ_OK) {
                qWarning("medCompositeDataSet: file.getFileName(): %d",file.getZipError());
                return false;
            }

            QString dirn = outdir+name;

            if (name.contains('/')) {
                dirn.chop(dirn.length()-dirn.lastIndexOf("/"));
                QDir().mkpath(dirn);
            }

            QFile out;
            out.setFileName(outdir+name);
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
                qWarning("medCompositeDataSet: file.getFileName(): %d", file.getZipError());
                return false;
            }

            if(!file.atEnd()) {
                qWarning("medCompositeDataSet: read all but not EOF");
                return false;
            }

            file.close();

            if(file.getZipError()!=UNZ_OK) {
                qWarning("medCompositeDataSet: file.close(): %d", file.getZipError());
                return false;
            }
        }

        zipfile.close();
        descname = outdir+dname;
    }
        
    //  Assume a simple text file.

    desc = new QFile(descname);
    if (!desc->open(QIODevice::ReadOnly))
        return false;

    //  Read the first line of the description.

    char header[1024];
    desc->readLine(header,sizeof(header));

    //  Verify the header and extract the file type and version.

    std::string type;
    unsigned    version;

    std::istringstream iss(header);
    iss >> io_utils::match("# MEDINRIA COMPOSITE DATA:") >> type >> version;

    //  Verify that there is a manager form this type and version.

    reader = MedInria::medCompositeDataSetsBase::known(type,version);
    return reader!=0;
}

void medCompositeDataSetsReader::readInformation(const QString&) {
    QByteArray buffer = desc->readAll();
    reader->read_description(buffer);
}

bool medCompositeDataSetsReader::read(const QString& path) {

    //  Read the type information.

    this->setProgress(0);
    this->readInformation(path);
    this->setProgress(50);

    //  Create the final data object.

    //  TODO: add code here....

    this->setProgress (100);

    return true;
}

void medCompositeDataSetsReader::setProgress(int value) {
}
