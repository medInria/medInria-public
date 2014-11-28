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


class medMagicWandCommandPrivate;

class medClickAndMoveEventFilter;
class medAbstractImageView;
class medAbstractData;


class MEDVIEWSEGMENTATIONPLUGIN_EXPORT medMagicWandCommand : public medAbstractPaintCommand
{
public:
    medMagicWandCommand( medPaintCommandOptions *options,  bool run3D, QUndoCommand *parent = 0);
    virtual ~medMagicWandCommand();

    virtual void undo();
    virtual void redo();

    void paint();

private:
    template <typename IMAGE> void RunConnectedFilter (MaskType::IndexType &index, unsigned int planeIndex, unsigned int maskValue);

private:
    medMagicWandCommandPrivate *d;
};
