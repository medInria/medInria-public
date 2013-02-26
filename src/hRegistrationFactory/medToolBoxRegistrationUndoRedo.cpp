

#include "medToolBoxRegistrationUndoRedo.h"
#include <medAbstractDataImage.h>
#include <medToolBoxTab.h>
#include <medToolBoxFactory.h>
#include <QtGui>
#include <itkRegistrationFactory.h>
#include "hRegistrationFactory.h"
// /////////////////////////////////////////////////////////////////
// medToolBoxRegistrationUndoRedoPrivate
// /////////////////////////////////////////////////////////////////

class medToolBoxRegistrationUndoRedoPrivate
{
public:
    QPushButton * undoButton;
    QPushButton * redoButton;
    QListWidget * TransformationStack;
    QIcon arrowCurrentStep; 
    int currentStep;
   //typedef float ScalarType;
  ///\todo The orientedimage is really something really interesting to look at very closely...
/*   typedef itk::OrientedImage<ScalarType, 3> ImageType; */
   // typedef itk::Image<ScalarType, 3> ImageType;
   // typedef itk::ImageRegistrationFactory<ImageType> RegistrationFactoryType;


   // RegistrationFactoryType::Pointer m_Factory;
};

// /////////////////////////////////////////////////////////////////
// medToolBoxRegistrationUndoRedo
// /////////////////////////////////////////////////////////////////

medToolBoxRegistrationUndoRedo::medToolBoxRegistrationUndoRedo(QWidget *parent) : medToolBox(parent), d(new medToolBoxRegistrationUndoRedoPrivate)
{
 
    // Undo/redo Buttons
    d->undoButton = new QPushButton(QIcon("C:/Users/mfadil/dev/projects/medInria/app/medInria/icons/FlecheBas.png"),tr("Undo"),this);
    d->redoButton = new QPushButton(QIcon("C:/Users/mfadil/dev/projects/medInria/app/medInria/icons/FlecheHaut.png"),tr("Redo"),this);
    
    connect(d->undoButton,SIGNAL(clicked()),this,SLOT(onUndo()));
    connect(d->redoButton,SIGNAL(clicked()),this,SLOT(onRedo()));

    d->arrowCurrentStep = QIcon("C:/Users/mfadil/dev/projects/medInria/app/medInria/icons/blueArrowRight.png");
    d->currentStep = 0;

    // Transformation Stack
    d->TransformationStack = new QListWidget(this);
    d->TransformationStack->addItem(new QListWidgetItem(d->arrowCurrentStep,"TransAffine"));
    d->TransformationStack->addItem("transf1");
    d->TransformationStack->addItem("transf41");
    d->TransformationStack->addItem("transf471");

    //QButtonGroup *layoutButtonGroup = new QButtonGroup(this);

    //layoutButtonGroup->addButton(d->undoButton);
    //layoutButtonGroup->addButton(d->redoButton);

    QHBoxLayout *layoutButtonUndoRedo = new QHBoxLayout;
    layoutButtonUndoRedo->addWidget(d->undoButton);
    layoutButtonUndoRedo->addWidget(d->redoButton);

    //QVBoxLayout *layoutLayout = new QVBoxLayout;
    //layoutLayout->addLayout(layoutButtonUndoRedo);

    QWidget * layoutSection = new QWidget(this);
    //layoutSection->setLayout(layoutLayout);
    layoutSection->setLayout(layoutButtonUndoRedo);
    addWidget(layoutSection);
    addWidget(d->TransformationStack);
    this->setTitle(tr("Stack of transformations"));

    // mettre la ligne suivante apres pour lajout dune transform dans la liste donc certainement apres un success de recalage
    // faire donc communiquer les toolboxes entre elle
    //d->m_Factory->GetGeneralTransform()->InsertTransform();
    hRegistrationFactory * medRegFac = hRegistrationFactory::instance();
    connect(medRegFac,SIGNAL(transformationAdded(void)),this,SLOT(updateTransformationList(void)));
}

medToolBoxRegistrationUndoRedo::~medToolBoxRegistrationUndoRedo(void)
{
    delete d;
    d = NULL;
}

void medToolBoxRegistrationUndoRedo::onUndo()
{
    if (d->currentStep<d->TransformationStack->count()-1)
    {
        d->TransformationStack->item(d->currentStep)->setIcon(QIcon());
        d->currentStep++;
        d->TransformationStack->item(d->currentStep)->setIcon(d->arrowCurrentStep);
    }
 /* try
  {
   // this->GetFactory()->Undo();
  //  this->RefreshGUIToFactory();
  }
  catch (itk::ExceptionObject & e)
  {
  //  std::cerr << e;
  // // std::cerr<<"Error during undo process"<<std::endl;
  //  return;
  }
  
  // update views*/
}
void medToolBoxRegistrationUndoRedo::onRedo()
{
    if (d->currentStep>0)
    {
        d->TransformationStack->item(d->currentStep)->setIcon(QIcon());
        d->currentStep--;
        d->TransformationStack->item(d->currentStep)->setIcon(d->arrowCurrentStep);
    }
  /*try
  {
   // this->GetFactory()->Redo();
   // this->RefreshGUIToFactory();
  }
  //catch (itk::ExceptionObject & e)
  //{
 //   std::cerr << e;
  //  std::cerr<<"Error during redo process"<<std::endl;
  //  return;
  //}
  
  
  // update views*/
}

//medToolBoxRegistrationUndoRedo::RegistrationFactoryType::Pointer medToolBoxRegistrationUndoRedo::GetFactory(void)
//{ 
      //  return d->m_Factory;
//}

void medToolBoxRegistrationUndoRedo::RefreshGUIToFactory(void)
{
  /*if ( !this->CheckInputs() )
  {
    return;
  }*/
  
  //try
 // {
   // switch (m_ImageType)
   // {
	//case IMAGE_TYPE_TENSORS :
	//  m_TensorFactory->UpdateLargestPossibleRegion();
// 	  m_TensorFactory->Update();
	 // break;	  
	//case IMAGE_TYPE_SCALARS :
	//default :
	 // d->m_Factory->UpdateLargestPossibleRegion();
// 	  m_Factory->Update();
	  //break;
    //}

  //  this->SetMovingImageFromFactoryOutput();
  //  this->SetUpFusionPipeline();
  //}
  /*catch (itk::ExceptionObject & e)
  {
    this->Enable();
    std::cerr << e;
    throw itk::ExceptionObject (__FILE__,__LINE__,
				"Error in wxImageFusionMainFrameInteractor::RefreshGUIToFactory()");
  }*/
}

void medToolBoxRegistrationUndoRedo::SetMovingImageFromFactoryOutput (void)
{
  ///\todo refresh the view without overwriting the image !
  //   m_MovingImagePageView->Refresh();
  //   m_FusedImagePageView->Refresh();
    /*
  if (m_ImageType == IMAGE_TYPE_SCALARS)
  {
    itk::ImageDuplicator<ImageType>::Pointer duplicator = itk::ImageDuplicator<ImageType>::New();
    duplicator->SetInputImage (m_Factory->GetOutput());
    duplicator->Update();
    m_MovingImagePageView->GetView1()->SetITKImage (duplicator->GetOutput());  
    m_MovingImagePageView->GetView2()->SetITKImage (duplicator->GetOutput());  
    m_MovingImagePageView->GetView3()->SetITKImage (duplicator->GetOutput());
  }
  else if (m_ImageType == IMAGE_TYPE_TENSORS)
  {
    if (m_TensorFactory->GetTensorFeatureExportType() != TensorRegistrationFactoryType::FEATURE_TYPE_COLOR_FA)
    {
      typedef TensorRegistrationFactoryType::FloatImageType ExtractedType;
      ExtractedType::Pointer extracted;
      
      try
      {
	extracted = m_TensorFactory->ExportScalarFeatureImage(TensorRegistrationFactoryType::INPUT_TYPE_OUTPUT);
      }
      catch (itk::ExceptionObject & e)
      {
	this->Enable();
	std::cerr << e;
	throw itk::ExceptionObject (__FILE__,__LINE__,
				    "Error in wxImageFusionMainFrameInteractor::SetMovingImageFromFactoryOutput()");
      }
      
      m_MovingImagePageView->GetView1()->SetITKImage (extracted);
      m_MovingImagePageView->GetView2()->SetITKImage (extracted);  
      m_MovingImagePageView->GetView3()->SetITKImage (extracted);
    }
    else
    {
      typedef TensorRegistrationFactoryType::ColorImageType ExtractedType;
      ExtractedType::Pointer extracted;
      
      try
      {
	extracted = m_TensorFactory->ExportColorFeatureImage(TensorRegistrationFactoryType::INPUT_TYPE_OUTPUT);
      }
      catch (itk::ExceptionObject & e)
      {
	this->Enable();
	std::cerr << e;
	throw itk::ExceptionObject (__FILE__,__LINE__,
				    "Error in wxImageFusionMainFrameInteractor::SetMovingImageFromFactoryOutput()");
      }

      ExtractedType::DirectionType direction = extracted->GetDirection();
      vtkMatrix4x4 *directionMatrix = vtkMatrix4x4::New();
      directionMatrix->Identity();
      for (int i=0; i<3; i++)
	for (int j=0; j<3; j++)
	  directionMatrix->SetElement (i, j, direction (i,j));
      
      vtkImageData* vtkextracted = this->GenerateColorImage (extracted);
      m_MovingImagePageView->GetView1()->SetImage (vtkextracted);
      m_MovingImagePageView->GetView2()->SetImage (vtkextracted);  
      m_MovingImagePageView->GetView3()->SetImage (vtkextracted);

      m_MovingImagePageView->GetView1()->SetDirectionMatrix ( directionMatrix );
      m_MovingImagePageView->GetView2()->SetDirectionMatrix ( directionMatrix );
      m_MovingImagePageView->GetView3()->SetDirectionMatrix ( directionMatrix );
      
      m_MovingImagePageView->GetView1()->SyncReset();
      vtkextracted->Delete();
      directionMatrix->Delete();
    }
    
      
  }*/
    
}


void medToolBoxRegistrationUndoRedo::SetUpFusionPipeline(bool doreset)
{
/*
  if ( !this->CheckInputs() )
  {
    return;
  }

  m_FusionFilter->SetInput ( 0,  m_FixedImagePageView->GetInput() );
  m_FusionFilter->SetInput ( 1,  m_MovingImagePageView->GetInput() );
  
  m_FusionFilter->Modified();
  m_FusionFilter->Update();
  
  bool doreset2 = doreset || ( !m_FusedImagePageView->GetInput() );
  m_FusedImagePageView->SetInput ( m_FusionFilter->GetOutput(), doreset2, m_FixedImagePageView->GetView1()->GetDirectionMatrix() );
  
  */
}

void medToolBoxRegistrationUndoRedo::Render (void)
{
  /*wxPanel* panel = dynamic_cast<wxPanel*>(m_PageNotebook->GetPage(m_PageNotebook->GetSelection()));
  if (panel == m_FusedImagePanel)
  {
    m_FusedImagePageView->Render();
  }
  else if (panel == m_FixedMovingImagePanel)
  {
    m_FixedImagePageView->Render();
    m_MovingImagePageView->Render();
  }
  else if (panel == m_DisplacementFieldPanel)
  {
    m_DisplacementFieldPageView->Render();
  }*/
}

void medToolBoxRegistrationUndoRedo::updateTransformationList(void)
{
    hRegistrationFactory * medRegFac = hRegistrationFactory::instance();

}


