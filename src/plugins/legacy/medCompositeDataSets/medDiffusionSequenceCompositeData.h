/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractData.h>
#include <medCompositeDataSetsPluginExport.h>
#include <medCompositeDataSetsBase.h>
#include <itkGradientFileReader.h>

class medDiffusionSequenceCompositeDataToolBox;

class MEDCOMPOSITEDATASETSPLUGIN_EXPORT medDiffusionSequenceCompositeData: public MedInria::medCompositeDataSetsBase {

    typedef itk::GradientFileReader            GradientReaderType;
    typedef QVector<dtkAbstractData*>          Volumes;

public:

    typedef GradientReaderType::VectorType     GradientType;
    typedef GradientReaderType::VectorListType GradientListType;

    friend class medDiffusionSequenceCompositeDataToolBox;

    medDiffusionSequenceCompositeData():
        MedInria::medCompositeDataSetsBase(Tag,this),major_vers(defaultMajorVersion),minor_vers(defaultMinorVersion),meta_data_index(0) { }
    virtual ~medDiffusionSequenceCompositeData() { }

    virtual QString description() const;
    virtual QString identifier()  const { return ID; }

    virtual bool has_version(const unsigned major,const unsigned minor) const { return major==defaultMajorVersion; }

    virtual MedInria::medCompositeDataSetsBase* clone(const int major,const int minor) const {
        const unsigned maj_version = (major==-1) ? defaultMajorVersion : major;
        const unsigned min_version = (minor==-1) ? defaultMinorVersion : minor;
        return new medDiffusionSequenceCompositeData(maj_version,min_version);
    }

    virtual QString tag()     const { return Tag;                                                         }
    virtual QString version() const { return QString::number(major_vers)+"."+QString::number(minor_vers); }

    bool registered() const;

    virtual bool read_description(const QByteArray& buf);
    virtual bool read_data(const QString&);

    virtual bool write_description(QTextStream& file);
    virtual bool write_data(const QString&);

    virtual QImage& thumbnail() { return images[meta_data_index]->thumbnail(); }

    //  Methods specific to this type.

    static dtkAbstractData* readVolume(const QString& path);

    void readVolumes(const QStringList& paths,const bool add_to_image_list=false);
    void readVolumes(const QString& dirname,const QStringList& paths);
    void writeVolumes(const QString& dirname,const QStringList& paths) const;

    void setGradientList(const GradientListType& grads) { gradients = grads; }
    void setVolumeList(const Volumes& vols)             { images = vols;     }

private:

    medDiffusionSequenceCompositeData(const unsigned major,const unsigned minor): MedInria::medCompositeDataSetsBase(Tag,this),major_vers(major),minor_vers(minor) { }

    const unsigned   major_vers;
    const unsigned   minor_vers;
    QStringList      image_list;
    Volumes          images;
    GradientListType gradients;
    unsigned         meta_data_index;

    static dtkAbstractData* create() { return new medDiffusionSequenceCompositeData; }

    static const medDiffusionSequenceCompositeData proto;

    static const char     ID[];

    static const char     Tag[];
    static const char     ImagesString[];
    static const char     MetaDataString[];

    static const unsigned defaultMajorVersion = 1;
    static const unsigned defaultMinorVersion = 0;
};

dtkAbstractData* createDiffusionSequenceCompositeData();


