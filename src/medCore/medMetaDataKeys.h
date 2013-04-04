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

    class MEDCORE_EXPORT Key : public QObject{
      Q_OBJECT
    public:

        typedef std::vector<const Key*> Registery;

        Key(const char* name): KEY(name) { registery.push_back(this); }
        ~Key() { }

        const QString& key() const { return KEY; }

        bool is_set_in(const dtkAbstractData *d) const { return d->hasMetaData(KEY) ; }

        const QStringList getValues(const dtkAbstractData *d) const { return d->metaDataValues(KEY); }

        const QString getFirstValue(const dtkAbstractData *d,const QString defaultValue=QString()) const {
            return  d->hasMetaData(KEY) ? d->metaDataValues(KEY)[0] : defaultValue;
        }

        void add(dtkAbstractData* d,const QStringList& values) const { d->addMetaData(KEY,values); }
        void add(dtkAbstractData* d,const QString& value)      const { d->addMetaData(KEY,value);  }
        void set(dtkAbstractData* d,const QStringList& values) const { d->setMetaData(KEY,values); }
        void set(dtkAbstractData* d,const QString& value)      const { d->setMetaData(KEY,value);  }

        static const Registery& all() { return registery; }

    private:

        static Registery registery;

        const QString KEY;
    };


    /** Define the actual keys to use */

    extern MEDCORE_EXPORT const Key TransferSyntaxUID;

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
    extern MEDCORE_EXPORT const Key DtkDataType;
    extern MEDCORE_EXPORT const Key PreferredDataReader;
    extern MEDCORE_EXPORT const Key ImageID;
    extern MEDCORE_EXPORT const Key ThumbnailPath;
};



