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
#include <medRefineMeshProcess.h>
#include <medUtilities.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkMetaSurfaceMesh.h>

//vtk
#include <vtkButterflySubdivisionFilter.h>
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>

// /////////////////////////////////////////////////////////////////
// medRefineMeshProcessPrivate
// /////////////////////////////////////////////////////////////////

class medRefineMeshProcessPrivate
{
public:
    dtkSmartPointer <medAbstractData> input;
    dtkSmartPointer <medAbstractData> output;
    double nbOfSubdivisions;
    bool changeMetaData;
};

// /////////////////////////////////////////////////////////////////
// medRefineMeshProcess
// /////////////////////////////////////////////////////////////////

medRefineMeshProcess::medRefineMeshProcess() : d(new medRefineMeshProcessPrivate)
{
    d->input = nullptr;
    d->output = nullptr;
    d->nbOfSubdivisions = 1.0;
    d->changeMetaData = true;
}

medRefineMeshProcess::~medRefineMeshProcess()
{
    delete d;
    d = nullptr;
}

bool medRefineMeshProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medRefineMeshProcess", createmedRefineMeshProcess);
}

QString medRefineMeshProcess::description() const
{
    return "medRefineMeshProcess";
}

void medRefineMeshProcess::setInput(medAbstractData *data, int channel)
{
    d->input = data;
}

void medRefineMeshProcess::setParameter(double data, int channel)
{
    d->nbOfSubdivisions = data;
}

void medRefineMeshProcess::setParameter(int data, int channel)
{
    if(channel == 1)
    {
        d->changeMetaData = data;
    }
}

vtkMetaDataSet* medRefineMeshProcess::refineOneMetaDataSet(vtkMetaDataSet *inputMetaDaset)
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

        vtkButterflySubdivisionFilter *loopRefineFilter = vtkButterflySubdivisionFilter::New();
        loopRefineFilter->SetInputConnection(triFilter->GetOutputPort());
        loopRefineFilter->SetNumberOfSubdivisions(d->nbOfSubdivisions);
        loopRefineFilter->SetGlobalWarningDisplay(true);
        loopRefineFilter->Update();

        vtkMetaSurfaceMesh *smesh = vtkMetaSurfaceMesh::New();
        smesh->SetDataSet(loopRefineFilter->GetOutput());
        loopRefineFilter->Delete();

        return smesh;
    }
}

int medRefineMeshProcess::update()
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
            vtkMetaDataSet *outputMetaDataSet = refineOneMetaDataSet(inputMetaDataSet);
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
        vtkMetaDataSet *outputMetaDataSet = refineOneMetaDataSet(inputMetaDataSet);
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
        medUtilities::setDerivedMetaData(d->output, d->input, "refined");
    }

    return medAbstractProcessLegacy::SUCCESS;
}

medAbstractData * medRefineMeshProcess::output()
{
    return d->output;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createmedRefineMeshProcess()
{
    return new medRefineMeshProcess;
}
