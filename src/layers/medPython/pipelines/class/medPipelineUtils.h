#pragma once

#include <itkImage.h>

#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>

#include <dtkCoreSupport/dtkSmartPointer>

#include <medAbstractData.h>
#include <medPython.h>

#include "medPipelineAttachedData.h"
#include "medPipelineConstants.h"
#include "medPipelineExport.h"

class medAbstractProcessLegacy;

namespace med::pipeline
{

class MEDPIPELINE_EXPORT PipelineUtils
{
public:
    static dtkSmartPointer<medAbstractData> mask(dtkSmartPointer<medAbstractData> sourceImage,
                                                 dtkSmartPointer<medAbstractData> mask, double background = 0);

    static dtkSmartPointer<medAbstractData> createMeshFromMask(dtkSmartPointer<medAbstractData> mask, double isoValue,
                                                               double decimation = 0, int smoothIterations = 0,
                                                               double relaxationFactor = 0);

    static dtkSmartPointer<medAbstractData> gaussianBlur(dtkSmartPointer<medAbstractData> input, double sigma);
    static dtkSmartPointer<medAbstractData> normalize(dtkSmartPointer<medAbstractData> input);
    static dtkSmartPointer<medAbstractData> median(dtkSmartPointer<medAbstractData> input, double neighborhoodSize = 1);
    static dtkSmartPointer<medAbstractData> invert(dtkSmartPointer<medAbstractData> input);
    static dtkSmartPointer<medAbstractData> shrink(dtkSmartPointer<medAbstractData> input,
                                                   unsigned int xFactor, unsigned int yFactor, unsigned int zFactor);
    static dtkSmartPointer<medAbstractData> intensityWindowing(dtkSmartPointer<medAbstractData> input,
                                                               double minimum, double maximum,
                                                               double outputMinimum, double outputMaximum);
    static dtkSmartPointer<medAbstractData> threshold(dtkSmartPointer<medAbstractData> input,
                                                      double threshold, int outsideValue, med::pipeline::ThresholdType thresholdType);
    static dtkSmartPointer<medAbstractData> threshold(dtkSmartPointer<medAbstractData> input,
                                                     double lowerValue, double upperValue, int outsideValue);
    static dtkSmartPointer<medAbstractData> binaryThreshold(dtkSmartPointer<medAbstractData> input,
                                                            double lowerValue, double upperValue,
                                                            int insideValue, int outsideValue);
    static dtkSmartPointer<medAbstractData> sizeThreshold(dtkSmartPointer<medAbstractData> input, int minimumSize);

    static dtkSmartPointer<medAbstractData> addConstant(dtkSmartPointer<medAbstractData> data, double constant);
    static dtkSmartPointer<medAbstractData> subtractConstant(dtkSmartPointer<medAbstractData> data, double constant);
    static dtkSmartPointer<medAbstractData> multiplyByConstant(dtkSmartPointer<medAbstractData> data, double constant);
    static dtkSmartPointer<medAbstractData> divideByConstant(dtkSmartPointer<medAbstractData> data, double constant);

    static dtkSmartPointer<medAbstractData> notImage(dtkSmartPointer<medAbstractData> data);
    static dtkSmartPointer<medAbstractData> andImage(dtkSmartPointer<medAbstractData> data1,
                                                     dtkSmartPointer<medAbstractData> data2);
    static dtkSmartPointer<medAbstractData> orImage(dtkSmartPointer<medAbstractData> data1,
                                                    dtkSmartPointer<medAbstractData> data2);
    static dtkSmartPointer<medAbstractData> xorImage(dtkSmartPointer<medAbstractData> data1,
                                                     dtkSmartPointer<medAbstractData> data2);

    static dtkSmartPointer<medAbstractData> morphologicalFilter(dtkSmartPointer<medAbstractData> input,
                                                                med::pipeline::MorphologicalFilterType filterType,
                                                                double radius, med::pipeline::RadiusType radiusType);
    static dtkSmartPointer<medAbstractData> dilate(dtkSmartPointer<medAbstractData> input, double radius,
                                                   med::pipeline::RadiusType radiusType);
    static dtkSmartPointer<medAbstractData> erode(dtkSmartPointer<medAbstractData> input, double radius,
                                                  med::pipeline::RadiusType radiusType);
    static dtkSmartPointer<medAbstractData> binaryClose(dtkSmartPointer<medAbstractData> input, double radius,
                                                        med::pipeline::RadiusType radiusType);
    static dtkSmartPointer<medAbstractData> binaryOpen(dtkSmartPointer<medAbstractData> input, double radius,
                                                        med::pipeline::RadiusType radiusType);
    static dtkSmartPointer<medAbstractData> grayscaleClose(dtkSmartPointer<medAbstractData> input, double radius,
                                                           med::pipeline::RadiusType radiusType);
    static dtkSmartPointer<medAbstractData> grayscaleOpen(dtkSmartPointer<medAbstractData> input, double radius,
                                                          med::pipeline::RadiusType radiusType);

    static dtkSmartPointer<medAbstractData> resampleByDimension(dtkSmartPointer<medAbstractData> input, int x, int y, int z);
    static dtkSmartPointer<medAbstractData> resampleBySpacing(dtkSmartPointer<medAbstractData> input, double x, double y, double z);

    static dtkSmartPointer<medAbstractData> binaryFillhole(dtkSmartPointer<medAbstractData> input);

    static void copyWindowLevel(medAbstractData* targetData, medAbstractData* sourceData);
    static dtkSmartPointer<PipelineAttachedData> getAttachedData(medAbstractData* data, bool createIfNotFound = false);

//    static PyObject* getImage(medAbstractData *medData);

//    static dtkSmartPointer<medAbstractData> createMedAbstractDataFromITKPythonImage(PyObject *image_buffer,
//                                                                                    PyObject *shape,
//                                                                                    QList<double> size,
//                                                                                    QList<double> spacing,
//                                                                                    QList<double> origin,
//                                                                                    QList<double> direction,
//                                                                                    QString type,
//                                                                                    PyObject *numOfComponents);

//    static dtkSmartPointer<medAbstractData> castImage(medAbstractData* data, QString outputType);
//    static dtkSmartPointer<medAbstractData> createMedAbstractDataFromVTKMesh(vtkPolyData* vtkObject, QString identifier);
//    static dtkSmartPointer<medAbstractData> createMedAbstractDataFromVTKMesh(vtkUnstructuredGrid* vtkObject, QString identifier);
//    static bool isVolumeMesh(medAbstractData* data);
//    static vtkPolyData* getVTKPolyData(medAbstractData* data);
//    static vtkUnstructuredGrid* getVTKUnstructuredGrid(medAbstractData* data);

    static QString getIdentifier(medAbstractData* data);

private:
    static dtkSmartPointer<medAbstractData> applyArithmeticFilter(dtkSmartPointer<medAbstractData> data,
                                                                  med::pipeline::BasicFilterType filterType,
                                                                  double constant);

    static dtkSmartPointer<medAbstractData> applyBinaryFilter(QString identifier, dtkSmartPointer<medAbstractData> data1,
                                                              dtkSmartPointer<medAbstractData> data2);

    static medAbstractData* outputWithCopiedWindowLevel(medAbstractProcessLegacy* process, medAbstractData* source);

    PipelineUtils() {}
    ~PipelineUtils() {}

    template<typename PixelType>
    static PyObject* getImage(medAbstractData *medData);

    template<typename CType>
    static const char* getTypename();
    template<typename PixelType>
    static typename itk::Image<PixelType, 3>::Pointer createITKImageFromPyObject(PyObject *image_buffer,
                                                                                 PyObject *shape,
                                                                                 QList<double> size,
                                                                                 QList<double> spacing,
                                                                                 QList<double> origin,
                                                                                 QList<double> direction,
                                                                                 PyObject *numOfComponents);

    template<class InputPixelType>
    static dtkSmartPointer<medAbstractData> castITKImage(medAbstractData *inputData, QString outputPixelType);
    template <class InputPixelType, class OutputPixelType>
    static dtkSmartPointer<medAbstractData> castITKImage(medAbstractData* data, QString type);


    static dtkSmartPointer<medAbstractData> threshold(dtkSmartPointer<medAbstractData> input,
                                                      double lowerValue, double upperValue, double threshold,
                                                      int outsideValue, med::pipeline::ThresholdType thresholdType);
};

} // namespace med::pipeline
