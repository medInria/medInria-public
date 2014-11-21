/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QUndoCommand>
#include <QtCore>

#include <itkImage.h>

class medMagicWandCommandPrivate;

class medClickAndMoveEventFilter;
class medAbstractImageView;
class medAbstractData;


class medMagicWandCommand : public QUndoCommand
{
public:

    typedef itk::Image<unsigned char, 3> MaskType;

    medMagicWandCommand( medAbstractImageView * view, medAbstractData* imageData, QVector3D &vec, double radius,
                         MaskType::Pointer itkMask, unsigned int maskValue, bool run3D, /*medPaintMode mode = PAINT,*/ QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    void updateWandRegion(unsigned int maskValue);

    template <typename IMAGE> void RunConnectedFilter (MaskType::IndexType &index, unsigned int planeIndex, unsigned int maskValue);

private:
    medMagicWandCommandPrivate *d;
};
