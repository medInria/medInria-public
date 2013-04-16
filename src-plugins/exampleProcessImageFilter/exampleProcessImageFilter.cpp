/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/




#include "exampleProcessImageFilter.h"
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataImage.h>

#include <itkImage.h>
#include <itkMaskImageFilter.h>
#include <itkAddImageFilter.h>
#include <itkConnectedThresholdImageFilter.h>
#include "itkImageFileWriter.h"




// /////////////////////////////////////////////////////////////////
// exampleProcessImageFilterPrivate
// /////////////////////////////////////////////////////////////////

class exampleProcessImageFilterPrivate
{
public:
    medAbstractDataImage *inputA;
    medAbstractDataImage *inputB;
    medAbstractDataImage *output;

public:
    int x;
    int y;
    int z;
    int lowerThreshold;

public:
    exampleProcessImageFilter::Option option;
};

// /////////////////////////////////////////////////////////////////
// exampleProcessImageFilter
// /////////////////////////////////////////////////////////////////

exampleProcessImageFilter::exampleProcessImageFilter() : dtkAbstractProcess(), d(new exampleProcessImageFilterPrivate)
{
    d->inputA = 0;
    d->inputB = 0;
    d->output = 0;

    d->x = 0;
    d->y = 0;
    d->z = 0;
    d->lowerThreshold = 1.0;
    d->option = exampleProcessImageFilter::optionA;

   // this->addMetaData("category","tractography");
}

exampleProcessImageFilter::~exampleProcessImageFilter()
{
    if (d->inputA)
        delete d->inputA;

    if (d->inputB)
        delete d->inputB;

    delete d;

    d = NULL;
}

bool exampleProcessImageFilter::registered()
{
    qDebug() << "HOLAS able to register exampleProcessImageFilter toolbox";
    return dtkAbstractProcessFactory::instance()->registerProcessType("exampleProcessImageFilter", createExampleProcessImageFilter);
}

QString exampleProcessImageFilter::description() const
{
    return "exampleProcessImageFilter";
}


void exampleProcessImageFilter::setLowerThreshold(int lowerTh)
{
    d->lowerThreshold = lowerTh;
}

void exampleProcessImageFilter::setPoint(int x, int y, int z)
{
    d->x = x;
    d->y = y;
    d->z = z;
}

void exampleProcessImageFilter::setInput(dtkAbstractData *data)
{
    Q_UNUSED(data);
}

void exampleProcessImageFilter::setInput(dtkAbstractData *data, int channel)
{
    if(channel == 0)
    {
       d->inputA = dynamic_cast<medAbstractDataImage *>(data);
       qDebug("Channel 0 loaded! ");
    }

    if(channel == 1)
    {
       d->inputB=dynamic_cast<medAbstractDataImage *>(data);
       qDebug("Channel 1 loaded! ");
    }

}

void exampleProcessImageFilter::setOption(Option option)
{
    d->option = option;
}

dtkAbstractData *exampleProcessImageFilter::output()
{
    return d->output;
}

int exampleProcessImageFilter::update()
{
    typedef unsigned short PixelType;
    const unsigned int Dimension = 3;
    typedef itk::Image< PixelType, Dimension > ImageType;

    qDebug() << d->inputA->identifier();

    if (d->option == optionA)
    {

        if (d->inputA->identifier()=="itkDataImageUShort3" && d->inputB->identifier()=="itkDataImageUShort3")
        {
            typedef itk::MaskImageFilter< ImageType,ImageType,ImageType >    MaskFilterType;
            MaskFilterType::Pointer maskFilter = MaskFilterType::New();

            //convert from dtkImages to ItkImages and set as intputs for the itk::MaskImageFilter
            maskFilter->SetInput1(dynamic_cast<ImageType*>((itk::Object*)(d->inputA->output())));
            maskFilter->SetInput2(dynamic_cast<ImageType*>((itk::Object*)(d->inputB->output())));

            //Create an image where the output of the filter is going to be displayed
            d->output =dynamic_cast<medAbstractDataImage *>(dtkAbstractDataFactory::instance()->create ("itkDataImageUShort3"));

            maskFilter->Update();

            //Set the data for the output
            d->output->setData(maskFilter->GetOutput());

        }
        else if(d->inputA->identifier()=="itkDataImageDouble4" && d->inputB->identifier()=="itkDataImageDouble4")
        {
            typedef double PixelType4;
            const unsigned int Dimension4 = 4;
            typedef itk::Image< PixelType4, Dimension4 > ImageType4;

            typedef itk::MaskImageFilter< ImageType4,ImageType4,ImageType4 >    MaskFilterType4;
            MaskFilterType4::Pointer maskFilter4 = MaskFilterType4::New();

            //convert from dtkImages to ItkImages and set as intputs for the itk::MaskImageFilter
            maskFilter4->SetInput1(dynamic_cast<ImageType4*>((itk::Object*)(d->inputA->output())));
            maskFilter4->SetInput2(dynamic_cast<ImageType4*>((itk::Object*)(d->inputB->output())));

            maskFilter4->Update();

            //Create an image where the output of the filter is going to be displayed
            d->output =dynamic_cast<medAbstractDataImage *> (dtkAbstractDataFactory::instance()->create ("itkDataImageDouble4"));

            //Set the data for the output
            d->output->setData(maskFilter4->GetOutput());

            typedef itk::ImageFileWriter< ImageType4 > WriterType;
            WriterType::Pointer writer = WriterType::New();
            writer->SetFileName("/user/jgarciag/home/AnalyzeStuff/headerModify/ResulHalfMask4D.mha");
            writer->SetInput(/*dynamic_cast<ImageType4*>((itk::Object*)(d->output->output()))*/maskFilter4->GetOutput());
            writer->Update();
        }
        else
            qDebug("Not the right itkDataImageUShort3 type of images");
    }

    else if (d->option == optionB)
    {

        if (d->inputA->identifier()=="itkDataImageUShort3" && d->inputB->identifier()=="itkDataImageUShort3")
        {
            typedef itk::AddImageFilter< ImageType,ImageType,ImageType >    AddFilterType;
            AddFilterType::Pointer AddFilter = AddFilterType::New();

            AddFilter->SetInput1(dynamic_cast<ImageType*>((itk::Object*)(d->inputA->output())));
            AddFilter->SetInput2(dynamic_cast<ImageType*>((itk::Object*)(d->inputB->output())));

            d->output =dynamic_cast<medAbstractDataImage *> (dtkAbstractDataFactory::instance()->create ("itkDataImageUShort3"));
            AddFilter->Update();
            d->output->setData(AddFilter->GetOutput());


        }
        else
            qDebug("Not the right itkDataImageUShort3 type of images");
    }

    else if (d->option == optionC)
    {
        if (d->inputA->identifier()=="itkDataImageUShort3" && d->inputB->identifier()=="itkDataImageUShort3")
        {
            typedef itk::ConnectedThresholdImageFilter< ImageType, ImageType > ConnectedFilterType;
            ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();
            connectedThreshold->SetInput( dynamic_cast<ImageType*>((itk::Object*)(d->inputA->output())) );
            connectedThreshold->SetLower((d->lowerThreshold));
            connectedThreshold->SetUpper((d->lowerThreshold)+150);

            ImageType::IndexType  index;

            index[0] = d->x;
            index[1] = d->y;
            index[2] = d->z;

            connectedThreshold->SetSeed( index );

            connectedThreshold->Update();

            d->output =dynamic_cast<medAbstractDataImage *> (dtkAbstractDataFactory::instance()->create ("itkDataImageUShort3"));

            d->output->setData(connectedThreshold->GetOutput());
        }
        else
            qDebug("Not the right itkDataImageUShort3 type of image");
    }
//    emit progressed (100);
     qDebug("Adios");
    return 0;
}


// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createExampleProcessImageFilter()
{
    return new exampleProcessImageFilter;
}
