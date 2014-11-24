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


class medMagicWandCommand : public medAbstractPaintCommand
{
public:
    medMagicWandCommand( medPaintCommandOptions *options,  bool run3D, QUndoCommand *parent = 0);

protected:
    void paint();
    void unpaint();

private:
    template <typename IMAGE> void RunConnectedFilter (MaskType::IndexType &index, unsigned int planeIndex, unsigned int maskValue);

    void updateWandRegion(unsigned int maskValue);

private:
    medMagicWandCommandPrivate *d;
};
