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

    zipfile.setZipName(path);
    qDebug() <<  dirname(path).append("/Description.txt");
    if (zipfile.open(dtkZip::mdUnzip) && zipfile.setCurrentFile(dirname(path).append("/Description.txt"))) {

        //  OK this is a zip archive and the file Description.txt is in the zip archive.

        zipdesc = new dtkZipFile(&zipfile);
        if (!zipdesc->open(QIODevice::ReadOnly) || zipdesc->getZipError()!=UNZ_OK) {
            qWarning("Corrupted composite data set zip file: %d",zipfile.getZipError());
            return false;
        }

        is_zip_archive = true;
        desc           = zipdesc;

        return is_valid_desc();
    }
        
    //  Assume a simple text file.

    is_zip_archive = false;

    desc = new QFile(path);
    if (!desc->isOpen())
        return false;

    return is_valid_desc();
}

void medCompositeDataSetsReader::readInformation(const QString&) {
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

bool medCompositeDataSetsReader::is_valid_desc() {

    //  Read the first line of the description.

    char header[1024];
    desc->readLine(header,sizeof(header));

    //  Verify the header and extract the file type and version.

    std::string type;
    unsigned    version;

    std::istringstream iss(header);
    iss >> io_utils::match("# MEDINRIA COMPOSITE DATA:") >> type >> version;

     std::cerr << type << version << std::endl;
 
    //  Verify that there is a manager form this type and version.

    MedInria::medCompositeDataSetsBase* reader = MedInria::medCompositeDataSetsBase::known(type,version);
    if (reader) {
        return true;
    }

    return false;
}

void medCompositeDataSetsReader::setProgress(int value) {
}
