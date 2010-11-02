/**
* This software was developed using DCMTK from OFFIS e.V.
*
* Kuratorium OFFIS e.V.
* Healthcare Information and Communication Systems
* Escherweg 2
* D-26121 Oldenburg, Germany
*/


#ifndef DCMTKMODIFY_H
#define DCMTKMODIFY_H

// inc
#include "dcmtkBase.h"

/**
* @class dcmtkModify
* @author Michael Knopke
* @brief Modify attributes of DICOM files. Based on dcmModify
*/
class dcmtkModify : public dcmtkBase
{
public:

    dcmtkModify();

    ~dcmtkModify();


    //if (job.option=="i")
    //    result = ds_man->modifyOrInsertPath(job.path, job.value, OFFalse, update_metaheader_uids_option, ignore_missing_tags_option, no_reservation_checks);
    //else if (job.option == "if")
    //    result = ds_man->modifyOrInsertFromFile(job.path, job.value /*filename*/, OFFalse, update_metaheader_uids_option, ignore_missing_tags_option, no_reservation_checks);
    //else if (job.option == "m")
    //    result = ds_man->modifyOrInsertPath(job.path, job.value, OFTrue, update_metaheader_uids_option, ignore_missing_tags_option, no_reservation_checks);
    //else if (job.option == "mf")
    //    result = ds_man->modifyOrInsertFromFile(job.path, job.value /*filename*/, OFTrue, update_metaheader_uids_option, ignore_missing_tags_option, no_reservation_checks);
    //else if (job.option == "ma")
    //    result = ds_man->modifyAllTags(job.path, job.value, update_metaheader_uids_option, count);
    //else if (job.option == "e")
    //    result = ds_man->deleteTag(job.path, OFFalse, ignore_missing_tags_option);
    //else if (job.option == "ea")
    //    result = ds_man->deleteTag(job.path, OFTrue, ignore_missing_tags_option);
    //else if (job.option == "ep")
    //    result = ds_man->deletePrivateData();
    //else if (job.option == "gst")
    //    result = ds_man->generateAndInsertUID(DCM_StudyInstanceUID);
    //else if (job.option == "gse")
    //    result = ds_man->generateAndInsertUID(DCM_SeriesInstanceUID);
    //else if (job.option == "gin")
    //    result = ds_man->generateAndInsertUID(DCM_SOPInstanceUID);


protected:

private:

};

#endif // DCMTKMODIFY_H
