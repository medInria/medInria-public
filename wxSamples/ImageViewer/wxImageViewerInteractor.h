/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxImageViewerInteractor.h 1197 2009-07-20 16:35:54Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-07-20 18:35:54 +0200 (Mon, 20 Jul 2009) $
Version:   $Revision: 1197 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wx_ImageViewerInteractor_h_
#define _wx_ImageViewerInteractor_h_

#include "vtkINRIA3DConfigure.h"

#include "wxImageViewer.h"

#include <itkImage.h>
#include <wxVtkPageView.h>


class wxImageThumbnail;
class vtkImageData;
class vtkMatrix4x4;


class IMAGEVIEWER_EXPORT wxImageViewerInteractor : public wxImageViewer
{
  
 public:
  wxImageViewerInteractor(wxWindow* parent, int id=-1,
                          const wxPoint& pos=wxDefaultPosition,
                          const wxSize& size=wxDefaultSize,
                          long style=wxDEFAULT_FRAME_STYLE,
                          const wxString& title=wxT (""));
  virtual ~wxImageViewerInteractor(){};


  typedef wxVtkPageView::ImageComponentType ImageComponentType;
  typedef wxVtkPageView::ImageType          ImageType;
  typedef itk::Image<ImageComponentType, 4>  Image4DType;
  
  virtual void OnToolBarOpenVol              (wxCommandEvent &);
  virtual void OnToolBarOpenDICOM            (wxCommandEvent &);
  virtual void OnToolBarSave                 (wxCommandEvent &);
  virtual void OnToolBarSaveAll              (wxCommandEvent &);
  virtual void OnToolBarSaveDicom            (wxCommandEvent &);
  virtual void OnToolBarSnapShot             (wxCommandEvent &);
  virtual void OnFullScreenAxial             (wxCommandEvent &);
  virtual void OnFullScreenCoronal           (wxCommandEvent &);
  virtual void OnFullScreenSagittal          (wxCommandEvent &);
  virtual void OnFullScreen3D                (wxCommandEvent &);
  virtual void OnToolBarFlip                 (wxCommandEvent &);
  virtual void OnToolBarLink                 (wxCommandEvent &);
  virtual void OnDeletePage                  (wxAuiNotebookEvent&);
  virtual void OnPageChanged                 (wxAuiNotebookEvent&);
  virtual void OnSelectInteractionEvent      (wxCommandEvent &);
  virtual void OnWindowLevelInteractionEvent (wxCommandEvent &);
  virtual void OnZoomInteractionEvent        (wxCommandEvent &);
  virtual void OnToolBarCLUT                 (wxCommandEvent &);
  virtual void OnToolBarPreset               (wxCommandEvent &);
  void         OnToolBarVRMode               (wxCommandEvent &);
  void         OnToolBarShading              (wxCommandEvent &);
  virtual void OnFullPageInteractionEvent    (wxCommandEvent &);
  virtual void On3DVolumeRendering           (wxCommandEvent &);
  virtual void OnToolBarAxis                 (wxCommandEvent &);
  
  virtual void SaveImage (wxVtkPageView* page, const char* filename);
  virtual void OpenImage (const char* filename);
  
  int SetCmdLine (int argc, char** argv);

  /** Get open file name */
  static wxString    GetOpenFileName (wxWindow* parent, const wxString& tip, const wxString& filters)
  {
    wxFileDialog* myFileDialog = new wxFileDialog(parent, tip,
                                                  wxT(""), wxT(""),
                                                  filters,
                                                  wxFD_OPEN|wxFD_CHANGE_DIR, wxDefaultPosition);
    wxString fileName;
    fileName.Empty();
    int OK = myFileDialog->ShowModal();
    
    if( OK==wxID_OK )
      fileName = myFileDialog->GetPath();
    
    myFileDialog->Destroy();
    return fileName;
  }


  /** Get open file names */
  static wxArrayString    GetOpenFileNames (wxWindow* parent, const wxString& tip, const wxString& filters)
  {
    wxFileDialog* myFileDialog = new wxFileDialog(parent, tip,
                                                  wxT(""), wxT(""),
                                                  filters,
                                                  wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_MULTIPLE, wxDefaultPosition);
    wxArrayString fileNames;
    fileNames.Empty();
    int OK = myFileDialog->ShowModal();
    
    if( OK==wxID_OK )
      myFileDialog->GetPaths(fileNames);
    
	myFileDialog->Destroy();
    return fileNames;
  }


  /** Get save file name */
  static wxString    GetSaveFileName (wxWindow* parent, const wxString& tip, const wxString& filters)
  {
    wxFileDialog* myFileDialog = new wxFileDialog(parent, tip,
                                                  wxT(""), wxT(""),
                                                  filters,
                                                  wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
    wxString fileName;
    fileName.Empty();
    int OK = myFileDialog->ShowModal();
    
    if( OK==wxID_OK )
      fileName = myFileDialog->GetPath();
    
	myFileDialog->Destroy();
    return fileName;
  }


  
  /** Display warning message */
  static void        DisplayWarningMessage (wxWindow* parent, const wxString& message)
  {
    wxMessageDialog* myDialog = new wxMessageDialog(parent, message,
                                                    wxT ("ImageViewer warning"),
                                                    wxOK|wxICON_WARNING);
    myDialog->ShowModal();
    myDialog->Destroy();
  }
  
  
  /** Display error message */
  static void        DisplayErrorMessage   (wxWindow* parent, const wxString& message)
  {
    wxMessageDialog* myDialog = new wxMessageDialog(parent, message,
                                                    wxT ("ImageViewer error"),
                                                    wxOK|wxICON_ERROR);
    myDialog->ShowModal();
    myDialog->Destroy();
  }


  static std::string GetNameFromFile (std::string filename)
  {
    // look for the path to the file
#ifdef __WXMSW__
    std::string::size_type slashpos = filename.rfind ("\\"); // last occurence of "/"
#else
    std::string::size_type slashpos = filename.rfind ("/"); // last occurence of "/"
#endif
    std::string name = "";
    if( slashpos != std::string::npos )
      name=filename.substr(slashpos+1, (filename.length() - slashpos));
    else
      name = filename;
    
    return name;
  }


  static wxString GetNameFromFile (wxString filename)
  {
    // look for the path to the file
#ifdef __WXMSW__
    std::string::size_type slashpos = filename.rfind ( wxT("\\") ); // last occurence of "/"
#else
    std::string::size_type slashpos = filename.rfind ( wxT("/") ); // last occurence of "/"
#endif
    wxString name = wxT("");
    if( slashpos != std::string::npos )
      name=filename.substr(slashpos+1, (filename.length() - slashpos));
    else
      name = filename;
    
    return name;
  }

  

 protected:
  
  wxVtkPageView* CreateNewVtkPage(void);
  void AddPreviewThumbnail ( wxVtkPageView*, const char*);
  void AddImage (vtkImageData*, vtkMatrix4x4* orientationmatrix, const char*);
  void AddITKImage (ImageType::Pointer, const char*);
  
 private:
  
  wxImageThumbnail* m_Thumbnail;

  int m_InteractionStyle;
  
};

#endif
