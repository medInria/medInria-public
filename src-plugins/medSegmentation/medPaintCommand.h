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

class medPaintCommandPrivate;

class medClickAndMoveEventFilter;
class medAbstractImageView;


class medPaintCommand : public QUndoCommand
{
public:
    typedef itk::Image<unsigned char, 3> MaskType;

    enum medPaintMode {PAINT, ERASE};

    medPaintCommand(QVector<QVector3D> points, medAbstractImageView * view, double radius,
                    MaskType::Pointer itkMask, unsigned int maskValue, medPaintMode mode = PAINT, QUndoCommand *parent = 0);

    void undo();
    void redo();

    bool mergeWith(const QUndoCommand *command);
    int id() const { return 1234; }

private:
    void paint(medPaintMode mode);

private:
    medPaintCommandPrivate *d;
};
