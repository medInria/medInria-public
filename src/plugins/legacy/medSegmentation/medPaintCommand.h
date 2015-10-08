/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

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
    medPaintCommand(medPaintCommandOptions *options, QUndoCommand *parent = 0);
    virtual ~medPaintCommand();

    virtual void undo();
    virtual void redo();

    void paint();

private:
    medPaintCommandPrivate *d;
};
