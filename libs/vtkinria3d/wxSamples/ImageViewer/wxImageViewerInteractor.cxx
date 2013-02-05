/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxImageViewerInteractor.cxx 1436 2010-01-22 18:27:59Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2010-01-22 19:27:59 +0100 (Fri, 22 Jan 2010) $
Version:   $Revision: 1436 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxImageViewerInteractor.h"

#include "wxVtkImageFlip.h"
#include "wxVtkPageView.h"

#include <itkMatrix.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <itksys/SystemTools.hxx>
#include <vtksys/SystemTools.hxx>

#include <itkAnalyzeImageIOFactory.h>
#include <itkImageToVTKImageFilter.h>
#include <itkVTKImageToImageFilter.h>
#include <itkGDCMImageIOFactory.h>
#include <itkCastImageFilter.h>
#include <itkExtractImageFilter.h>

#include <vtkRenderWindow.h>
#include <vtkViewImage2D.h>
#include <vtkViewImage3D.h>
#include <vtkWindowToImageFilter.h>
#include <vtkJPEGWriter.h>

#include <vtkLookupTableManager.h>

#include "wxImageThumbnail.h"
#include <wxVTKRenderWindowInteractor.h>

#include <wxVtkDICOMImporter.h>
#include <wxSnapshotTaker.h>
#include <wxCommon.h>

#include <wxVtkDICOMExporter.h>

#include <wx/cmdline.h>
// #include "GetPot.h"
//#include <wx/cmdline.h>

#include <wx/progdlg.h>

#include <vtkMetaImageData.h>
#include <vtkMetaDataSetSequence.h>



wxImageViewerInteractor::wxImageViewerInteractor(wxWindow* parent, int id,
                                                    const wxPoint& pos, const wxSize& size,
                                                    long styl, const wxString& title)
  : wxImageViewer( parent, id, pos, size, styl, title )
{
  m_Thumbnail = new wxImageThumbnail ( this->m_MainNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, styl, wxT ("Preview") );
  m_MainNotebook->AddPage ( m_Thumbnail, wxT ("Preview"), true );
  m_InteractionStyle = vtkViewImage2D::SELECT_INTERACTION;	
}



int wxImageViewerInteractor::SetCmdLine (int argc, char** argv)
{

  wxCmdLineParser* parser = new wxCmdLineParser (argc, argv);

  parser->AddSwitch (wxT("h"),   wxT ("help"),                      wxT ("Print this help message"));
  parser->AddOption (wxT("i"),   wxT ("input"),                     wxT ("input image file"));
  parser->AddOption (wxT("dc"),   wxT ("dicom_directory"),          wxT ("input DICOM root directory") );
  parser->AddOption (wxT("dca"),   wxT ("dicom_auto_mode"),         wxT ("Auto organization mode (0: no re-organization; 1: default re-organization; 2: position based)"));
  parser->AddOption (wxT("s"),   wxT ("save"),                      wxT ("save image file"));
  parser->AddOption (wxT("dco"), wxT ("save_all_directory"),        wxT ("save all images in directory"));
  
  int ok = parser->Parse(true);

  if( ok != 0 )
  {

    m_MainNotebook->Show(false);
    for( int i=1; i<argc; i++)
    {
      this->OpenImage (argv[i]);
    }
    
    m_MainNotebook->SetSelection(0);
    m_MainNotebook->Show(true);
    
    delete parser;
    return 0;
  }
  
  if( parser->Found (wxT ("h")) || parser->Found (wxT ("help")) )
  {
    parser->Usage();
    delete parser;
    return -1;
  }
  

  wxString  inputimage;
  wxString  dicomdirectory;
  long int dicomautomode;
  wxString savefilename;
  bool saveallasked;
  wxString savealldirectory;


  if( parser->Found (wxT ("i"), &inputimage) || parser->Found (wxT ("input"), &inputimage) )
  {
    this->OpenImage (inputimage.char_str());
  }
  else if (parser->Found (wxT ("dc"), &dicomdirectory) || parser->Found (wxT ("dicom_directory"), &dicomdirectory) )
  {
    saveallasked = ( parser->Found (wxT ("dco"), &savealldirectory) || parser->Found (wxT ("save_all_directory"), &savealldirectory) );
    parser->Found (wxT ("dca"), &dicomautomode) || parser->Found (wxT ("dicom_auto_mode"), &dicomautomode);
    
    itk::GDCMImporter2<ImageType>::Pointer myImporter = itk::GDCMImporter2<ImageType>::New();
    myImporter->RecursiveScanOn();
    myImporter->SetInputDirectory (dicomdirectory.char_str());
    
    try
    {
      myImporter->Scan();
      
      if ( dicomautomode == 1 )
	myImporter->AutoOrganization();
      if ( dicomautomode == 2 )
	myImporter->AutoOrganizationPositionBased();

      myImporter->Update();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << e;
      return -1;
    }

    if (saveallasked)
    {
      myImporter->SaveOutputsInDirectory (savealldirectory.char_str());
      return 0;
    }
    
    for (unsigned int i=0; i<myImporter->GetNumberOfOutputs(); i++)
    {
      itk::GDCMImporter2<ImageType>::DICOMImageType::Pointer image =
	static_cast< itk::GDCMImporter2<ImageType>::DICOMImageType*>(myImporter->GetOutput (i));
      if (!image.IsNull())
	this->AddITKImage ( static_cast<ImageType*>(image), image->GetName());
    }
    
  }

  if (parser->Found (wxT ("s"), &savefilename) || parser->Found (wxT ("save"), &savefilename) )
  {
    wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(1));
    if( !page )
      return -1;
    this->SaveImage (page, savefilename.char_str());

    return 0;
  }
  
  return 0;
  
}



void wxImageViewerInteractor::OnToolBarOpenVol(wxCommandEvent& event)
{
  
  wxArrayString filenames = this->GetOpenFileNames(this, wxT("Choose an image"),
                                                   wxSupportedImageExtensionsFilter);
  if( filenames.IsEmpty() )
  {
    return;
  }


  m_MainNotebook->Show(false);


  // create a progress bar
  wxProgressDialog* dialog = new wxProgressDialog (wxT ("Opening images, please wait..."),
                                                   wxT (""), 100, this,
                                                   wxPD_APP_MODAL|wxPD_AUTO_HIDE|wxPD_SMOOTH|wxPD_CAN_ABORT);

  bool keepGoing = dialog->Update (0);
  
  unsigned int Nimages = filenames.Count();
  double step = 100.0/(double)(Nimages);
  
  for( unsigned int i=0; i<Nimages && keepGoing; i++ )
  {
    this->OpenImage (filenames[i].char_str());
    keepGoing = dialog->Update ( (int)(step*(double)i) );
  }
  
  m_MainNotebook->SetSelection(m_MainNotebook->GetPageCount()-1);
  m_MainNotebook->Show(true);

  dialog->Destroy();
    
}

  
void wxImageViewerInteractor::OpenImage(const char* filename)
{

  if (vtkMetaImageData::CanReadFile(filename))
  {
    vtkMetaImageData* metaimage = vtkMetaImageData::New();
    
    try
    {
      metaimage->Read (filename);
    }
    catch (vtkErrorCode::ErrorIds)
    {
      std::cerr<<"cannot read file "<<filename<<std::endl;
      this->DisplayErrorMessage (this, wxT("Error while reading file.\n(check the log window for more details)"));
      metaimage->Delete();
      return;
    }

    this->AddImage(metaimage->GetImageData(), metaimage->GetOrientationMatrix(), vtksys::SystemTools::GetFilenameName (filename).c_str());
    metaimage->Delete();
    
  }
  else if (vtkMetaDataSetSequence::CanReadFile (filename))
  {
    
  
    vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::New();
    
    try
    {
      sequence->Read (filename);
    }
    catch (vtkErrorCode::ErrorIds)
    {
      std::cerr<<"cannot read file "<<filename<<std::endl;
      this->DisplayErrorMessage (this, wxT("Error while reading file.\n(check the log window for more details)"));
      sequence->Delete();
      return;
    }

    for (int i=0; i<sequence->GetNumberOfMetaDataSets(); i++)
    {
      vtkMetaImageData* metaimage = vtkMetaImageData::SafeDownCast (sequence->GetMetaDataSet (i));
      if (!metaimage)
	continue;
      
      this->AddImage(metaimage->GetImageData(), metaimage->GetOrientationMatrix(), vtksys::SystemTools::GetFilenameName (filename).c_str());
    }
    sequence->Delete();
  }
  else
  {
    std::cerr<<"this file is not an image !"<<std::endl;
  }
  
    
//   /*
//     As their might be more than one volume in an image, we read 4D
//     images, and then split them into 3D volumes if the 4th dimension
//     is greater than 1.
//   */
//   itk::ImageFileReader<Image4DType>::Pointer reader 
//     = itk::ImageFileReader<Image4DType>::New();
//   reader->SetFileName (filename);
  
//   try
//   {
//     std::cout << "Reading: " << filename << std::endl;
//     reader->Update();
//   }
//   catch (itk::ExceptionObject & e)
//   {
//     std::cerr << e;
//     return;
//   }  
  
//   // here we look for the 4th dimension:
//   Image4DType::Pointer image = reader->GetOutput();
//   Image4DType::SizeType size = image->GetLargestPossibleRegion().GetSize();
//   unsigned int nVolumes = size[3];
      
//   // split the 4D volume into 3D volumes
//   Image4DType::RegionType regionToExtract = image->GetLargestPossibleRegion();
//   Image4DType::IndexType index;
//   index[0] = 0;
//   index[1] = 0;
//   index[2] = 0;
//   index[3] = 0;
//   size[3] = 0;
//   regionToExtract.SetSize (size);
//   regionToExtract.SetIndex (index);
    
//   if( nVolumes==0 )
//   {
//     //throw itk::ExceptionObject (__FILE__,__LINE__,"Error: 4th dimension is zero, and should be at least one.");
//     this->DisplayErrorMessage (this, wxT ("Error: No image present in this file") );
//     return;
//   }
    
    
//   for( unsigned int n=0; n<nVolumes;  n++)
//   {
//     typedef itk::ExtractImageFilter<Image4DType, ImageType> ExtractImageType;
//     regionToExtract.SetIndex (3,n);
//     ExtractImageType::Pointer myExtractor = ExtractImageType::New();
//     myExtractor->SetExtractionRegion (regionToExtract);
//     myExtractor->SetInput (image);
      
//     try
//     {
//       myExtractor->Update();
//     }
//     catch (itk::ExceptionObject &e)
//     {
//       std::cerr << e;
//       throw itk::ExceptionObject (__FILE__,__LINE__,"Error while extracting a 3D volume from a 4D image.");
//     }

    
//     wxString name = wxT(filename);  
//     wxString basename = this->GetNameFromFile( name );

//     /*
//     ImageType::SpacingType spacing;
//     spacing[0] = 0.001;
//     spacing[1] = 0.001;
//     spacing[2] = 0.001;
//     myExtractor->GetOutput()->SetSpacing ( spacing );
//     */

//     this->AddITKImage ( myExtractor->GetOutput(), basename.c_str() );

//   }
    
}

  
wxVtkPageView* wxImageViewerInteractor::CreateNewVtkPage()
{
  wxVtkPageView* view = new wxVtkPageView(m_MainNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize/*m_MainNotebook->GetPageBestSize()*/);
  view->SetLayoutMode (wxVtkPageView::LAYOUT_NORMAL);
  view->SetViewMode (wxVtkPageView::VIEW_MODE_TRACING);
  
  
  return view;
}


void wxImageViewerInteractor::OnPageChanged(wxAuiNotebookEvent& event)
{
}


void wxImageViewerInteractor::OnDeletePage(wxAuiNotebookEvent& event)
{
  if( m_MainNotebook->GetSelection()==0 )
  {
      event.Veto();
      return;
  }

  /*
  int res = wxMessageBox(wxT("Are you sure you want to close this tab?"),
                         wxT("wxImageViewer"),
                         wxYES_NO,
                         this);
  if (res != wxYES)
  {
      event.Veto();
      return;
  }
  else
  {  
    m_Thumbnail->RemoveImage ( event.GetSelection()-1 );
  }
  */
  m_Thumbnail->RemoveImage ( event.GetSelection()-1 );
}


void wxImageViewerInteractor::OnSelectInteractionEvent(wxCommandEvent& event)
{
  m_InteractionStyle = vtkViewImage2D::SELECT_INTERACTION;
  int npages = m_MainNotebook->GetPageCount();
  for( int i=0; i<npages; i++)
  {
    wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(i));
    if( page )
    {
      page->SetInteractionStyleToSelector();
    }
  }
  m_Thumbnail->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
}

void wxImageViewerInteractor::OnWindowLevelInteractionEvent(wxCommandEvent& event)
{
  m_InteractionStyle = vtkViewImage2D::WINDOW_LEVEL_INTERACTION;
  int npages = m_MainNotebook->GetPageCount();
  for( int i=0; i<npages; i++)
  {
    wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(i));
    if( page )
    {
      page->SetInteractionStyleToWindowLevel();
    }
  }
  m_Thumbnail->SetInteractionStyle (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
}

void wxImageViewerInteractor::OnZoomInteractionEvent(wxCommandEvent& event)
{
  m_InteractionStyle = vtkViewImage2D::ZOOM_INTERACTION;
  int npages =m_MainNotebook->GetPageCount();
  for( int i=0; i<npages; i++)
  {
    wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(i));
    if( page )
    {
      page->SetInteractionStyleToZoom();
    }
  }
  m_Thumbnail->SetInteractionStyle (vtkViewImage2D::ZOOM_INTERACTION);
}

void wxImageViewerInteractor::OnFullPageInteractionEvent(wxCommandEvent& event)
{
  int npages = m_MainNotebook->GetPageCount();
  for( int i=0; i<npages; i++)
  {
    wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(i));
    if( page )
    {
      page->SetInteractionStyleToFullPage();
    }
  }
}

void wxImageViewerInteractor::On3DVolumeRendering(wxCommandEvent& event)
{
  wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(m_MainNotebook->GetSelection()));
  if( !page )
    return;
  vtkViewImage3D* view = page->GetView4();
  if (!view)
    return;

  bool isVR = (view->GetRenderingMode() == vtkViewImage3D::VOLUME_RENDERING);
  if (isVR)
    page->SetRenderingModeToMPR();
  else
    page->SetRenderingModeToVR();
  
  view->Render();
}



void wxImageViewerInteractor::AddPreviewThumbnail ( wxVtkPageView* pageView, const char* filename)
{
  if( !pageView )
  {
    return;
  }

  m_Thumbnail->AddImage ( pageView->GetInput(), pageView->GetOrientationMatrix(), filename);
  m_Thumbnail->Render();

}



void wxImageViewerInteractor::AddImage (vtkImageData* image, vtkMatrix4x4* orientationmatrix, const char* filename)
{
  
  wxVtkPageView* newPage = this->CreateNewVtkPage();  
  
  newPage->SetImageName (filename);
    
  wxString number;
  number.Printf (wxT("%d-"), (int)m_MainNotebook->GetPageCount());
  wxString basename = number+wxString (filename, wxConvUTF8);
  
  m_MainNotebook->AddPage( newPage, basename, true);

  newPage->SetInput( image, true, orientationmatrix );
  newPage->SetInteractionStyle ( m_InteractionStyle );    
  
#ifndef __WXMAC__
  newPage->GetView4()->SetCubeVisibility (true);
#endif

  this->AddPreviewThumbnail (newPage, basename.char_str());
  
}



void wxImageViewerInteractor::AddITKImage (ImageType::Pointer image, const char* filename)
{
  
  wxVtkPageView* newPage = this->CreateNewVtkPage();  
  
  newPage->SetImageName (filename);
    
  wxString number;
  number.Printf (wxT("%d-"), (int)m_MainNotebook->GetPageCount());
  wxString basename = number+wxString (filename, wxConvUTF8);
  
  m_MainNotebook->AddPage( newPage, basename, true);

  vtkMetaImageData* metaimage = vtkMetaImageData::New();
  metaimage->SetItkImage<ImageType::PixelType>(image);
  newPage->SetInput( metaimage->GetImageData(), true, metaimage->GetOrientationMatrix());
  metaimage->Delete();
  
//   newPage->SetITKInput( image );
  newPage->SetInteractionStyle ( m_InteractionStyle );    
  
#ifndef __WXMAC__
  newPage->GetView4()->SetCubeVisibility (true);
#endif

  this->AddPreviewThumbnail (newPage, basename.char_str());
  
}



void wxImageViewerInteractor::OnToolBarOpenDICOM(wxCommandEvent& event)
{
  
  wxVtkDICOMImporter* myimporter = new wxVtkDICOMImporter (this);
  
  //myimporter->ShowModal();
  
  if( myimporter->GetReturnCode()!=wxID_OK )
  {
    myimporter->Destroy();
    return;
  }
  
  //m_MainNotebook->Show(false);


  // create a progress bar
  wxProgressDialog* dialog = new wxProgressDialog (wxT ("Please wait"),
                                                   wxT ("Loading volumes..."), 100, this,
                                                   wxPD_APP_MODAL|wxPD_AUTO_HIDE|wxPD_SMOOTH|wxPD_CAN_ABORT);

  bool keepGoing = dialog->Update (0);

  unsigned int Nimages = myimporter->GetOutputs().size();

  double step = 100.0/(double)(Nimages);
  
  for (unsigned int i=0; i<Nimages && keepGoing; i++)
  {
    
    if (myimporter->GetOutput(i).IsNull())
    {
      continue;
    }

    vtkMetaImageData* metaimage = vtkMetaImageData::New();
    metaimage->SetItkImage<wxVtkDICOMImporter::ImageComponentType>(myimporter->GetOutput(i));

    this->AddImage(metaimage->GetImageData(), metaimage->GetOrientationMatrix(), myimporter->GetDescription (i).c_str());

    metaimage->Delete();
    
    

//     // no need to cast anymore as the types are consistent on with the other
//     // yes we need !
//     typedef itk::CastImageFilter<wxVtkDICOMImporter::ImageType, ImageType> CastFilterType;
    
//     CastFilterType::Pointer myCast = CastFilterType::New();
//     myCast->SetInput (myimporter->GetOutput(i));
    
    
//     wxString basename = wxT (myimporter->GetDescription (i).c_str());
//     //wxString basename = wxT ("1");
    
//     this->AddITKImage ( myCast->GetOutput(), basename.c_str());
//     //     this->AddITKImage ( myimporter->GetOutput(i), basename.c_str());
    
    keepGoing = dialog->Update ( (int)(step*(double)i) );
    
  }
  
  m_MainNotebook->SetSelection(0);
  m_MainNotebook->Show(true);

  myimporter->Destroy();
  dialog->Destroy();
  
}


void wxImageViewerInteractor::OnToolBarSave(wxCommandEvent& event)
{
  wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(m_MainNotebook->GetSelection()));
  if( !page )
    return;

  wxString filename = this->GetSaveFileName(this, wxT("Choose an image"),
                                           wxSupportedImageExtensionsFilter);
  if( filename.IsEmpty() )
  {
    return;
  }

  this->SaveImage (page, filename.char_str());
}


void wxImageViewerInteractor::OnToolBarSaveAll(wxCommandEvent& event)
{

  wxDirDialog* myDirDialog = new wxDirDialog(this, wxT ("Choose the DICOM directory"),
                                             wxT(""),
                                             wxDD_DIR_MUST_EXIST|wxDD_DEFAULT_STYLE|wxDD_CHANGE_DIR, wxDefaultPosition);

  int OK = myDirDialog->ShowModal();
  if( OK!=wxID_OK )
  {
    delete myDirDialog;
    return;
  }
  
  wxString directory = myDirDialog->GetPath();
  std::string dir (directory.char_str());


#ifdef __WXMSW__
  dir = dir+"\\";
#else
  dir = dir+"/";
#endif

  for (unsigned int id = 0; id < m_MainNotebook->GetPageCount(); id++)
  {

    wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(id));
    if (!page)
    {
      continue;
    }
    
    
    std::string name = page->GetImageName();
    
    //bool validname=false;

    std::string::iterator last = name.end();
#ifndef __WXMSW__
    last = std::remove (name.begin(), last, ' ');
    name.resize ( (size_t)(last-name.begin()));
#endif
    std::replace (name.begin(), last, '/', '_');
    std::replace (name.begin(), last, '\\', '_');
    std::replace (name.begin(), last, ':', '_');

   
    if (name.length() == 0)
    {
      name = "NotAValidFileName";
    }

    std::string basename = dir+name;

    std::string extension = vtksys::SystemTools::GetFilenameLastExtension(basename);
    if (vtkMetaImageData::IsImageExtension(extension.c_str()) )
      basename += ".hdr";      
    
//     if (!ext.size())
//       basename += ".hdr";


    
    std::string filename = basename + ".hdr";
        
    unsigned int iter = 1;

    while (itksys::SystemTools::FileExists (filename.c_str()) )
    {
      // file exists
      char num[256];
      sprintf (num, "(%d)", iter);
      
      filename = basename + num + ".hdr";

      iter++;
    }
    
    
    this->SaveImage (page, filename.c_str());
  }

}



void wxImageViewerInteractor::OnToolBarSaveDicom(wxCommandEvent& event)
{
  wxVtkDICOMExporter* exporter = new wxVtkDICOMExporter (this, -1,
							 wxT("DICOM exporter"),
							 wxDefaultPosition,
							 wxSize(400,400));
  

  unsigned int number = m_MainNotebook->GetPageCount();

    
  for (unsigned int i=0; i<number; i++)
  {
    wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage( i ));
    if( !page )
      continue;

    vtkMetaImageData* imagedata = vtkMetaImageData::New();
    imagedata->SetDataSet (page->GetInput());

    wxVtkDICOMExporter::ImageType::Pointer itkimage;
    
    bool isvalid = imagedata->GetItkImage<wxVtkDICOMExporter::ImageComponentType>(itkimage);
    if (!isvalid)
    {
      std::cerr<<"cannot access itk image from page "<<page->GetImageName()<<std::endl;
      continue;
    }
    
    exporter->AddVolume(page->GetImageName(), itkimage);
    imagedata->Delete();
    
  }

  exporter->ShowModal();

  exporter->Destroy();
    
}



void wxImageViewerInteractor::OnToolBarFlip(wxCommandEvent& event)
{
  
  wxVtkImageFlip* myflipper = new wxVtkImageFlip (this, -1,
						  wxT("Radiologic conventions"),
						  wxDefaultPosition,
						  wxSize(400,400));

  int selection = m_MainNotebook->GetSelection();
  wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage( selection ));
  if( !page )
  {
    return;
  }
  
  myflipper->SetImage (page->GetInput(), page->GetView1()->GetDirectionMatrix() );
  int OK = myflipper->ShowModal();
  
  if (OK == wxID_OK )
  {
    page->SetInput(myflipper->GetOutput(), false, page->GetOrientationMatrix()); // do not reset window/level/position
    page->GetView1()->SyncReset();
    page->Render();
    m_Thumbnail->ChangeImage (selection-1, myflipper->GetOutput());
  }
  
  myflipper->Destroy();
	
}

void wxImageViewerInteractor::SaveImage (wxVtkPageView* page, const char* filename)
{

  if (!page)
  {
    return;
  }

  vtkMetaImageData* metaimage = vtkMetaImageData::New();

  metaimage->SetDataSet(page->GetInput());
  metaimage->SetOrientationMatrix (page->GetOrientationMatrix());

  try
  {
    metaimage->Write (filename);
  }
  catch (vtkErrorCode::ErrorIds)
  {
    std::cerr<<"cannot write file "<<filename<<std::endl;
    this->DisplayErrorMessage (this, wxT("Error while writing file.\n(check the log window for more details)"));
  }
  
  metaimage->Delete();
  
  // write the image
  
//   itk::VTKImageToImageFilter<ImageType>::Pointer myConverter =
//     itk::VTKImageToImageFilter<ImageType>::New();
  
  
//   vtkImageData* image = vtkImageData::New();
//   image->DeepCopy(page->GetInput() );
  
//   try
//   {
//     myConverter->SetInput( image );
//     myConverter->Update();
//   }
//   catch (itk::ExceptionObject & e)
//   {
//     std::cerr << e;
//     this->DisplayErrorMessage (this, wxT("Error while converting image.\n(check the log window for more details)"));
//     return;
//   }
  
//   itk::Matrix<double,3,3> cosines;
//   cosines[0][0]= 1;
//   cosines[0][1]= 0;
//   cosines[0][2]= 0;
//   cosines[1][0]= 0;
//   cosines[1][1]=-1;
//   cosines[1][2]= 0;
//   cosines[2][0]= 0;
//   cosines[2][1]= 0;
//   cosines[2][2]= 1;
  
//   ImageType::Pointer itkimage = const_cast<ImageType*>(myConverter->GetOutput());
//   itkimage->SetDirection(cosines);
  
//   itk::ImageFileWriter<ImageType>::Pointer writer
//     = itk::ImageFileWriter<ImageType>::New();
//   writer->SetFileName (filename);
//   writer->SetInput(itkimage);
  
//   try
//   {
//     std::cout << "writing : " << filename << std::endl;
//     writer->Update();
//   }
//   catch (itk::ExceptionObject & e)
//   {
//     std::cerr << e;
//     this->DisplayErrorMessage (this, wxT("Error while writing file.\n(check the log window for more details)"));
//     return;
//   }  
//   image->Delete();
}


void wxImageViewerInteractor::OnFullScreenAxial(wxCommandEvent &event)
{
  bool show = event.IsChecked();

  wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(m_MainNotebook->GetSelection()));

  if( show )
  {
    m_ToolBarView->ToggleTool (TOOLBAR_COR, false);
    m_ToolBarView->ToggleTool (TOOLBAR_SAG, false);
    m_ToolBarView->ToggleTool (TOOLBAR_3D, false);

    if (page)
      page->OnFullScreen(vtkViewImage::AXIAL_ID);
    
  }
  else
  {
    if (page)
      page->NoFullScreen();
  }

  if( page ) {
    page->GetView1()->Render();
    page->Render();
  }

  m_Thumbnail->ChangeOrientation (vtkViewImage::AXIAL_ID);

}
void wxImageViewerInteractor::OnFullScreenCoronal(wxCommandEvent &event)
{
  bool show = event.IsChecked();

  wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(m_MainNotebook->GetSelection()));
  
  if( show )
  {
    m_ToolBarView->ToggleTool (TOOLBAR_AX, false);
    m_ToolBarView->ToggleTool (TOOLBAR_SAG, false);
    m_ToolBarView->ToggleTool (TOOLBAR_3D, false);

    if (page)
      page->OnFullScreen(vtkViewImage::CORONAL_ID);
    
  }
  else
  {
    if (page)
      page->NoFullScreen();
  }

  m_Thumbnail->ChangeOrientation (vtkViewImage::CORONAL_ID);
  
}
void wxImageViewerInteractor::OnFullScreenSagittal(wxCommandEvent &event)
{
  bool show = event.IsChecked();

  wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(m_MainNotebook->GetSelection()));
  
  if( show )
  {
    m_ToolBarView->ToggleTool (TOOLBAR_AX, false);
    m_ToolBarView->ToggleTool (TOOLBAR_COR, false);
    m_ToolBarView->ToggleTool (TOOLBAR_3D, false);

    if (page)
      page->OnFullScreen(vtkViewImage::SAGITTAL_ID);
    
  }
  else
  {
    if (page)
      page->NoFullScreen();
  }

  m_Thumbnail->ChangeOrientation (vtkViewImage::SAGITTAL_ID);

}
void wxImageViewerInteractor::OnFullScreen3D(wxCommandEvent &event)
{

  bool show = event.IsChecked();

  wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(m_MainNotebook->GetSelection()));
  
  if( show )
  {
    m_ToolBarView->ToggleTool (TOOLBAR_AX, false);
    m_ToolBarView->ToggleTool (TOOLBAR_COR, false);
    m_ToolBarView->ToggleTool (TOOLBAR_SAG, false);

    if (page)
      page->OnFullScreen(vtkViewImage::NB_PLAN_IDS);
    
  }
  else
  {
    if (page)
      page->NoFullScreen();
  }

}


void wxImageViewerInteractor::OnToolBarLink(wxCommandEvent &event)
{
  m_Thumbnail->LinkViews (event.IsChecked());
  if (event.IsChecked())
    m_Thumbnail->Render();
}


void wxImageViewerInteractor::OnToolBarSnapShot(wxCommandEvent& event)
{

  wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(m_MainNotebook->GetSelection()));

  if (!page)
  {
    this->DisplayWarningMessage( this, wxT("No snap shot in preview."));
    return;
  }
  
  // find out which window is full screen
  wxVTKRenderWindowInteractor* viewToSnap = 0;

  viewToSnap = page->GetDisplayedView();

  if( !viewToSnap )
  {
    this->DisplayWarningMessage( this, wxT("Please set a window in full page first."));
    return;
  }


  wxSnapshotTaker* sstaker = new wxSnapshotTaker (this);
  sstaker->SetRenderWindow (viewToSnap->GetRenderWindow());
  sstaker->SetMagnification(2);
  sstaker->Snap();

  delete sstaker;
}



void wxImageViewerInteractor::OnToolBarCLUT (wxCommandEvent& event)
{

  int val = event.GetInt();
  wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(m_MainNotebook->GetSelection()));

  vtkLookupTable* lut = vtkLookupTableManager::GetLookupTable(val-1);
  
  if( lut && page )
  {
    page->SetLookupTable (lut);
    page->Render();
    lut->Delete();
    return;
  }

  wxImageThumbnail* thumbPage = dynamic_cast<wxImageThumbnail*>(m_MainNotebook->GetPage(m_MainNotebook->GetSelection()));

  if( lut && thumbPage )
  {
    thumbPage->SetLookupTable (lut);
    thumbPage->Render();
    lut->Delete();
    return;
  } 
}


void wxImageViewerInteractor::OnToolBarPreset (wxCommandEvent& event)
{

  int val = event.GetInt();
  wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(m_MainNotebook->GetSelection()));

  if( !page )
    return;
  
  if ( val==1 ) // VR muscles and bones
  {
    vtkLookupTable* lut = vtkLookupTableManager::GetVRMusclesBonesLookupTable();
    page->SetLookupTable (lut);
    page->GetView1()->SyncSetWindow ( 337.0 );
    page->GetView1()->SyncSetLevel ( 1237.0 );
    page->SetRenderingModeToVR();
    page->GetView4()->ShadeOn();
    page->Render();
    lut->Delete();
  }
  
}


void wxImageViewerInteractor::OnToolBarVRMode (wxCommandEvent& event)
{
  int val = event.GetInt();

  wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(m_MainNotebook->GetSelection()));

  if( !page )
  {
    return;
  }
  
  switch(val)
  {
      case 1:
        page->GetView4()->SetVolumeRayCastFunctionToComposite();
        break;
        
      case 2:
        page->GetView4()->SetVolumeRayCastFunctionToMIP();
        break;

      default:
        return;
  }

  page->GetView4()->Render();
  
}

void wxImageViewerInteractor::OnToolBarShading (wxCommandEvent& event)
{
  bool val = event.IsChecked();

  wxVtkPageView* page = dynamic_cast<wxVtkPageView*>(m_MainNotebook->GetPage(m_MainNotebook->GetSelection()));

  if( !page )
  {
    return;
  }

  if( val )
  {
    page->GetView4()->ShadeOn();
  }
  else
  {
    page->GetView4()->ShadeOff();
  }

  page->GetView4()->Render();
  
  
}

void wxImageViewerInteractor::OnToolBarAxis (wxCommandEvent& event)
{
  bool val = event.IsChecked();

  wxImageThumbnail* thumbPage = dynamic_cast<wxImageThumbnail*>(m_MainNotebook->GetPage(m_MainNotebook->GetSelection()));

  if( thumbPage )
  {
    thumbPage->SetShow2DAxis (val);
    thumbPage->Render();
  } 
}


