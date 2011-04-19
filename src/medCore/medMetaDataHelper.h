#ifndef MEDMETADATAHELPER_H
#define MEDMETADATAHELPER_H

#include <QtCore/QObject>

#include <dtkCore/dtkAbstractData.h>

#include "medCore/medCoreExport.h"

/**
* @class medMetaDataHelper
* @author John Stark
* @brief static class to provide access to the metadata in dtkAbstractData.
* It allows compile-time verification that the keyword is correct.
*/
class MEDCORE_EXPORT medMetaDataHelper : public QObject
{
    Q_OBJECT

public:
#define MED_DEFINE_METADATA_ENTRY( key )                                \
    static QString KEY_##key () { return QString ( #key ) ; }           \
    static bool has##key (const dtkAbstractData *d) { return d->hasMetaData ( #key ) ; }     \
    static const QStringList get##key##Values (const dtkAbstractData *d)       \
        { return d->metaDataValues( #key ); }                           \
    static const QString getFirst##key##Value (const dtkAbstractData *d, const QString defaultValue = QString () ) \
        { return  d->hasMetaData ( #key ) ?                             \
            d->metaDataValues( #key )[0] : defaultValue; }              \
    static void add##key ( dtkAbstractData *d, const QStringList & values )    \
        { d->addMetaData( #key , values ); }                            \
    static void add##key ( dtkAbstractData *d, const QString & value )         \
        { d->addMetaData( #key , value ); }                             \
    static void set##key ( dtkAbstractData *d, const QStringList & values )    \
        { d->setMetaData( #key , values ); }                            \
    static void set##key ( dtkAbstractData *d, const QString & value )         \
        { d->setMetaData( #key , value ); }


    /** Define the actual keys to use */

    // PATIENT
    MED_DEFINE_METADATA_ENTRY( PatientID );
    MED_DEFINE_METADATA_ENTRY( PatientName );
    MED_DEFINE_METADATA_ENTRY( Age );
    MED_DEFINE_METADATA_ENTRY( BirthDate );
    MED_DEFINE_METADATA_ENTRY( Gender );
    MED_DEFINE_METADATA_ENTRY( Description ); //what?

    // STUDY
    MED_DEFINE_METADATA_ENTRY( StudyID );
    MED_DEFINE_METADATA_ENTRY( StudyDescription );
    MED_DEFINE_METADATA_ENTRY( Institution );
    MED_DEFINE_METADATA_ENTRY( Referee );
    MED_DEFINE_METADATA_ENTRY( StudyDate );
    MED_DEFINE_METADATA_ENTRY( StudyTime );

    // SERIES
    MED_DEFINE_METADATA_ENTRY( SeriesID );
    MED_DEFINE_METADATA_ENTRY( SeriesNumber );
    MED_DEFINE_METADATA_ENTRY( Modality );
    MED_DEFINE_METADATA_ENTRY( Performer );
    MED_DEFINE_METADATA_ENTRY( Report );
    MED_DEFINE_METADATA_ENTRY( Protocol );
    MED_DEFINE_METADATA_ENTRY( SeriesDescription );
    MED_DEFINE_METADATA_ENTRY( SeriesDate );
    MED_DEFINE_METADATA_ENTRY( SeriesTime );

    // IMAGE
    MED_DEFINE_METADATA_ENTRY( SOPInstanceUID );
    MED_DEFINE_METADATA_ENTRY( Columns );
    MED_DEFINE_METADATA_ENTRY( Rows );
    MED_DEFINE_METADATA_ENTRY( Dimensions );
    MED_DEFINE_METADATA_ENTRY( NumberOfDimensions );
    MED_DEFINE_METADATA_ENTRY( Orientation );
    MED_DEFINE_METADATA_ENTRY( Origin );
    MED_DEFINE_METADATA_ENTRY( SliceThickness );
    MED_DEFINE_METADATA_ENTRY( ImportationDate );
    MED_DEFINE_METADATA_ENTRY( AcquisitionDate );
    MED_DEFINE_METADATA_ENTRY( AcquisitionTime );
    MED_DEFINE_METADATA_ENTRY( Comments );
    MED_DEFINE_METADATA_ENTRY( FilePaths );
    MED_DEFINE_METADATA_ENTRY( Status );
    MED_DEFINE_METADATA_ENTRY( SequenceName );
    MED_DEFINE_METADATA_ENTRY( Size );
    MED_DEFINE_METADATA_ENTRY( VolumeUID );
    MED_DEFINE_METADATA_ENTRY( Spacing );
    MED_DEFINE_METADATA_ENTRY( XSpacing );
    MED_DEFINE_METADATA_ENTRY( YSpacing );
    MED_DEFINE_METADATA_ENTRY( ZSpacing );
    MED_DEFINE_METADATA_ENTRY( NumberOfComponents );
    MED_DEFINE_METADATA_ENTRY( ComponentType );
    MED_DEFINE_METADATA_ENTRY( PixelType );
    MED_DEFINE_METADATA_ENTRY( DtkDataType );
    MED_DEFINE_METADATA_ENTRY( PreferredDataReader );



protected:

private:
    /**  Constructor : Not defined as all methods are static */
    medMetaDataHelper(dtkAbstractData * dtkdata){};
    /**  Destructor */
    ~medMetaDataHelper(){};
};

#endif // MEDMETADATAHELPER_H
