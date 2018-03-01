/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
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
#include <itkLabelObject.h>
#include <itkAttributeLabelObject.h>
#include <itkLabelMap.h>
#include <itkLabelImageToLabelMapFilter.h>
#include <itkSmartPointer.h>

#include <medSegmentationPluginExport.h>

class medAbstractPaintCommandPrivate;

class medClickAndMoveEventFilter;
class medAbstractImageView;
class medAbstractData;
class medAnnotationData;
class medAbstractImageData;


typedef itk::Image<unsigned char, 3> MaskType;

typedef itk::AttributeLabelObject<unsigned char, 3, unsigned char> LabelObjectType;
typedef itk::LabelMap< LabelObjectType > LabelMapType;


typedef itk::SmartPointer<itk::Image<unsigned char, 3> >MaskTypePointer;
typedef itk::SmartPointer<itk::LabelMap< LabelObjectType > > LabelMapTypePointer;


class MEDVIEWSEGMENTATIONPLUGIN_EXPORT medPaintCommandManager : public QObject
{
    Q_OBJECT

public:
    static medPaintCommandManager *instance();
    LabelMapType::Pointer labelMap(MaskType* mask);

protected:
    medPaintCommandManager(){}
    ~medPaintCommandManager(){}

protected:
    static medPaintCommandManager *s_instance;
    QHash<MaskType*, LabelMapType::Pointer> labelMaps;
};



struct medPaintCommandOptions
{
    medAbstractImageView *view;
    medAbstractData* data;
    //medAbstractData *maskData;
    medAnnotationData *maskAnnotationData;
    MaskType::Pointer itkMask;
    QVector<QVector3D> points;
    unsigned int maskValue;
    double radius;
};


class MEDVIEWSEGMENTATIONPLUGIN_EXPORT medAbstractPaintCommand : public QUndoCommand
{
public:
    medAbstractPaintCommand(medPaintCommandOptions *options, QUndoCommand *parent = 0);
    virtual ~medAbstractPaintCommand();

    virtual void undo() = 0;
    virtual void redo() = 0;

    medPaintCommandOptions *options() const;

private:
    medAbstractPaintCommandPrivate *d;
};
