/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
//dtk
#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

//medinria
#include <medAbstractDataFactory.h>
#include <medDecimateMeshProcess.h>
#include <medUtilities.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkMetaSurfaceMesh.h>

//vtk
#include <vtkDecimatePro.h>
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>

// /////////////////////////////////////////////////////////////////
// medDecimateMeshProcessPrivate
// /////////////////////////////////////////////////////////////////

class medDecimateMeshProcessPrivate
{
public:
    dtkSmartPointer <medAbstractData> input;
    dtkSmartPointer <medAbstractData> output;
    double targetReduction;
    bool preserveTopology;
    bool changeMetaData;
};

// /////////////////////////////////////////////////////////////////
// medDecimateMeshProcess
// /////////////////////////////////////////////////////////////////

medDecimateMeshProcess::medDecimateMeshProcess() : d(new medDecimateMeshProcessPrivate)
{
    d->input = nullptr;
    d->output = nullptr;
    d->targetReduction = 0.0;
    d->preserveTopology = false;
    d->changeMetaData = true;
}

medDecimateMeshProcess::~medDecimateMeshProcess()
{
    delete d;
    d = nullptr;
}

bool medDecimateMeshProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medDecimateMeshProcess", createmedDecimateMeshProcess);
}

QString medDecimateMeshProcess::description() const
{
    return "medDecimateMeshProcess";
}

void medDecimateMeshProcess::setInput(medAbstractData *data, int channel)
{
    d->input = data;
}

void medDecimateMeshProcess::setParameter(double data, int channel)
{
    if (channel == 0)
    {
        d->targetReduction = data;
    }
}

void medDecimateMeshProcess::setParameter(int data, int channel)
{
    if (channel == 1)
    {
        d->preserveTopology = data;
    }
    else if(channel == 2)
    {
        d->changeMetaData = data;
    }
}

vtkMetaDataSet* medDecimateMeshProcess::decimateOneMetaDataSet(vtkMetaDataSet *inputMetaDaset)
{
    vtkPolyData *polyData = dynamic_cast<vtkPolyData*>(inputMetaDaset->GetDataSet());

    if(!polyData)
    {
        emit polyDataCastFailure();
        return nullptr;
    }
    else
    {
        vtkSmartPointer<vtkTriangleFilter> triFilter = vtkSmartPointer<vtkTriangleFilter>::New();
        triFilter->SetInputData(polyData);
        triFilter->Update();

        int originalNbOfPolygons = polyData->GetNumberOfPolys();
        // nb of polygons expected after processing
        int theoreticalFinalNbOfPolygons = ceil (originalNbOfPolygons * (1-d->targetReduction));

        vtkDecimatePro* contourDecimated = vtkDecimatePro::New();
        contourDecimated->SetInputConnection(triFilter->GetOutputPort());
        contourDecimated->SetTargetReduction(d->targetReduction);
        // Turn on/off whether to preserve the mesh topology.
        // If on, splitting and hole elimination will not occur.
        // Can limit the maximum reduction that may be achieved.
        contourDecimated->SetPreserveTopology(d->preserveTopology);
        contourDecimated->Update();

        vtkMetaSurfaceMesh * smesh = vtkMetaSurfaceMesh::New();
        smesh->SetDataSet(contourDecimated->GetOutput());
        //If we can't reach the desired decimation
        if (contourDecimated->GetOutput()->GetNumberOfPolys() > theoreticalFinalNbOfPolygons)
        {
            emit warning();
        }
        contourDecimated->Delete();

        return smesh;
    }
}

int medDecimateMeshProcess::update()
{
    progressed(0);

    if (!d->input)
    {
        return medAbstractProcessLegacy::FAILURE;
    }
    if(!d->input->identifier().contains("vtkDataMesh"))
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    d->output = medAbstractDataFactory::instance()->createSmartPointer(d->input->identifier());

    if (d->input->identifier() == "vtkDataMesh4D")
    {
        vtkMetaDataSetSequence *inputSequence = static_cast<vtkMetaDataSetSequence*>(d->input->data());
        vtkMetaDataSetSequence *outputSequence = vtkMetaDataSetSequence::New();

        foreach(vtkMetaDataSet *inputMetaDataSet, inputSequence->GetMetaDataSetList())
        {
            vtkMetaDataSet *outputMetaDataSet = decimateOneMetaDataSet(inputMetaDataSet);
            outputMetaDataSet->SetTime(inputMetaDataSet->GetTime());
            if (outputMetaDataSet == nullptr)
            {
                return medAbstractProcessLegacy::FAILURE;
            }
            outputSequence->AddMetaDataSet(outputMetaDataSet);
            outputMetaDataSet->Delete();
        }
        d->output->setData(outputSequence);
        outputSequence->Delete();
    }
    else
    {
        vtkMetaDataSet *inputMetaDataSet = static_cast<vtkMetaDataSet*>(d->input->data());
        vtkMetaDataSet *outputMetaDataSet = decimateOneMetaDataSet(inputMetaDataSet);
        if (outputMetaDataSet == nullptr)
        {
            return medAbstractProcessLegacy::FAILURE;
        }
        d->output->setData(outputMetaDataSet);
        outputMetaDataSet->Delete();
    }

    progressed(100);

    if (d->changeMetaData)
    {
        medUtilities::setDerivedMetaData(d->output, d->input, "decimated");
    }

    return medAbstractProcessLegacy::SUCCESS;
}

medAbstractData * medDecimateMeshProcess::output()
{
    return d->output;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createmedDecimateMeshProcess()
{
    return new medDecimateMeshProcess;
}
