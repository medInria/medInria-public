#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkImage.h>

#include <medAbstractRoi.h>
#include <medAlgorithmPaintPluginExport.h>

namespace med
{

class medPaintBrushPrivate;

typedef itk::Image <unsigned char, 2> Mask2dType;

class MEDALGORITMPAINT_EXPORT medPaintBrush : public medAbstractRoi
{
    Q_OBJECT

public:
    medPaintBrush(Mask2dType::Pointer slice, int id, bool isMaster, int label, medAbstractRoi* parent = nullptr);

    virtual ~medPaintBrush();

    void Off() override;

    void On() override;

    bool isVisible() override;

    void forceInvisibilityOn() override;

    void forceInvisibilityOff() override;

    QString info() override;

    QString type() override;

    void computeRoiStatistics() override;

    bool canRedo() override;

    bool canUndo() override;

    void undo() override;

    void redo() override;

    void saveState() override;

    bool copyROI(medAbstractView *view) override;

    medAbstractRoi *getCopy(medAbstractView *view) override;

    QList<medAbstractRoi *> *interpolate(medAbstractRoi *roi) override;

    Mask2dType::Pointer getSlice();

    int getLabel();

private:
    medPaintBrushPrivate* d;
    void setRightColor() override;
};
}
