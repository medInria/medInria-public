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

#include <medAbstractView.h>
#include <itkImage.h>

#include <medSegmentationPluginExport.h>


class MEDVIEWSEGMENTATIONPLUGIN_EXPORT medClearMaskCommand : public QUndoCommand
{
public:
    typedef itk::Image<unsigned char, 3> MaskType;

    medClearMaskCommand(medAbstractView* view, QUndoCommand *parent = 0);

    void undo();
    void redo();

    void setMaskToClear(MaskType::Pointer mask);

private:
    MaskType::Pointer m_mask;
    MaskType::Pointer m_previousMask;
    medAbstractView* m_view;
};
