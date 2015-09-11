/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <iostream>
#include <string>
#include <sstream>

#include <IOUtils.H>
#include <dirTools.h>

#include <dtkZip/dtkZipReader.h>
#include <medCompositeDataSetsReader.h>

bool medCompositeDataSetsReader::canRead(const QString& path) {

    if (desc!=0) {
        if (reader!=0)
            return true;
        cleanup();
    }

    QString descname(path);

    //  Check if the file is a zip file.

    dtkZipReader zip(path,QIODevice::ReadOnly);
    if (zip.status()==dtkZipReader::NoError) {

        //  Check that the file Description.txt exists.

        const QString& dname = zip_dirname(path)+QDir::separator()+"Description.txt";

        const QList<dtkZipReader::FileInfo>& files = zip.fileInfoList();
        bool found = false;
        for (QList<dtkZipReader::FileInfo>::const_iterator i=files.begin();i!=files.end();++i)
            if (i->filePath==dname) {
                found = true;
                break;
            }
        if (!found)
            return false;

        const QString& uuid = QUuid::createUuid().toString().replace("{","").replace("}","");
        tmpdir = QDir::tempPath()+QDir::separator()+"medcds"+uuid+QDir::separator();
        descname = tmpdir+dname;

        zip.extractAll(tmpdir);
        is_zip_file = true;

    } else {
        descname = descname+QDir::separator()+"Description.txt";
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

bool medCompositeDataSetsReader::readInformation(const QString&) {
    QByteArray buffer = desc->readAll();
    in_error = reader->read_description(buffer);
    return in_error;
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
    //qDebug() << "setProgress" << value;
}
