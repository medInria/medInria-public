/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <vector>
#include <medAbstractData.h>
#include <medCoreExport.h>

namespace medMetaDataKeys {

    /**
    * @class Key
    * @author John Stark, modified by papadop
    * It allows compile-time verification that the keyword is correct.
    */

    class MEDCORE_EXPORT Key {
    public:
        typedef std::vector<const Key*> Registery;

        Key(const char* name, const char* label="",
            QVariant::Type type=QVariant::String, bool isEditable = true): KEY(name), LABEL(label), TYPE(type), ISEDITABLE(isEditable)
        {
            if(QString(label)=="") LABEL=QString(name);
            registery.push_back(this);
        }

        ~Key() { }

        const QString& key() const { return KEY; }
        const QString& label() const { return LABEL; }
        const QVariant::Type& type() const { return TYPE; }
        bool isEditable() const { return ISEDITABLE; }

        bool is_set_in(const medAbstractData *data) const { return data->hasMetaData(KEY) ; }

        const QStringList getValues(const medAbstractData *data) const { return data->metaDataValues(KEY); }

        const QString getFirstValue(const medAbstractData *data, const QString defaultValue=QString()) const
        {
            return  data->hasMetaData(KEY) ? data->metaDataValues(KEY)[0] : defaultValue;
        }

        void add(medAbstractData* d,const QStringList& values) const { d->addMetaData(KEY,values); }
        void add(medAbstractData* d,const QString& value)      const { d->addMetaData(KEY,value);  }
        void set(medAbstractData* d,const QStringList& values) const { d->setMetaData(KEY,values); }
        void set(medAbstractData* d,const QString& value)      const { d->setMetaData(KEY,value);  }

        static const Registery& all() { return registery; }

        bool operator==(const Key& other){ return ( this->key() == other.key() ); }

        static const Key* fromKeyName(const char* name)
        {
            std::vector<const Key*>::iterator it;
            for ( it=registery.begin() ; it < registery.end(); it++ )
            {
                if( (*it)->key() == name )
                    return *it;
            }
            return NULL;
        }


    private:

        static Registery registery;

        const QString KEY;
        QString LABEL;
        QVariant::Type TYPE;
        bool ISEDITABLE;
    };


    /** Define the actual keys to use */

    extern MEDCORE_EXPORT const Key TransferSyntaxUID;
    extern MEDCORE_EXPORT const Key ContainsBasicInfo;

    // PATIENT
    extern MEDCORE_EXPORT const Key PatientID;
    extern MEDCORE_EXPORT const Key PatientName;
    extern MEDCORE_EXPORT const Key Age;
    extern MEDCORE_EXPORT const Key BirthDate;
    extern MEDCORE_EXPORT const Key Gender;
    extern MEDCORE_EXPORT const Key Description; //what?

    // STUDY
    extern MEDCORE_EXPORT const Key StudyID;
    extern MEDCORE_EXPORT const Key StudyDicomID;
    extern MEDCORE_EXPORT const Key StudyDescription;
    extern MEDCORE_EXPORT const Key Institution;
    extern MEDCORE_EXPORT const Key Referee;
    extern MEDCORE_EXPORT const Key StudyDate;
    extern MEDCORE_EXPORT const Key StudyTime;

    // SERIES
    extern MEDCORE_EXPORT const Key SeriesID;
    extern MEDCORE_EXPORT const Key SeriesDicomID;
    extern MEDCORE_EXPORT const Key SeriesStoreId;
    extern MEDCORE_EXPORT const Key SeriesNumber;
    extern MEDCORE_EXPORT const Key Modality;
    extern MEDCORE_EXPORT const Key Performer;
    extern MEDCORE_EXPORT const Key Report;
    extern MEDCORE_EXPORT const Key Protocol;
    extern MEDCORE_EXPORT const Key SeriesDescription;
    extern MEDCORE_EXPORT const Key SeriesDate;
    extern MEDCORE_EXPORT const Key SeriesTime;
    extern MEDCORE_EXPORT const Key SeriesThumbnail;

    // IMAGE
    extern MEDCORE_EXPORT const Key SOPInstanceUID;
    extern MEDCORE_EXPORT const Key Columns;
    extern MEDCORE_EXPORT const Key Rows;
    extern MEDCORE_EXPORT const Key Dimensions;
    extern MEDCORE_EXPORT const Key NumberOfDimensions;
    extern MEDCORE_EXPORT const Key Orientation;
    extern MEDCORE_EXPORT const Key Origin;
    extern MEDCORE_EXPORT const Key SliceThickness;
    extern MEDCORE_EXPORT const Key ImportationDate;
    extern MEDCORE_EXPORT const Key AcquisitionDate;
    extern MEDCORE_EXPORT const Key AcquisitionTime;
    extern MEDCORE_EXPORT const Key Comments;
    extern MEDCORE_EXPORT const Key FilePaths;
    extern MEDCORE_EXPORT const Key Status;
    extern MEDCORE_EXPORT const Key SequenceName;
    extern MEDCORE_EXPORT const Key Size;
    extern MEDCORE_EXPORT const Key VolumeUID;
    extern MEDCORE_EXPORT const Key Spacing;
    extern MEDCORE_EXPORT const Key XSpacing;
    extern MEDCORE_EXPORT const Key YSpacing;
    extern MEDCORE_EXPORT const Key ZSpacing;
    extern MEDCORE_EXPORT const Key NumberOfComponents;
    extern MEDCORE_EXPORT const Key ComponentType;
    extern MEDCORE_EXPORT const Key PixelType;
    extern MEDCORE_EXPORT const Key medDataType;
    extern MEDCORE_EXPORT const Key PreferredDataReader;
    extern MEDCORE_EXPORT const Key ImageID;
    extern MEDCORE_EXPORT const Key ThumbnailPath;
};



