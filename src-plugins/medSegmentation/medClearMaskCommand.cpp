/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medClearMaskCommand.h>

#include <itkImageDuplicator.h>


medClearMaskCommand::medClearMaskCommand(medAbstractView* view, QUndoCommand *parent): QUndoCommand(parent)
{
    m_view = view;
    m_mask = 0;
    m_previousMask = MaskType::New();
}

void medClearMaskCommand::setMaskToClear(MaskType::Pointer mask)
{
    m_mask = mask;
}

void medClearMaskCommand::undo()
{
    if(m_mask)
    {
        m_mask->Graft( m_previousMask );
        m_mask->Modified();
        m_mask->GetPixelContainer()->Modified();
        m_mask->SetPipelineMTime(m_mask->GetMTime());
        m_view->render();
    }
}

void medClearMaskCommand::redo()
{
    typedef itk::ImageDuplicator< MaskType > DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();
    m_mask->Print(std::cout);
    duplicator->SetInputImage(m_mask);
    duplicator->Update();
    m_previousMask = duplicator->GetOutput();

    if ( m_mask )
    {
        m_mask->FillBuffer(0);
        m_mask->Modified();
        m_mask->GetPixelContainer()->Modified();
        m_mask->SetPipelineMTime(m_mask->GetMTime());

        m_view->render();
    }
}
