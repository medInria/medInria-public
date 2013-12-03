/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <sstream>

#include <medMetaDataKeys.h>
#include <medDataReaderWriter.h>
#include <medMessageController.h>

#include <medDiffusionSequenceCompositeData.h>
#include <medDiffusionSequenceCompositeDataToolBox.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>

#include <IOUtils.H>

// /////////////////////////////////////////////////////////////////
// medDiffusionSequenceCompositeData
// /////////////////////////////////////////////////////////////////

const medDiffusionSequenceCompositeData medDiffusionSequenceCompositeData::proto;

const char medDiffusionSequenceCompositeData::ID[] = "medDiffusionSequenceCompositeData";
const char medDiffusionSequenceCompositeData::Tag[] = "DWI";
const char medDiffusionSequenceCompositeData::ImagesString[] = "Images:";
const char medDiffusionSequenceCompositeData::MetaDataString[] = "MetaData:";

bool medDiffusionSequenceCompositeData::registered() const {
    return dtkAbstractDataFactory::instance()->registerDataType(ID,create) &&
           medDiffusionSequenceCompositeDataToolBox::registered();
}

QString medDiffusionSequenceCompositeData::description() const {
    return "Composite dataset for diffusion sequence";
}

bool medDiffusionSequenceCompositeData::read_description(const QByteArray& buf) {
    const std::string description(buf.data());
    std::istringstream iss(description);

    unsigned num;
    iss >> io_utils::skip_comments('#') >> io_utils::match(ImagesString) >> num;

    //  Decode the index specifying which meta data to associate with the dataset.

    bool has_metadata;
    iss >> io_utils::match_optional(MetaDataString,has_metadata);
    if (has_metadata)
        iss >> meta_data_index;
    if (meta_data_index>=num)
        qWarning("medDiffusionSequence: invalid metadata index %d",meta_data_index);

    //  Read the gradient image names and the associated gradient.

    for (unsigned i=0;i<num;++i) {
        std::string name;
        GradientType V;
        iss >> io_utils::skip_comments('#') >> io_utils::filename(name) >> io_utils::match('[') >> V[0] >> V[1] >> V[2] >> io_utils::match(']');
        gradients.push_back(V);
        image_list << QString(name.c_str());
    }

    return (iss.fail()) ? true : false;
}

bool medDiffusionSequenceCompositeData::write_description(QTextStream& out) {

    if (image_list.size()!=images.size()) {
        qWarning("medDiffusionSequence: incoherent dataset %d!=%d",image_list.size(),images.size());
        return false;
    }

    const unsigned num = images.size();
    out << ImagesString << num << ' ' << MetaDataString << ' ' << meta_data_index << '\n';
    for (unsigned i=0;i<num;++i) {
        const GradientType V = gradients[i];
        const QString im_name = image_list[i];
        out << '"' << im_name << '"' << " [" << V[0] << ' ' << V[1] << ' ' << V[2] << "]\n";
    }

    return true;
}

bool medDiffusionSequenceCompositeData::write_data(const QString& dirname) {
    writeVolumes(dirname,image_list);
    return true;
}

dtkAbstractData* medDiffusionSequenceCompositeData::readVolume(const QString& path) {
    dtkAbstractData* volume = medDataReaderWriter::read(path);
    if (!volume)
    {
        //Why is this method static? using it to send messages sounded like a good idea
//        emit medCompositeDataSetImporterSelectorToolBox::showError(this,
//                        tr("This file is not readable by this Importer"));
        return NULL;
    }
    const QString&   type   = volume->name();

    if (!type.contains("Image")) {
        // emit medCompositeDataSetImporterSelectorToolBox::showError (this, tr ("file does not describe any known image type"), 3000);
        return 0;
    }

#if 0
    if (type.contains("Image4D")) {
        // emit medCompositeDataSetImporterSelectorToolBox::showError (this, tr ("4D image is not supported yet"), 3000);
        return 0;
    }

    if (!type.contains("Image3D")) {
        // emit medCompositeDataSetImporterSelectorToolBox::showError (this, tr ("image should be 3D"), 3000);
        return 0;
    }
#endif

    return volume;
}

void medDiffusionSequenceCompositeData::readVolumes(const QStringList& paths,const bool add_to_image_list) {

    QList<QString> readers = dtkAbstractDataFactory::instance()->readers();

    for (int i=0;i<paths.size();++i) {
        const QString& filepath = paths[i];
        dtkAbstractData* volume = readVolume(filepath);
        if (!volume)
        {
            //Warning should have been sent by the readVolume method by now,
            //so no need for further warning.
            continue;
        }
        images.push_back(volume);

        if (add_to_image_list) {
            const QFileInfo& info = QFileInfo(filepath);
            image_list << info.fileName();
        }

        //  To be changed (and the push_back(s) above might be moved after the checks).

        QString identifier = volume->identifier();
        if (!identifier.contains("Image")) {
            // emit medCompositeDataSetImporterSelectorToolBox::showError(this,tr("file does not describe any known image type"),3000);
            continue;
        }
        if (identifier.contains ("Image4D")) {
            // emit medCompositeDataSetImporterSelectorToolBox::showError(this,tr("4D image is not supported yet"),3000);
            continue;
        }
        if (!identifier.contains ("Image3D")) {
            // emit medCompositeDataSetImporterSelectorToolBox::showError(this,tr("image should be 3D"),3000);
            continue;
        }
    }

    for (medMetaDataKeys::Key::Registery::const_iterator i=medMetaDataKeys::Key::all().begin();i!=medMetaDataKeys::Key::all().end();++i)
        if ((*i)->is_set_in(images[meta_data_index])) {
            (*i)->set(this,(*i)->getValues(images[meta_data_index]));
            //qDebug() << "MetaData: " << (*i)->key() << (*i)->getValues(images[meta_data_index]);
        }
}

void medDiffusionSequenceCompositeData::readVolumes(const QString& dirname,const QStringList& paths) {
    QStringList filelist;
    for (int i=0;i<paths.size();++i) {
        const QString filepath = dirname+"/"+paths[i];
        filelist << filepath;
    }
    readVolumes(filelist);
}

void medDiffusionSequenceCompositeData::writeVolumes(const QString& dirname,const QStringList& paths) const {
    for (int i=0;i<paths.size();++i) {
        const QString& filepath = dirname+"/"+paths[i];
        medDataReaderWriter::write(filepath,images[i]);
    }
}

bool medDiffusionSequenceCompositeData::read_data(const QString& dirname) {
    readVolumes(dirname,image_list); // TODO: Error management....
    return true;
}
