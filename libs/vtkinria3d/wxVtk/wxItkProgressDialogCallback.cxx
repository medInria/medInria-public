/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxItkProgressDialogCallback.cxx 973 2008-10-16 13:22:23Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-10-16 15:22:23 +0200 (Thu, 16 Oct 2008) $
Version:   $Revision: 973 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// For compilers that supports precompilation , includes  wx/wx.h
#include  "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include <wx/progdlg2.h>

#include "wxItkProgressDialogCallback.h"

#include <itkProcessObject.h>


wxItkProgressDialogCallback::wxItkProgressDialogCallback()
{
  m_Dialog=0;
  m_Count=0;
  m_StartingDash=false;
  m_EndingDash=false;
}


void
wxItkProgressDialogCallback::Execute(itk::Object *caller, const itk::EventObject &event)
{
  itk::ProcessObject* po = dynamic_cast<itk::ProcessObject*>(caller);
  if( !po || !m_Dialog ) return;

  if( typeid(event) == typeid ( itk::ProgressEvent)  )
  {

    if( wxItkProgressDialogCallback::GetGraphicalMode() == MODE_GUI )
    {
    
      if( po->GetProgress()==0.0 )
      {
        m_Dialog->Centre (wxCENTER_FRAME | wxBOTH);
        m_Dialog->Show (true);
        m_Dialog->Resume();    
      }
      
      if( !m_Dialog->Update ( (int)(po->GetProgress()*100) ) )
      {
        std::cerr << "Progress: " << po->GetProgress() << std::endl;
        po->AbortGenerateDataOn();
        m_Dialog->Show (false);
	throw itk::ExceptionObject (__FILE__,__LINE__,
				    "Error in ITK process");

      }
      
      if( po->GetProgress() == 1.0 )
      {
        m_Dialog->Show (false);
      }
      
    }
    else
    {

      //std::cout << po->GetProgress() << " ";
      
      if( !m_StartingDash && po->GetProgress()==0.0 )
      {
        std::cout << "|" << std::flush;
        m_StartingDash = true;
        m_EndingDash   = false;
      }
      else if( !m_EndingDash && po->GetProgress() == 1.0 )
      {
        std::cout << "|" << std::endl;
        std::cout << std::flush;
        m_EndingDash   = true;
        m_StartingDash = false;
        m_Count = 0;
      }
      else if( (int)(po->GetProgress()*100)%4==0
               && (int)(po->GetProgress()*25)>m_Count)
      {
        std::cout << "=" << std::flush;
        m_Count++;
      }
      
    }
        
  }
  
}


void
wxItkProgressDialogCallback::Execute(const itk::Object *caller, const itk::EventObject &event)
{
  itk::ProcessObject* po = dynamic_cast<itk::ProcessObject*>( const_cast<itk::Object*>(caller));
  if( !po || !m_Dialog ) return;

  if( typeid(event) == typeid ( itk::ProgressEvent)  )
  {

    if( wxItkProgressDialogCallback::GetGraphicalMode() == MODE_GUI )
    {
      if( po->GetProgress()==0.0 )
      {
        m_Dialog->Show (true);
        m_Dialog->Resume();
      }
      
      if( !m_Dialog->Update ( (int)(po->GetProgress()*100) ) )
      {
        po->AbortGenerateDataOn();
        m_Dialog->Show (false);
      }
      
      if( po->GetProgress() <= 1.0 )
      {
        m_Dialog->Show (false);
      }
    }
    else
    {

      //std::cout << po->GetProgress() << " ";
      
      if( !m_StartingDash && po->GetProgress()==0.0 )
      {
        std::cout << "|" << std::flush;
        m_StartingDash = true;
        m_EndingDash   = false;
      }
      else if( !m_EndingDash && po->GetProgress() == 1.0 )
      {
        std::cout << "|" << std::endl;
        std::cout << std::flush;
        m_EndingDash   = true;
        m_StartingDash = false;
      }
      else if( (int)(po->GetProgress()*100)%4==0
               && (int)(po->GetProgress()*25)>m_Count)
      {
        std::cout << "=" << std::flush;
        m_Count++;
      }
      
    }
    
  }
}
