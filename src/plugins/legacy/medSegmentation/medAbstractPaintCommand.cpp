/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractPaintCommand.h>

#include <medAbstractImageView.h>
#include <itkLabelImageToLabelMapFilter.h>


medPaintCommandManager *medPaintCommandManager::instance()
{
    if(!s_instance)
        s_instance = new medPaintCommandManager;

    return s_instance;
}

LabelMapType::Pointer medPaintCommandManager::labelMap(MaskType* mask)
{
    if(!labelMaps.contains(mask))
    {
        typedef itk::LabelImageToLabelMapFilter<MaskType, LabelMapType> ImageToMapFilter;
        ImageToMapFilter::Pointer filter = ImageToMapFilter::New();

        filter->SetInput(mask);
        filter->Update();

        LabelMapType::Pointer labelMap =  LabelMapType::New();
        labelMap ->Graft(filter->GetOutput());

        labelMaps.insert(mask, labelMap);
    }
    return labelMaps[mask];
}

medPaintCommandManager *medPaintCommandManager::s_instance = NULL;


//-------------------------------------------------------------------------


class medAbstractPaintCommandPrivate
{
public:
    medAbstractPaintCommand* previousCommand;
    medPaintCommandOptions *option;
};


//-------------------------------------------------------------------------


medAbstractPaintCommand::medAbstractPaintCommand(medPaintCommandOptions *options, QUndoCommand *parent):
    QUndoCommand(parent), d(new medAbstractPaintCommandPrivate)
{
    d->option = options;
}

medAbstractPaintCommand::~medAbstractPaintCommand()
{
    delete d->option;
    delete d;
    d = 0;
}

medPaintCommandOptions* medAbstractPaintCommand::options() const
{
    return d->option;
}
