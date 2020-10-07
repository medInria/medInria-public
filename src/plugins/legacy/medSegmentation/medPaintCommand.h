#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractPaintCommand.h>

#include <QtCore>

#include <itkImage.h>

class medPaintCommandPrivate;

class medClickAndMoveEventFilter;
class medAbstractImageView;
class medAbstractData;

class MEDVIEWSEGMENTATIONPLUGIN_EXPORT medPaintCommand : public medAbstractPaintCommand
{
public:
    medPaintCommand(medPaintCommandOptions *options, QUndoCommand *parent = nullptr);
    virtual ~medPaintCommand();

    virtual void undo();
    virtual void redo();

    void paint();

private:
    medPaintCommandPrivate *d;
};
