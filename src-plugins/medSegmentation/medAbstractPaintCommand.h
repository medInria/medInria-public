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
#include <QVector>
#include <QVector3D>

#include <itkImage.h>

class medAbstractPaintCommandPrivate;

class medClickAndMoveEventFilter;
class medAbstractImageView;
class medAbstractData;


typedef itk::Image<unsigned char, 3> MaskType;


struct medPaintCommandOptions
{
    medAbstractImageView *view;
    medAbstractData* data;
    MaskType::Pointer itkMask;
    QVector<QVector3D> points;
    unsigned int maskValue;
    double radius;
};


class medAbstractPaintCommand : public QUndoCommand
{
public:
    medAbstractPaintCommand(medPaintCommandOptions *options, QUndoCommand *parent = 0);

    virtual void undo();
    virtual void redo();

    medPaintCommandOptions *options() const;

    medAbstractPaintCommand *previousCommand() const;
    void setPreviousCommand(medAbstractPaintCommand *cmd);

protected:
    virtual void paint() {}
    virtual void unpaint() {}

private:
    medAbstractPaintCommandPrivate *d;
};
