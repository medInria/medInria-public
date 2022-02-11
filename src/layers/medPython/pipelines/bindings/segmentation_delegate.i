%rename(startPolygonRoiStep) SegmentationDelegate::startPolygonROIStep;
%rename(endPolygonRoiStep) SegmentationDelegate::endPolygonROIStep;
%rename(startPolygonRoiSamplingStep) SegmentationDelegate::startPolygonROISamplingStep;
%rename(endPolygonRoiSamplingStep) SegmentationDelegate::endPolygonROISamplingStep;
%rename(startVoiCutterStep) SegmentationDelegate::startVOICutterStep;
%rename(endVoiCutterStep) SegmentationDelegate::endVOICutterStep;

%{
#include "medPipelineSegmentationDelegate.h"
%}

/* These rules allow SWIG to replace C++ output arguments by multiple return values in Python.
 * (Python does not support output arguments)
 * To avoid side effects the rules are cleared after use.
 */
%apply dtkSmartPointer<medAbstractData>* OUTPUT { dtkSmartPointer<medAbstractData>* output };
%apply QList<dtkSmartPointer<medAbstractData> >* OUTPUT { QList<dtkSmartPointer<medAbstractData> >* outputs };
%apply QList<double>* OUTPUT { QList<double>* outputValues };
%apply QList<double>* OUTPUT { QList<double>* outputStats };
%apply QList<double>* OUTPUT { QList<double>* thresholds };
%apply int* OUTPUT { Pipeline::ThresholdType* chosenThresholdType };

%include "medPipelineSegmentationDelegate.h"

%clear dtkSmartPointer<medAbstractData>* output;
%clear QList<dtkSmartPointer<medAbstractData> >* outputs;
%clear QList<double>* outputValues;
%clear QList<double>* outputStats;
%clear QList<double>* thresholds;
%clear med::pipeline::ThresholdType* chosenThresholdType;

%forwardStepCreationFunction(paintStep, SegmentationDelegate, createDelegatePaintStep)
%forwardStepCreationFunction(customPaintStep, SegmentationDelegate, createPaintStep)

%forwardStepCreationFunction(polygonRoiStep, SegmentationDelegate, createDelegatePolygonROIStep)
%forwardStepCreationFunction(customPolygonRoiStep, SegmentationDelegate, createPolygonROIStep)

%forwardStepCreationFunction(polygonRoiSamplingStep, SegmentationDelegate, createDelegatePolygonROISamplingStep)
%forwardStepCreationFunction(customPolygonRoiSamplingStep, SegmentationDelegate, createPolygonROISamplingStep)

%forwardStepCreationFunction(draftMagicWandStep, SegmentationDelegate, createDelegateDraftMagicWandStep)
%forwardStepCreationFunction(customDraftMagicWandStep, SegmentationDelegate, createDraftMagicWandStep)

%forwardStepCreationFunction(magicWandStep, SegmentationDelegate, createDelegateMagicWandStep)
%forwardStepCreationFunction(customMagicWandStep, SegmentationDelegate, createMagicWandStep)

%forwardStepCreationFunction(maskPaintCorrectionStep, SegmentationDelegate, createDelegateMaskPaintCorrectionStep)
%forwardStepCreationFunction(customMaskPaintCorrectionStep, SegmentationDelegate, createMaskPaintCorrectionStep)

%forwardStepCreationFunction(vesselStep, SegmentationDelegate, createDelegateVesselStep)
%forwardStepCreationFunction(customVesselStep, SegmentationDelegate, createVesselStep)

%forwardStepCreationFunction(histogramAnalysisStep, SegmentationDelegate, createDelegateHistogramAnalysisStep)
%forwardStepCreationFunction(customHistogramAnalysisStep, SegmentationDelegate, createHistogramAnalysisStep)

%forwardStepCreationFunction(voiCutterStep, SegmentationDelegate, createDelegateVOICutterStep)
%forwardStepCreationFunction(customVoiCutterStep, SegmentationDelegate, createVOICutterStep)

%forwardStepCreationFunction(geodesicActiveContourLevelSetStep, SegmentationDelegate, createDelegateGeodesicActiveContourLevelSetStep)
%forwardStepCreationFunction(customGeodesicActiveContourLevelSetStep, SegmentationDelegate, createGeodesicActiveContourLevelSetStep)

%forwardStepCreationFunction(maskPaintOperationsStep, SegmentationDelegate, createDelegateMaskPaintOperationsStep)
%forwardStepCreationFunction(customMaskPaintOperationsStep, SegmentationDelegate, createMaskPaintOperationsStep)

%extend med::pipeline::PythonPipeline
{
    %forwardMethod(startPaintStep, getSegmentationDelegate())
    %forwardMethod(endPaintStep, getSegmentationDelegate())

    %forwardMethod(startPolygonRoiStep, getSegmentationDelegate())
    %forwardMethod(endPolygonRoiStep, getSegmentationDelegate())

    %forwardMethod(startPolygonRoiSamplingStep, getSegmentationDelegate())
    %forwardMethod(endPolygonRoiSamplingStep, getSegmentationDelegate())

    %forwardMethod(startDraftMagicWandStep, getSegmentationDelegate())
    %forwardMethod(endDraftMagicWandStep, getSegmentationDelegate())

    %forwardMethod(startMagicWandStep, getSegmentationDelegate())
    %forwardMethod(endMagicWandStep, getSegmentationDelegate())
    
    %forwardMethod(startMaskPaintCorrectionStep, getSegmentationDelegate())
    %forwardMethod(endMaskPaintCorrectionStep, getSegmentationDelegate())

    %forwardMethod(startVesselStep, getSegmentationDelegate())
    %forwardMethod(endVesselStep, getSegmentationDelegate())

    %forwardMethod(startHistogramAnalysisStep, getSegmentationDelegate())

    %pythoncode
    {
        def endHistogramAnalysisStep(self):
            output = self.getSegmentationDelegate().endHistogramAnalysisStep()
            chosenThresholdType = output.pop()
            return (output, chosenThresholdType)
    }

    %forwardMethod(startVoiCutterStep, getSegmentationDelegate())
    %forwardMethod(endVoiCutterStep, getSegmentationDelegate())

    %forwardMethod(startGeodesicActiveContourLevelSetStep, getSegmentationDelegate())
    %forwardMethod(endGeodesicActiveContourLevelSetStep, getSegmentationDelegate())

    %forwardMethod(startMaskPaintOperationsStep, getSegmentationDelegate())
    %forwardMethod(endMaskPaintOperationsStep, getSegmentationDelegate())
}
