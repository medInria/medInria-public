/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "iterativeClosestPointProcess.h"

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractDataFactory.h>
#include <medUtilities.h>
#include <medUtilitiesVTK.h>

#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkICPFilter.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkLandmarkTransform.h>
#include <vtkLinearTransform.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>

// /////////////////////////////////////////////////////////////////
// iterativeClosestPointProcessPrivate
// /////////////////////////////////////////////////////////////////

class iterativeClosestPointProcessPrivate
{
public:
    dtkSmartPointer <medAbstractData> inputSource;
    dtkSmartPointer <medAbstractData> inputTarget;
    dtkSmartPointer <medAbstractData> output;
    
    int bStartByMatchingCentroids;
    int bTransformation;
    int bCheckMeanDistance;

    double ScaleFactor;
    int MaxNumIterations;
    int MaxNumLandmarks;
    double MaxMeanDistance;
    int exportMatrixState;
    QString exportMatrixFilePath;
    QString sourceName;
    QString targetName;

    vtkSmartPointer<vtkMatrix4x4> TransformMatrix;
};

// /////////////////////////////////////////////////////////////////
// iterativeClosestPointProcess
// /////////////////////////////////////////////////////////////////

iterativeClosestPointProcess::iterativeClosestPointProcess()
    : medAbstractProcessLegacy(), d(new iterativeClosestPointProcessPrivate)
{
    d->inputSource = nullptr;
    d->inputTarget = nullptr;
    d->output = nullptr;

    d->bStartByMatchingCentroids = 1;
    d->bTransformation           = 0;
    d->bCheckMeanDistance        = 0;
    d->ScaleFactor               = 1;
    d->MaxNumIterations          = 100;
    d->MaxNumLandmarks           = 1000;
    d->MaxMeanDistance           = 1;
}

iterativeClosestPointProcess::~iterativeClosestPointProcess()
{
    delete d;
    d = nullptr;
}

bool iterativeClosestPointProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("iterativeClosestPointProcess", createiterativeClosestPointProcess);
}

QString iterativeClosestPointProcess::description() const
{
    return "iterativeClosestPointProcess";
}

void iterativeClosestPointProcess::setInput(medAbstractData *data, int channel)
{
    if (data && (data->identifier().contains("vtkDataMesh") ||
                 data->identifier().contains("EPMap")))
    {
        if (channel == 0)
        {
            d->inputSource = data;
        }

        if (channel == 1)
        {
            d->inputTarget = data;
        }
    }
}    

void iterativeClosestPointProcess::setParameter(double data, int channel)
{
    switch (channel)
    {
        case 3:
            d->MaxMeanDistance = data;
            break;
        case 6:
            d->ScaleFactor = data;
            break;
    }
}

void iterativeClosestPointProcess::setParameter(int data, int channel)
{
    switch (channel)
    {
        case 0:
            d->bStartByMatchingCentroids = data;
            break;
        case 1:
            d->bTransformation = data;
            break;
        case 2:
            d->bCheckMeanDistance = data;
            break;
        case 4:
            d->MaxNumIterations = data;
            break;
        case 5:
            d->MaxNumLandmarks = data;
            break;
        case 7:
            d->exportMatrixState = data;
            break;
    }
}

void iterativeClosestPointProcess::setParameter(QString data, int channel)
{
    switch (channel)
    {
        case 8:
            d->exportMatrixFilePath = data;
            break;
        case 9:
            d->sourceName = data;
            break;
        case 10:
            d->targetName = data;
            break;
    }
}



int iterativeClosestPointProcess::update()
{
    if ( !d->inputSource || !d->inputTarget )
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    vtkSmartPointer<vtkICPFilter> ICPFilter = vtkSmartPointer<vtkICPFilter>::New();

    vtkMetaDataSet *source_dataset = static_cast<vtkMetaDataSet*>(d->inputSource->data());
    vtkMetaDataSet *target_dataset = static_cast<vtkMetaDataSet*>(d->inputTarget->data());

    ICPFilter->SetSource(static_cast<vtkPolyData*>(source_dataset->GetDataSet()));
    ICPFilter->SetTarget(static_cast<vtkPolyData*>(target_dataset->GetDataSet()));

    ICPFilter->SetbStartByMatchingCentroids(d->bStartByMatchingCentroids);
    ICPFilter->SetbTransformation(d->bTransformation);
    ICPFilter->SetbCheckMeanDistance(d->bCheckMeanDistance);
    ICPFilter->SetScaleFactor(d->ScaleFactor);
    ICPFilter->SetMaxNumIterations(d->MaxNumIterations);
    ICPFilter->SetMaxNumLandmarks(d->MaxNumLandmarks);
    ICPFilter->SetMaxMeanDistance(d->MaxMeanDistance);

    ICPFilter->Update();

    vtkLinearTransform* transform = ICPFilter->GetLinearTransform();
    d->TransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    d->TransformMatrix->DeepCopy(transform->GetMatrix());

    if (d->exportMatrixState == 2)
    {
        exportTransformMatrix();
    }

    // save the transformation matrix
    vtkSmartPointer<vtkDoubleArray> icpTransformArray = vtkSmartPointer<vtkDoubleArray>::New();
    icpTransformArray->SetNumberOfTuples(16);
    icpTransformArray->SetNumberOfComponents(1);
    icpTransformArray->SetName("ICP_TransformMatrix");
    double* data = icpTransformArray->GetPointer(0);
    double* mat  = d->TransformMatrix->GetData();
    std::memcpy(data, mat, 16 * sizeof(*data));

    vtkPolyData *output_polyData = ICPFilter->GetOutput();
    output_polyData->GetFieldData()->AddArray(icpTransformArray);

    vtkMetaSurfaceMesh *output_mesh = vtkMetaSurfaceMesh::New();
    output_mesh->SetDataSet(output_polyData);

    d->output = medAbstractDataFactory::instance()->createSmartPointer(d->inputSource->identifier());
    d->output->setData(output_mesh);
    medUtilities::setDerivedMetaData(d->output, d->inputSource, "ICP");
    output_mesh->Delete();

    // manually transform catheter coordinates;
    if (d->output->identifier().contains("EPMap"))
    {
        QStringList arrayNames;
        arrayNames << "KT_Coordinates" << "_catheter_electrode_positions";
        vtkLinearTransform* transform = ICPFilter->GetLinearTransform();
        medUtilitiesVTK::transformCoordinates(d->output, arrayNames, transform);
    }

    return medAbstractProcessLegacy::SUCCESS;
}


void iterativeClosestPointProcess::exportTransformMatrix()
{
    if (d->exportMatrixState == 2)
    {
        QByteArray matrixStr;
        matrixStr += "# Transformation matrix from " + d->sourceName + " to " + d->targetName + "\n";
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                matrixStr += QByteArray::number(d->TransformMatrix->GetElement(i, j)) + "\t";
            }
            matrixStr += "\n";
        }

        QFile f(d->exportMatrixFilePath);
        if (!f.open(QIODevice::WriteOnly))
        {
            qDebug() << "Can't open file" << d->exportMatrixFilePath;
            return;
        }

        f.write(matrixStr);
        f.close();

        qDebug() << "Done exporting tranform!";
    }
}

medAbstractData * iterativeClosestPointProcess::output()
{
    return d->output;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createiterativeClosestPointProcess()
{
    return new iterativeClosestPointProcess;
}
