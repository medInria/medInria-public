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
#include <medICPFilter.h>
#include <medUtilities.h>
#include <medUtilitiesVTK.h>

#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkLandmarkTransform.h>
#include <vtkLinearTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkSmartPointer.h>
#include <vtkTransformPolyDataFilter.h>


// /////////////////////////////////////////////////////////////////
// iterativeClosestPointProcessPrivate
// /////////////////////////////////////////////////////////////////

class iterativeClosestPointProcessPrivate
{
public:
    dtkSmartPointer <medAbstractData> inputSource;
    dtkSmartPointer <medAbstractData> inputTarget;
    dtkSmartPointer <medAbstractData> output;
    
    int StartByMatchingCentroids;
    int Transformation;
    int CheckMeanDistance;

    double ScaleFactor;
    int MaxNumIterations;
    int MaxNumLandmarks;
    double MaxMeanDistance;
    int exportMatrixState;
    QString exportMatrixFilePath;
    QString sourceName;
    QString targetName;

    vtkSmartPointer<vtkMatrix4x4> TransformMatrix;
    double mean, variance, median;
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

    d->StartByMatchingCentroids = 1;
    d->Transformation           = 0;
    d->CheckMeanDistance        = 0;
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
            d->StartByMatchingCentroids = data;
            break;
        case 1:
            d->Transformation = data;
            break;
        case 2:
            d->CheckMeanDistance = data;
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

    vtkSmartPointer<medICPFilter> ICPFilter = vtkSmartPointer<medICPFilter>::New();

    vtkMetaDataSet *source_dataset = static_cast<vtkMetaDataSet*>(d->inputSource->data());
    vtkMetaDataSet *target_dataset = static_cast<vtkMetaDataSet*>(d->inputTarget->data());

    ICPFilter->SetSource(static_cast<vtkPolyData*>(source_dataset->GetDataSet()));
    ICPFilter->SetTarget(static_cast<vtkPolyData*>(target_dataset->GetDataSet()));
    ICPFilter->SetMaximumNumberOfIterations(d->MaxNumIterations);
    ICPFilter->SetMaximumNumberOfLandmarks(d->MaxNumLandmarks);
    ICPFilter->SetMaximumMeanDistance(d->MaxMeanDistance);
    ICPFilter->SetScaleFactor(d->ScaleFactor);

    if(d->CheckMeanDistance)
    {
        ICPFilter->CheckMeanDistanceOn();
    }
    if(d->StartByMatchingCentroids)
    {
        ICPFilter->StartByMatchingCentroidsOn(); //align center of masses as first transformation
    }

    //Set the number of degrees of freedom to constrain the solution to.
    //Rigidbody (VTK_LANDMARK_RIGIDBODY): rotation and translation only.
    //Similarity (VTK_LANDMARK_SIMILARITY): rotation, translation and isotropic scaling.
    //Affine (VTK_LANDMARK_AFFINE): collinearity is preserved. Ratios of distances along a line are preserved.
    //The default is similarity.
    switch (d->Transformation)
    {
        case 0:
            ICPFilter->GetLandmarkTransform()->SetMode(VTK_LANDMARK_RIGIDBODY);
            break;
        case 1:
            ICPFilter->GetLandmarkTransform()->SetMode(VTK_LANDMARK_SIMILARITY);
            break;
        case 2:
            ICPFilter->GetLandmarkTransform()->SetMode(VTK_LANDMARK_AFFINE);
            break;
        default:
            break;
    }

    ICPFilter->Update();

    //bring the source to the target
    vtkSmartPointer<vtkTransformPolyDataFilter> TransformFilter2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    TransformFilter2->SetInputData(ICPFilter->GetSource());
    TransformFilter2->SetTransform(ICPFilter);
    TransformFilter2->Update();

    ICPFilter->GetFREStats(d->mean, d->variance, d->median);

    d->TransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    d->TransformMatrix->DeepCopy(ICPFilter->GetMatrix());

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

    vtkPolyData *output_polyData = TransformFilter2->GetOutput();
    output_polyData->GetFieldData()->AddArray(icpTransformArray);

    // create array to hold FRE stats
    vtkDoubleArray* FREArray = vtkDoubleArray::New();
    FREArray->SetNumberOfComponents(3);
    FREArray->SetNumberOfTuples(1);
    FREArray->SetName("FRE Stats");
    FREArray->SetTuple3(0, d->mean, d->variance, d->median);
    // add to outputs
    output_polyData->GetFieldData()->AddArray(FREArray);
    FREArray->Delete();

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
        vtkLinearTransform* transform = vtkLinearTransform::SafeDownCast(TransformFilter2->GetTransform());
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
        matrixStr += "# Scale Factor = " + QByteArray::number(d->ScaleFactor) + "\n";
        if (d->StartByMatchingCentroids)
        {
            matrixStr += "# Start by matching centroids = True \n";
        }
        else
        {
            matrixStr += "# Start by matching centroids = False \n";
        }
        switch (d->Transformation)
        {
            case 0:
                matrixStr += "# Transformation = Rigid Body \n";
                break;
            case 1:
                matrixStr += "# Transformation = Similarity \n";
                break;
            case 2:
                matrixStr += "# Transformation = Affine \n";
                break;
        }
        if (d->CheckMeanDistance)
        {
            matrixStr += "# Check mean distance = True \n";
        }
        else
        {
            matrixStr += "# Check mean distance = False \n";
        }
        matrixStr += "# Max Mean Distance = " + QByteArray::number(d->MaxMeanDistance) + "\n";
        matrixStr += "# Max Num Iterations = " + QByteArray::number(d->MaxNumIterations) + "\n";
        matrixStr += "# Max Num Landmarks = " + QByteArray::number(d->MaxNumLandmarks) + "\n";
        matrixStr += "# Fiducial Registration Error = " + QByteArray::number(d->mean);
        matrixStr += " +- " + QByteArray::number(d->variance);
        matrixStr += ", Median = " + QByteArray::number(d->median) + "\n";

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
