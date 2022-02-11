#include "medPipelineUtils.h"

#include <itkCastImageFilter.h>
#include <itkImageRegionIterator.h>

#include <vtkMetaSurfaceMesh.h>
#include <vtkMetaVolumeMesh.h>

#include <dtkCoreSupport/dtkAbstractProcessFactory>

#include <medAbstractDataFactory.h>
#include <medAbstractProcessLegacy.h>
#include <statsROI.h>

//#include "python-tmp/itkPyBuffer.h"

namespace med::pipeline
{

dtkSmartPointer<medAbstractData> PipelineUtils::mask(dtkSmartPointer<medAbstractData> sourceImage,
                                                     dtkSmartPointer<medAbstractData> mask, double background)
{
    dtkSmartPointer<medAbstractProcessLegacy> maskProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("medMaskApplication");
    maskProcess->setInput(mask, 0);
    maskProcess->setInput(sourceImage, 1);
    maskProcess->setParameter(background, 0);
    maskProcess->update();
    return outputWithCopiedWindowLevel(maskProcess, sourceImage);
}

dtkSmartPointer<medAbstractData> PipelineUtils::createMeshFromMask(dtkSmartPointer<medAbstractData> mask, double isoValue, double decimation, int smoothIterations, double relaxationFactor)
{
    dtkSmartPointer<medAbstractProcessLegacy> meshFromMaskProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("medCreateMeshFromMask");
    meshFromMaskProcess->setParameter(isoValue, 0);

    if (decimation > 0)
    {
        meshFromMaskProcess->setParameter(1.0 , 1);
        meshFromMaskProcess->setParameter(decimation, 2);
    }
    else
    {
        meshFromMaskProcess->setParameter(0.0 , 1);
    }

    if (smoothIterations > 0)
    {
        meshFromMaskProcess->setParameter(1.0, 3);
        meshFromMaskProcess->setParameter((double)smoothIterations, 4);
        meshFromMaskProcess->setParameter(relaxationFactor, 5);
    }
    else
    {
        meshFromMaskProcess->setParameter(0.0, 3);
    }

    meshFromMaskProcess->setInput(mask);
    meshFromMaskProcess->update();

    return meshFromMaskProcess->output();
}

dtkSmartPointer<medAbstractData> PipelineUtils::gaussianBlur(dtkSmartPointer<medAbstractData> input, double sigma)
{
    dtkSmartPointer<medAbstractProcessLegacy> gaussianProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("itkGaussianProcess");
    gaussianProcess->setInput(input);
    gaussianProcess->setParameter(sigma, 0);
    gaussianProcess->update();
    return outputWithCopiedWindowLevel(gaussianProcess, input);
}

dtkSmartPointer<medAbstractData> PipelineUtils::normalize(dtkSmartPointer<medAbstractData> input)
{
    dtkSmartPointer<medAbstractProcessLegacy> normalizeProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("itkNormalizeProcess");
    normalizeProcess->setInput(input);
    normalizeProcess->update();
    return outputWithCopiedWindowLevel(normalizeProcess, input);
}

dtkSmartPointer<medAbstractData> PipelineUtils::median(dtkSmartPointer<medAbstractData> input, double neighborhoodSize)
{
    dtkSmartPointer<medAbstractProcessLegacy> medianProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("itkMedianProcess");
    medianProcess->setInput(input);
    medianProcess->setParameter(neighborhoodSize);
    medianProcess->update();
    return outputWithCopiedWindowLevel(medianProcess, input);
}

dtkSmartPointer<medAbstractData> PipelineUtils::invert(dtkSmartPointer<medAbstractData> input)
{
    dtkSmartPointer<medAbstractProcessLegacy> invertProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("itkInvertProcess");
    invertProcess->setInput(input);
    invertProcess->update();
    return outputWithCopiedWindowLevel(invertProcess, input);
}

dtkSmartPointer<medAbstractData> PipelineUtils::shrink(dtkSmartPointer<medAbstractData> input,
                                                          unsigned int xFactor, unsigned int yFactor, unsigned int zFactor)
{
    dtkSmartPointer<medAbstractProcessLegacy> shrinkProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("itkShrinkProcess");
    shrinkProcess->setInput(input);
    shrinkProcess->setParameter(static_cast<double>(xFactor), 0);
    shrinkProcess->setParameter(static_cast<double>(yFactor), 1);
    shrinkProcess->setParameter(static_cast<double>(zFactor), 2);
    shrinkProcess->update();
    return outputWithCopiedWindowLevel(shrinkProcess, input);
}

dtkSmartPointer<medAbstractData> PipelineUtils::intensityWindowing(dtkSmartPointer<medAbstractData> input,
                                                                      double minimum, double maximum,
                                                                      double outputMinimum, double outputMaximum)
{
    dtkSmartPointer<medAbstractProcessLegacy> windowingProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("itkWindowingProcess");
    windowingProcess->setInput(input);
    windowingProcess->setParameter(minimum, 0);
    windowingProcess->setParameter(maximum, 1);
    windowingProcess->setParameter(outputMinimum, 2);
    windowingProcess->setParameter(outputMaximum, 3);
    windowingProcess->update();
    return outputWithCopiedWindowLevel(windowingProcess, input);
}

dtkSmartPointer<medAbstractData> PipelineUtils::threshold(dtkSmartPointer<medAbstractData> input,
                                                            double lowerValue, double upperValue, int outsideValue)
{
    return threshold(input, lowerValue, upperValue, 0.0, outsideValue, OUTSIDE_THRESHOLD);
}

dtkSmartPointer<medAbstractData> PipelineUtils::threshold(dtkSmartPointer<medAbstractData> input,
                                                             double thresholdValue, int outsideValue, ThresholdType thresholdType)
{
    return threshold(input, 0., 0., thresholdValue, outsideValue, thresholdType);
}

dtkSmartPointer<medAbstractData> PipelineUtils::binaryThreshold(dtkSmartPointer<medAbstractData> input,
                                                                   double lowerValue, double upperValue,
                                                                   int insideValue, int outsideValue)
{
    dtkSmartPointer<medAbstractProcessLegacy> process = dtkAbstractProcessFactory::instance()->createSmartPointer( "itkBinaryThresholdingProcess" );

    process->setInput ( input );
    process->setParameter ( insideValue, 0);
    process->setParameter ( outsideValue, 1);
    process->setParameter ( lowerValue, 0);
    process->setParameter ( upperValue, 1);

    process->update();
    return outputWithCopiedWindowLevel(process, input);
}

dtkSmartPointer<medAbstractData> PipelineUtils::sizeThreshold(dtkSmartPointer<medAbstractData> input, int minimumSize)
{
    dtkSmartPointer<medAbstractProcessLegacy> sizeThresholdProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("itkComponentSizeThresholdProcess");
    sizeThresholdProcess->setInput(input);
    sizeThresholdProcess->setParameter(minimumSize, 0);
    sizeThresholdProcess->update();
    return outputWithCopiedWindowLevel(sizeThresholdProcess, input);
}

dtkSmartPointer<medAbstractData> PipelineUtils::addConstant(dtkSmartPointer<medAbstractData> data, double constant)
{
    return applyArithmeticFilter(data, med::pipeline::ADD, constant);
}

dtkSmartPointer<medAbstractData> PipelineUtils::subtractConstant(dtkSmartPointer<medAbstractData> data, double constant)
{
    return applyArithmeticFilter(data, med::pipeline::SUBTRACT, constant);
}

dtkSmartPointer<medAbstractData> PipelineUtils::multiplyByConstant(dtkSmartPointer<medAbstractData> data, double constant)
{
    return applyArithmeticFilter(data, med::pipeline::MULTIPLY, constant);
}

dtkSmartPointer<medAbstractData> PipelineUtils::divideByConstant(dtkSmartPointer<medAbstractData> data, double constant)
{
    return applyArithmeticFilter(data, med::pipeline::DIVIDE, constant);
}

dtkSmartPointer<medAbstractData> PipelineUtils::notImage(dtkSmartPointer<medAbstractData> data)
{
    dtkSmartPointer<medAbstractProcessLegacy> notProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("itkNotOperator");
    notProcess->setInput(data);
    notProcess->update();
    return notProcess->output();
}

dtkSmartPointer<medAbstractData> PipelineUtils::andImage(dtkSmartPointer<medAbstractData> data1,
                                                            dtkSmartPointer<medAbstractData> data2)
{
    return applyBinaryFilter("itkAndOperator", data1, data2);
}

dtkSmartPointer<medAbstractData> PipelineUtils::orImage(dtkSmartPointer<medAbstractData> data1,
                                                           dtkSmartPointer<medAbstractData> data2)
{
    return applyBinaryFilter("itkOrOperator", data1, data2);
}

dtkSmartPointer<medAbstractData> PipelineUtils::xorImage(dtkSmartPointer<medAbstractData> data1,
                                                            dtkSmartPointer<medAbstractData> data2)
{
    return applyBinaryFilter("itkXorOperator", data1, data2);
}

dtkSmartPointer<medAbstractData> PipelineUtils::morphologicalFilter(dtkSmartPointer<medAbstractData> input,
                                                                       MorphologicalFilterType filterType,
                                                                       double radius, RadiusType radiusType)
{
    QString filterIdentifier;

    switch (filterType)
    {
    case DILATE:
        filterIdentifier = "itkDilateProcess";
        break;
    case ERODE:
        filterIdentifier = "itkErodeProcess";
        break;
    case BINARY_CLOSE:
        filterIdentifier = "itkBinaryCloseProcess";
        break;
    case BINARY_OPEN:
        filterIdentifier = "itkBinaryOpenProcess";
        break;
    case GRAYSCALE_CLOSE:
        filterIdentifier = "itkGrayscaleCloseProcess";
        break;
    case GRAYSCALE_OPEN:
        filterIdentifier = "itkGrayscaleOpenProcess";
        break;
    default:
        throw std::invalid_argument("medPipelineUtils::morphologicalFilter wrong type given.");
    }

    dtkSmartPointer<medAbstractProcessLegacy> filter = dtkAbstractProcessFactory::instance()->createSmartPointer(filterIdentifier);
    filter->setInput(input);
    filter->setParameter(radius, (radiusType == RADIUS_IN_MM) ? 0 : 1);
    filter->update();

    return outputWithCopiedWindowLevel(filter, input);
}

dtkSmartPointer<medAbstractData> PipelineUtils::dilate(dtkSmartPointer<medAbstractData> input, double radius, RadiusType radiusType)
{
    return morphologicalFilter(input, med::pipeline::DILATE, radius, radiusType);
}

dtkSmartPointer<medAbstractData> PipelineUtils::erode(dtkSmartPointer<medAbstractData> input, double radius, RadiusType radiusType)
{
    return morphologicalFilter(input, med::pipeline::ERODE, radius, radiusType);
}

dtkSmartPointer<medAbstractData> PipelineUtils::binaryClose(dtkSmartPointer<medAbstractData> input, double radius, RadiusType radiusType)
{
    return morphologicalFilter(input, med::pipeline::BINARY_CLOSE, radius, radiusType);
}

dtkSmartPointer<medAbstractData> PipelineUtils::binaryOpen(dtkSmartPointer<medAbstractData> input, double radius, RadiusType radiusType)
{
    return morphologicalFilter(input, med::pipeline::BINARY_OPEN, radius, radiusType);
}

dtkSmartPointer<medAbstractData> PipelineUtils::grayscaleClose(dtkSmartPointer<medAbstractData> input, double radius, RadiusType radiusType)
{
    return morphologicalFilter(input, med::pipeline::GRAYSCALE_CLOSE, radius, radiusType);
}

dtkSmartPointer<medAbstractData> PipelineUtils::grayscaleOpen(dtkSmartPointer<medAbstractData> input, double radius, RadiusType radiusType)
{
    return morphologicalFilter(input, med::pipeline::GRAYSCALE_OPEN, radius, radiusType);
}

dtkSmartPointer<medAbstractData> PipelineUtils::resampleByDimension(dtkSmartPointer<medAbstractData> input, int x, int y, int z)
{
    dtkSmartPointer<medAbstractProcessLegacy> resampleProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("resampleProcess");
    resampleProcess->setInput(input);
    resampleProcess->setParameter(double(x), 0);
    resampleProcess->setParameter(double(y), 1);
    resampleProcess->setParameter(double(z), 2);
    resampleProcess->update();
    return outputWithCopiedWindowLevel(resampleProcess, input);
}

dtkSmartPointer<medAbstractData> PipelineUtils::resampleBySpacing(dtkSmartPointer<medAbstractData> input, double x, double y, double z)
{
    dtkSmartPointer<medAbstractProcessLegacy> resampleProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("resampleProcess");
    resampleProcess->setInput(input);
    resampleProcess->setParameter(x, 3);
    resampleProcess->setParameter(y, 4);
    resampleProcess->setParameter(z, 5);
    resampleProcess->update();
    return outputWithCopiedWindowLevel(resampleProcess, input);
}

dtkSmartPointer<medAbstractData> PipelineUtils::binaryFillhole(dtkSmartPointer<medAbstractData> input)
{
    dtkSmartPointer<medAbstractProcessLegacy> process = dtkAbstractProcessFactory::instance()->createSmartPointer("itkBinaryFillholeProcess");
    process->setInput(input);
    QString identifier = input->identifier();
    if ( identifier == "itkDataImageChar3" || identifier == "itkDataImageUChar3" ||
         identifier == "itkDataImageShort3" || identifier == "itkDataImageUShort3" ||
         identifier == "itkDataImageInt3" || identifier == "itkDataImageUInt3" ||
         identifier == "itkDataImageLong3" || identifier== "itkDataImageULong3" )
    {
        process->setParameter(std::numeric_limits<int>::max());
    }
    else if ( identifier == "itkDataImageFloat3" || identifier == "itkDataImageDouble3" )
    {
        process->setParameter(std::numeric_limits<double>::max());
    }
    process->update();
    return outputWithCopiedWindowLevel(process, input);

}

void PipelineUtils::copyWindowLevel(medAbstractData* targetData, medAbstractData* sourceData)
{
    dtkSmartPointer<PipelineAttachedData> sourceAttachedData = getAttachedData(sourceData);
    if (sourceAttachedData)
    {
        dtkSmartPointer<PipelineAttachedData> targetAttachedData = getAttachedData(targetData, true);
        targetAttachedData->setWindowLevel(sourceAttachedData->getWindow(), sourceAttachedData->getLevel());
    }
}

dtkSmartPointer<PipelineAttachedData> PipelineUtils::getAttachedData(medAbstractData* data, bool createIfNotFound)
{
    QList<medAttachedData*> attachedData = data->attachedData();
    dtkSmartPointer<PipelineAttachedData> pipelineAttachedData ;

    for (int i = 0; !pipelineAttachedData && (i < attachedData.size()); i++)
    {
        pipelineAttachedData = dynamic_cast<PipelineAttachedData*>(attachedData[i]);
    }

    if (!pipelineAttachedData && createIfNotFound)
    {
        pipelineAttachedData = new PipelineAttachedData;
        data->addAttachedData(pipelineAttachedData);
    }

    return pipelineAttachedData;
}

dtkSmartPointer<medAbstractData> PipelineUtils::applyArithmeticFilter(dtkSmartPointer<medAbstractData> data,
                                                                         BasicFilterType filterType,
                                                                         double constant)
{
    QString filterIdentifier;

    switch (filterType)
    {
    case ADD:
        filterIdentifier = "itkAddProcess";
        break;
    case SUBTRACT:
        filterIdentifier = "itkSubtractProcess";
        break;
    case MULTIPLY:
        filterIdentifier = "itkMultiplyProcess";
        break;
    case DIVIDE:
        filterIdentifier = "itkDivideProcess";
        break;
    default:
        throw std::invalid_argument("Filter type must be one of ADD, SUBTRACT, MULTIPLY or DIVIDE");
    }

    dtkSmartPointer<medAbstractProcessLegacy> filter = dtkAbstractProcessFactory::instance()->createSmartPointer(filterIdentifier);
    filter->setInput(data);
    filter->setParameter(constant, 0);
    filter->update();

    return outputWithCopiedWindowLevel(filter, data);
}

dtkSmartPointer<medAbstractData> PipelineUtils::applyBinaryFilter(QString identifier,
                                                                     dtkSmartPointer<medAbstractData> data1,
                                                                     dtkSmartPointer<medAbstractData> data2)
{
    dtkSmartPointer<medAbstractProcessLegacy> filter = dtkAbstractProcessFactory::instance()->createSmartPointer(identifier);
    filter->setInput(data1, 0);
    filter->setInput(data2, 1);
    filter->update();
    return filter->output();
}

medAbstractData* PipelineUtils::outputWithCopiedWindowLevel(medAbstractProcessLegacy* process, medAbstractData* source)
{
    medAbstractData* output = process->output();
    copyWindowLevel(output, source);
    return output;
}

//PyObject *PipelineUtils::getImage(medAbstractData *medData)
//{
//    QString type = medData->identifier();
//    if ( type == "itkDataImageChar3" )
//    {
//        return PipelineUtils::getImage<char>(medData);
//    }
//    else if ( type == "itkDataImageUChar3" )
//    {
//        return PipelineUtils::getImage<unsigned char>(medData);
//    }
//    else if ( type == "itkDataImageShort3" )
//    {
//        return PipelineUtils::getImage<short>(medData);
//    }
//    else if ( type == "itkDataImageUShort3" )
//    {
//        return PipelineUtils::getImage<unsigned short>(medData);
//    }
//    else if ( type == "itkDataImageInt3" )
//    {
//        return PipelineUtils::getImage<int>(medData);
//    }
//    else if ( type == "itkDataImageUInt3" )
//    {
//        return PipelineUtils::getImage<unsigned int>(medData);
//    }
//    else if ( type == "itkDataImageLong3" )
//    {
//        return PipelineUtils::getImage<long>(medData);
//    }
//    else if ( type == "itkDataImageULong3" )
//    {
//        return PipelineUtils::getImage<unsigned long>(medData);
//    }
//    else if ( type == "itkDataImageFloat3" )
//    {
//        return PipelineUtils::getImage<float>(medData);
//    }
//    else if ( type == "itkDataImageDouble3" )
//    {
//        return PipelineUtils::getImage<double>(medData);
//    }
//    else
//    {
//        qDebug()<<"medPipelineUtils: wrong type given. Unsupported " <<type<< ".";
//        return nullptr;
//    }

//}

//template <typename PixelType>
//PyObject* PipelineUtils::getImage(medAbstractData* medData)
//{
//    typename itk::Image<PixelType, 3>* itkImage = dynamic_cast<itk::Image<PixelType, 3>*> ((itk::Object*)(medData->data()));

//    typename itk::Image<PixelType, 3>::RegionType region = itkImage->GetLargestPossibleRegion();
//    typename itk::Image<PixelType, 3>::SizeType itkSize = region.GetSize();
//    typename itk::Image<PixelType, 3>::DirectionType itkDirection = itkImage->GetDirection();
//    PyErr_Clear();

//    PyObject *pyImg = itk::PyBuffer<itk::Image<PixelType, 3>>::_GetArrayViewFromImage(itkImage);
//    PyObject *output = PyTuple_New(6);
//    PyObject *spacing = PyList_New(3);
//    PyObject *origin = PyList_New(3);
//    PyObject *size = PyList_New(3);
//    PyObject *direction = PyTuple_New(9);
//    PyObject *cType = PyUnicode_FromString(PipelineUtils::getTypename<PixelType>());

//    for (int i = 0; i < 3; i++)
//    {
//        PyObject* itSpa = PyFloat_FromDouble(itkImage->GetSpacing()[i]);
//        PyObject* itOri = PyFloat_FromDouble(itkImage->GetOrigin()[i]);
//        PyObject* itSize = PyLong_FromLong(itkSize[i]);
//        for (int j = 0; j < 3; j++)
//        {
//            PyObject* itDir = PyFloat_FromDouble(itkDirection[i][j]);
//            if (itDir != nullptr)
//            {
//                PyTuple_SET_ITEM(direction, i*3+j, itDir);
//            }
//        }

//        if (itSpa != nullptr && itOri != nullptr && itSize != nullptr)
//        {
//            PyList_SET_ITEM(spacing, i, itSpa);
//            PyList_SET_ITEM(origin, i, itOri);
//            PyList_SET_ITEM(size, i, itSize);
//        }

//        if (PyErr_Occurred())
//        {
//            Py_DECREF(spacing);
//            Py_DECREF(origin);
//            Py_DECREF(size);
//            Py_DECREF(direction);
//            break;
//        }
//    }
//    if ( pyImg != nullptr && spacing != nullptr && origin != nullptr && size != nullptr )
//    {
//        PyTuple_SET_ITEM(output, 0, pyImg);
//        PyTuple_SET_ITEM(output, 1, spacing);
//        PyTuple_SET_ITEM(output, 2, origin);
//        PyTuple_SET_ITEM(output, 3, size);
//        PyTuple_SET_ITEM(output, 4, direction);
//        PyTuple_SET_ITEM(output, 5, cType);
//    }
//    if (PyErr_Occurred())
//    {
//        Py_DECREF(output);
//        return nullptr;
//    }
//    return output;

//}

//dtkSmartPointer<medAbstractData> PipelineUtils::createMedAbstractDataFromITKPythonImage(PyObject* image_buffer,
//                                                                                           PyObject* shape,
//                                                                                           QList<double> size,
//                                                                                           QList<double> spacing,
//                                                                                           QList<double> origin,
//                                                                                           QList<double> direction,
//                                                                                           QString type,
//                                                                                           PyObject *numOfComponents)
//{
//    dtkSmartPointer<medAbstractData> output = medAbstractDataFactory::instance()->create(type);

//    if ( type == "itkDataImageChar3" )
//    {
//        itk::Image <char,3>::Pointer image = createITKImageFromPyObject<char>(image_buffer,
//                                                                              shape,
//                                                                              size,
//                                                                              spacing,
//                                                                              origin,
//                                                                              direction,
//                                                                              numOfComponents);
//        output->setData(image.GetPointer());
//    }
//    else if ( type == "itkDataImageUChar3" )
//    {
//        itk::Image <unsigned char,3>::Pointer image = createITKImageFromPyObject<unsigned char>(image_buffer,
//                                                                                                shape,
//                                                                                                size,
//                                                                                                spacing,
//                                                                                                origin,
//                                                                                                direction,
//                                                                                                numOfComponents);
//        output->setData(image.GetPointer());
//    }
//    else if ( type == "itkDataImageShort3" )
//    {
//        itk::Image <short,3>::Pointer image = createITKImageFromPyObject<short>(image_buffer,
//                                                                                shape,
//                                                                                size,
//                                                                                spacing,
//                                                                                origin,
//                                                                                direction,
//                                                                                numOfComponents);
//        output->setData(image.GetPointer());
//    }
//    else if ( type == "itkDataImageUShort3" )
//    {
//        itk::Image <unsigned short,3>::Pointer image = createITKImageFromPyObject<unsigned short>(image_buffer,
//                                                                                                  shape,
//                                                                                                  size,
//                                                                                                  spacing,
//                                                                                                  origin,
//                                                                                                  direction,
//                                                                                                  numOfComponents);
//        output->setData(image.GetPointer());
//    }
//    else if ( type == "itkDataImageInt3" )
//    {
//        itk::Image <int,3>::Pointer image = createITKImageFromPyObject<int>(image_buffer,
//                                                                            shape,
//                                                                            size,
//                                                                            spacing,
//                                                                            origin,
//                                                                            direction,
//                                                                            numOfComponents);
//        output->setData(image.GetPointer());
//    }
//    else if ( type == "itkDataImageUInt3" )
//    {
//        itk::Image <unsigned int,3>::Pointer image = createITKImageFromPyObject<unsigned int>(image_buffer,
//                                                                                              shape,
//                                                                                              size,
//                                                                                              spacing,
//                                                                                              origin,
//                                                                                              direction,
//                                                                                              numOfComponents);
//        output->setData(image.GetPointer());
//    }
//    else if ( type == "itkDataImageLong3" )
//    {
//        itk::Image <long,3>::Pointer image = createITKImageFromPyObject<long>(image_buffer,
//                                                                              shape,
//                                                                              size,
//                                                                              spacing,
//                                                                              origin,
//                                                                              direction,
//                                                                              numOfComponents);
//        output->setData(image.GetPointer());
//    }
//    else if ( type== "itkDataImageULong3" )
//    {
//        itk::Image <unsigned long,3>::Pointer image = createITKImageFromPyObject<unsigned long>(image_buffer,
//                                                                                                shape,
//                                                                                                size,
//                                                                                                spacing,
//                                                                                                origin,
//                                                                                                direction,
//                                                                                                numOfComponents);
//        output->setData(image.GetPointer());
//    }
//    else if ( type == "itkDataImageFloat3" )
//    {
//        itk::Image <float,3>::Pointer image = createITKImageFromPyObject<float>(image_buffer,
//                                                                                shape,
//                                                                                size,
//                                                                                spacing,
//                                                                                origin,
//                                                                                direction,
//                                                                                numOfComponents);
//        output->setData(image.GetPointer());
//    }
//    else if ( type == "itkDataImageDouble3" )
//    {
//        itk::Image <double,3>::Pointer image = createITKImageFromPyObject<double>(image_buffer,
//                                                                                  shape,
//                                                                                  size,
//                                                                                  spacing,
//                                                                                  origin,
//                                                                                  direction,
//                                                                                  numOfComponents);
//        output->setData(image.GetPointer());
//    }
//    else
//    {
//        return nullptr;
//    }


//    return output;
//}


//template <typename PixelType>
//typename itk::Image<PixelType, 3>::Pointer PipelineUtils::createITKImageFromPyObject(PyObject* image_buffer,
//                                                                                        PyObject* shape,
//                                                                                        QList<double> size,
//                                                                                        QList<double> spacing,
//                                                                                        QList<double> origin,
//                                                                                        QList<double> direction,
//                                                                                        PyObject* numOfComponents)
//{

//    typedef itk::Image<PixelType, 3> ImageType;

//    Py_INCREF(image_buffer);
//    typename ImageType::Pointer image = itk::PyBuffer<ImageType>::_GetImageViewFromArray(image_buffer, shape, numOfComponents);

//    typename ImageType::SpacingType newSpacing;
//    typename ImageType::PointType newOrigin;
//    typename ImageType::DirectionType newDirection;
//    typename ImageType::SizeType newSize;
//    newDirection.Fill(0);
//    for(int i=0; i<3; i++)
//    {
//        for ( int j= 0; j< 3; j++)
//        {
//            newDirection(i,j) = direction[i*3+j];
//        }
//        newSpacing[i] = spacing[i];
//        newOrigin[i] = origin[i];
//        newSize[i] = size[i];
//    }

//    image->SetSpacing(newSpacing);
//    image->SetOrigin(newOrigin);
//    image->SetDirection(newDirection);
//    typename itk::Image<PixelType, 3>::RegionType region = image->GetLargestPossibleRegion();
//    region.SetSize(newSize);
//    return image;
//}

template<>
const char* PipelineUtils::getTypename<char>()
{
    return "char";
}

template<>
const char* PipelineUtils::getTypename<unsigned char>()
{
    return "unsigned char";
}

template<>
const char* PipelineUtils::getTypename<short>()
{
    return "short";
}

template<>
const char* PipelineUtils::getTypename<unsigned short>()
{
    return "unsigned short";
}

template<>
const char* PipelineUtils::getTypename<int>()
{
    return "int";
}

template<>
const char* PipelineUtils::getTypename<unsigned int>()
{
    return "unsigned int";
}

template<>
const char* PipelineUtils::getTypename<long>()
{
    return "long";
}

template<>
const char* PipelineUtils::getTypename<unsigned long>()
{
    return "unsigned long";
}

template<>
const char* PipelineUtils::getTypename<float>()
{
    return "float";
}

template<>
const char* PipelineUtils::getTypename<double>()
{
    return "double";
}

//dtkSmartPointer<medAbstractData> PipelineUtils::castImage(medAbstractData* data, QString outputPixelType)
//{
//    QString type = data->identifier();
//    if ( type == "itkDataImageChar3" )
//    {
//        return PipelineUtils::castITKImage<char>(data, outputPixelType);
//    }
//    else if ( type == "itkDataImageUChar3" )
//    {
//        return PipelineUtils::castITKImage<unsigned char>(data, outputPixelType);
//    }
//    else if ( type == "itkDataImageShort3" )
//    {
//        return PipelineUtils::castITKImage<short>(data, outputPixelType);
//    }
//    else if ( type == "itkDataImageUShort3" )
//    {
//        return PipelineUtils::castITKImage<unsigned short>(data, outputPixelType);
//    }
//    else if ( type == "itkDataImageInt3" )
//    {
//       return PipelineUtils::castITKImage<int>(data, outputPixelType);
//    }
//    else if ( type == "itkDataImageUInt3" )
//    {
//        return PipelineUtils::castITKImage<unsigned int>(data, outputPixelType);
//    }
//    else if ( type == "itkDataImageLong3" )
//    {
//        return PipelineUtils::castITKImage<long>(data, outputPixelType);
//    }
//    else if ( type== "itkDataImageULong3" )
//    {
//        return PipelineUtils::castITKImage<unsigned long>(data, outputPixelType);
//    }
//    else if ( type == "itkDataImageFloat3" )
//    {
//        return PipelineUtils::castITKImage<float>(data, outputPixelType);
//    }
//    else if ( type == "itkDataImageDouble3" )
//    {
//        return PipelineUtils::castITKImage<double>(data, outputPixelType);
//    }
//    else
//    {
//        qDebug()<<"medPipelineUtils: wrong type given. Unsupported " <<type<< ".";
//        return nullptr;
//    }
//}

QString PipelineUtils::getIdentifier(medAbstractData *data)
{
    return data->identifier();
}

//template <class InputPixelType>
//dtkSmartPointer<medAbstractData> PipelineUtils::castITKImage(medAbstractData* inputData, QString outputPixelType)
//{
//    if ( outputPixelType == "itkDataImageChar3" )
//    {
//        return PipelineUtils::castITKImage<InputPixelType, char>(inputData, outputPixelType);
//    }
//    else if ( outputPixelType == "itkDataImageUChar3" )
//    {
//        return PipelineUtils::castITKImage<InputPixelType, unsigned char>(inputData, outputPixelType);
//    }
//    else if ( outputPixelType == "itkDataImageShort3" )
//    {
//        return PipelineUtils::castITKImage<InputPixelType, short>(inputData, outputPixelType);
//    }
//    else if ( outputPixelType == "itkDataImageUShort3" )
//    {
//        return PipelineUtils::castITKImage<InputPixelType, unsigned short>(inputData, outputPixelType);
//    }
//    else if ( outputPixelType == "itkDataImageInt3" )
//    {
//        return PipelineUtils::castITKImage<InputPixelType, int>(inputData, outputPixelType);
//    }
//    else if ( outputPixelType == "itkDataImageUInt3" )
//    {
//        return PipelineUtils::castITKImage<InputPixelType, unsigned int>(inputData, outputPixelType);
//    }
//    else if ( outputPixelType == "itkDataImageLong3" )
//    {
//        return PipelineUtils::castITKImage<InputPixelType, long>(inputData, outputPixelType);
//    }
//    else if ( outputPixelType== "itkDataImageULong3" )
//    {
//        return PipelineUtils::castITKImage<InputPixelType, unsigned long>(inputData, outputPixelType);
//    }
//    else if ( outputPixelType == "itkDataImageFloat3" )
//    {
//        return PipelineUtils::castITKImage<InputPixelType, float>(inputData, outputPixelType);
//    }
//    else if ( outputPixelType == "itkDataImageDouble3" )
//    {
//        return PipelineUtils::castITKImage<InputPixelType, double>(inputData, outputPixelType);
//    }
//    else
//    {
//        qDebug()<<"medPipelineUtils: wrong type given. Unsupported " <<outputPixelType<< ".";
//        return nullptr;
//    }
//    return nullptr;
//}

//template <class InputPixelType, class OutputPixelType>
//dtkSmartPointer<medAbstractData> PipelineUtils::castITKImage(medAbstractData* data, QString type)
//{
//    dtkSmartPointer<medAbstractData> output = medAbstractDataFactory::instance()->create(type);
//    typedef itk::Image<InputPixelType, 3> InputImageType;
//    typedef itk::Image<OutputPixelType, 3> OutputImageType;
//    typedef itk::CastImageFilter< InputImageType, OutputImageType > CastFilterType;

//    typename CastFilterType::Pointer  caster = CastFilterType::New();
//    typename InputImageType::Pointer im = static_cast<InputImageType*>(data->data());
//    caster->SetInput(im);
//    caster->Update();
//    output->setData(caster->GetOutput());
//    return output;
//}

dtkSmartPointer<medAbstractData> PipelineUtils::threshold(dtkSmartPointer<medAbstractData> input,
                                                             double lowerValue, double upperValue, double threshold, int outsideValue, ThresholdType thresholdType)
{
    dtkSmartPointer<medAbstractProcessLegacy> thresholdProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("itkThresholdingProcess");
    thresholdProcess->setInput(input);

    thresholdProcess->setParameter(thresholdType);
    thresholdProcess->setParameter(threshold, 0);
    thresholdProcess->setParameter(lowerValue, 1);
    thresholdProcess->setParameter(upperValue, 2);
    thresholdProcess->setParameter(double(outsideValue), 3);

    thresholdProcess->update();
    return outputWithCopiedWindowLevel(thresholdProcess, input);
}

//dtkSmartPointer<medAbstractData> PipelineUtils::createMedAbstractDataFromVTKMesh(vtkPolyData* vtkObject, QString identifier)
//{
//    dtkSmartPointer<medAbstractData> output = medAbstractDataFactory::instance()->create(identifier);
//    vtkMetaSurfaceMesh* metaMesh = vtkMetaSurfaceMesh::New();
//    metaMesh->SetDataSet(vtkObject);
//    output->setData(metaMesh);
//    metaMesh->Delete();
//    return output;
//}

//dtkSmartPointer<medAbstractData> PipelineUtils::createMedAbstractDataFromVTKMesh(vtkUnstructuredGrid* vtkObject, QString identifier)
//{
//    dtkSmartPointer<medAbstractData> output = medAbstractDataFactory::instance()->create(identifier);
//    vtkMetaVolumeMesh* metaMesh = vtkMetaVolumeMesh::New();
//    metaMesh->SetDataSet(vtkObject);
//    output->setData(metaMesh);
//    metaMesh->Delete();
//    return output;
//}

//bool PipelineUtils::isVolumeMesh(medAbstractData* data)
//{
//    if ( data->identifier().contains("vtkDataMesh") ||
//         data->identifier().contains("EPMap") )
//    {
//        vtkMetaDataSet* inputMetaDataSet = static_cast<vtkMetaDataSet*>(data->data());

//        if (inputMetaDataSet->GetType() == vtkMetaDataSet::VTK_META_VOLUME_MESH)
//        {
//            return true;
//        }
//    }
//    return false;
//}

//vtkPolyData* PipelineUtils::getVTKPolyData(medAbstractData* data)
//{
//    if ( data->identifier().contains("vtkDataMesh") ||
//         data->identifier().contains("EPMap") )
//    {
//        vtkMetaDataSet* inputMetaDataSet = static_cast<vtkMetaDataSet*>(data->data());
//        vtkPolyData* inputMesh = dynamic_cast<vtkPolyData*>(inputMetaDataSet->GetDataSet());
//        return inputMesh;
//    }
//    return nullptr;
//}

//vtkUnstructuredGrid* PipelineUtils::getVTKUnstructuredGrid(medAbstractData* data)
//{
//    if ( data->identifier().contains("vtkDataMesh") ||
//         data->identifier().contains("EPMap") )
//    {
//        vtkMetaDataSet* inputMetaDataSet = static_cast<vtkMetaDataSet*>(data->data());
//        vtkUnstructuredGrid* inputMesh = dynamic_cast<vtkUnstructuredGrid*>(inputMetaDataSet->GetDataSet());
//        return inputMesh;
//    }
//    return nullptr;
//}

} // namespace med::pipeline
