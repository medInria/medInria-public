#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vector>
#include <medAbstractData.h>

#include <medCoreLegacyExport.h>

namespace medMetaDataKeys
{
    /**
    * @class Key
    * @author John Stark, modified by papadop
    * It allows compile-time verification that the keyword is correct.
    */

    class MEDCORELEGACY_EXPORT Key
    {
    public:
        typedef std::vector<const Key*> Registery;

        Key(QString name, QString label="",
            QVariant::Type type=QVariant::String, bool isEditable = true): KEY(name), LABEL(label), TYPE(type), ISEDITABLE(isEditable)
        {
            if(label == "")
            {
                LABEL = name;
            }
            registery.push_back(this);
        }

        ~Key() { }

        const QString& key() const { return KEY; }
        const QString& label() const { return LABEL; }
        const QVariant::Type& type() const { return TYPE; }
        bool isEditable() const { return ISEDITABLE; }

        bool is_set_in(const medAbstractData *data) const { return data->hasMetaData(KEY) ; }

        const QStringList getValues(const medAbstractData *data) const { return data->metaDataValues(KEY); }

        const QString getFirstValue(const medAbstractData *data, const QString defaultValue=QString("")) const
        {
            return  data->hasMetaData(KEY) ? data->metaDataValues(KEY)[0] : defaultValue;
        }

        void add(medAbstractData* d,const QStringList& values) const { d->addMetaData(KEY,values); }
        void add(medAbstractData* d,const QString& value)      const { d->addMetaData(KEY,value);  }
        void set(medAbstractData* d,const QStringList& values) const { d->setMetaData(KEY,values); }
        void set(medAbstractData* d,const QString& value)      const { d->setMetaData(KEY,value);  }

        static const Registery& all() { return registery; }

        bool operator==(const Key& other){ return ( this->key() == other.key() ); }

        static const Key* fromKeyName(QString name)
        {
            std::vector<const Key*>::iterator it;
            for ( it=registery.begin() ; it < registery.end(); it++ )
            {
                if( (*it)->key() == name )
                    return *it;
            }
            return nullptr;
        }

    private:

        static Registery registery;

        const QString KEY;
        QString LABEL;
        QVariant::Type TYPE;
        bool ISEDITABLE;
    };


    /** Define the actual keys to use */

    extern MEDCORELEGACY_EXPORT const Key TransferSyntaxUID;
    extern MEDCORELEGACY_EXPORT const Key ContainsBasicInfo;

    // PATIENT
    extern MEDCORELEGACY_EXPORT const Key PatientID;
    extern MEDCORELEGACY_EXPORT const Key PatientName;
    extern MEDCORELEGACY_EXPORT const Key Age;
    extern MEDCORELEGACY_EXPORT const Key BirthDate;
    extern MEDCORELEGACY_EXPORT const Key Gender;
    extern MEDCORELEGACY_EXPORT const Key Description;

    // STUDY
    extern MEDCORELEGACY_EXPORT const Key StudyID;
    extern MEDCORELEGACY_EXPORT const Key StudyInstanceUID;
    extern MEDCORELEGACY_EXPORT const Key StudyDescription;
    extern MEDCORELEGACY_EXPORT const Key Institution;
    extern MEDCORELEGACY_EXPORT const Key Referee;
    extern MEDCORELEGACY_EXPORT const Key StudyDate;
    extern MEDCORELEGACY_EXPORT const Key StudyTime;

    // SERIES
    extern MEDCORELEGACY_EXPORT const Key SeriesID;
    extern MEDCORELEGACY_EXPORT const Key SeriesInstanceUID;
    extern MEDCORELEGACY_EXPORT const Key SeriesStoreId;
    extern MEDCORELEGACY_EXPORT const Key SeriesNumber;
    extern MEDCORELEGACY_EXPORT const Key Modality;
    extern MEDCORELEGACY_EXPORT const Key Performer;
    extern MEDCORELEGACY_EXPORT const Key Report;
    extern MEDCORELEGACY_EXPORT const Key Protocol;
    extern MEDCORELEGACY_EXPORT const Key SeriesDescription;
    extern MEDCORELEGACY_EXPORT const Key SeriesDate;
    extern MEDCORELEGACY_EXPORT const Key SeriesTime;
    extern MEDCORELEGACY_EXPORT const Key SeriesThumbnail;

    // IMAGE
    extern MEDCORELEGACY_EXPORT const Key SOPInstanceUID;
    extern MEDCORELEGACY_EXPORT const Key Columns;
    extern MEDCORELEGACY_EXPORT const Key Rows;
    extern MEDCORELEGACY_EXPORT const Key Dimensions;
    extern MEDCORELEGACY_EXPORT const Key NumberOfDimensions;
    extern MEDCORELEGACY_EXPORT const Key Orientation;
    extern MEDCORELEGACY_EXPORT const Key Origin;
    extern MEDCORELEGACY_EXPORT const Key SliceThickness;
    extern MEDCORELEGACY_EXPORT const Key PatientOrientation;
    extern MEDCORELEGACY_EXPORT const Key PatientPosition;
    extern MEDCORELEGACY_EXPORT const Key ImportationDate;
    extern MEDCORELEGACY_EXPORT const Key AcquisitionDate;
    extern MEDCORELEGACY_EXPORT const Key AcquisitionTime;
    extern MEDCORELEGACY_EXPORT const Key Comments;
    extern MEDCORELEGACY_EXPORT const Key FilePaths;
    extern MEDCORELEGACY_EXPORT const Key Status;
    extern MEDCORELEGACY_EXPORT const Key SequenceName;
    extern MEDCORELEGACY_EXPORT const Key Size;
    extern MEDCORELEGACY_EXPORT const Key VolumeUID;
    extern MEDCORELEGACY_EXPORT const Key Spacing;
    extern MEDCORELEGACY_EXPORT const Key XSpacing;
    extern MEDCORELEGACY_EXPORT const Key YSpacing;
    extern MEDCORELEGACY_EXPORT const Key ZSpacing;
    extern MEDCORELEGACY_EXPORT const Key NumberOfComponents;
    extern MEDCORELEGACY_EXPORT const Key ComponentType;
    extern MEDCORELEGACY_EXPORT const Key PixelType;
    extern MEDCORELEGACY_EXPORT const Key medDataType;
    extern MEDCORELEGACY_EXPORT const Key PreferredDataReader;
    extern MEDCORELEGACY_EXPORT const Key ImageID;
    extern MEDCORELEGACY_EXPORT const Key ImageType;
    extern MEDCORELEGACY_EXPORT const Key ThumbnailPath;
    extern MEDCORELEGACY_EXPORT const Key AcquisitionNumber;

    // Frame of reference
    extern MEDCORELEGACY_EXPORT const Key FrameOfReferenceUID;
    extern MEDCORELEGACY_EXPORT const Key PositionReferenceIndicator;

    // EQUIPEMENT
    extern MEDCORELEGACY_EXPORT const Key Manufacturer;

    // CT
    extern MEDCORELEGACY_EXPORT const Key KVP;

    // MR Image
    extern MEDCORELEGACY_EXPORT const Key FlipAngle;
    extern MEDCORELEGACY_EXPORT const Key EchoTime;
    extern MEDCORELEGACY_EXPORT const Key RepetitionTime;

    // EXPORT EXTRA DATA TO ATTACHED FILE
    extern MEDCORELEGACY_EXPORT const Key Toolbox;
    extern MEDCORELEGACY_EXPORT const Key OriginalDataUID;
    extern MEDCORELEGACY_EXPORT const Key OriginalDataDesc;
    extern MEDCORELEGACY_EXPORT const Key FileMetadataPath;
};



