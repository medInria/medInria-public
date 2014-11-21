/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medClearMaskCommand.h>

medClearMaskCommand::medClearMaskCommand(QUndoCommand *parent)
{

}

void medClearMaskCommand::undo()
{

}

void medClearMaskCommand::redo()
{
    //    if ( m_maskData && m_itkMask ){
    //        m_itkMask->FillBuffer( medSegmentationSelectorToolBox::MaskPixelValues::Unset );
    //        m_itkMask->Modified();
    //        m_itkMask->GetPixelContainer()->Modified();
    //        m_itkMask->SetPipelineMTime(m_itkMask->GetMTime());

    //        m_maskAnnotationData->invokeModified();
    //    }
}
