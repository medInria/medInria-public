#undef MEDPIPELINE_EXPORT
#define MEDPIPELINE_EXPORT

/* This macro is used to forward static methods as global Python functions.
 * For example, you can write "setDerivedMetaData(...)" instead of "medUtilities.setDerivedMetaData(...)".
 */
%define %forwardFunction(name, object)
%pythoncode
{
    def name(*args):
        return object.name(*args)
}
%enddef

/* This macro is used to forward attibute methods as methods of the class holding that attribute.
 * For example, you can write "self.startPolygonRoiStep(...)" instead of
 * "self.getSegmentationDelegate().startPolygonRoiStep(...)".
 */
%define %forwardMethod(name, attribute)
%pythoncode
{
    def name(self, *args):
        return self.attribute.name(*args)
}
%enddef

// Necessary because the types are imported with the medInria bindings.
%{
#include "medDataManager.h"
#include "medDropSite.h"
#include "medAbstractImageData.h"
#include "medAbstractMeshData.h"
%}
