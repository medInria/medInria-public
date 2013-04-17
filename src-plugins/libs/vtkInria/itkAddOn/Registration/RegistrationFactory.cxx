/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <cstddef> // For ITK 3.20 that does not define correctly ptrdiff_t

#include "itkCommand.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"
#include "itkTransformToDeformationFieldFilter.h"
#include "itkTranslationTransform.h"
#include "itkTransformFactory.h"

#include <itkAnalyzeImageIOFactory.h>
#include <itkGDCMImageIOFactory.h>
#include <itkImageRegistrationFactory.h>

#include <itkAffineRegistrationMethod.h>
#include <itkRigidRegistrationMethod.h>

#include <itkOrientedImage.h>

#ifndef WIN32
#include <getopt.h>
#else
#include "getoptcompat/getopt.h"
#endif //WIN32
#include <iostream>


struct arguments
{
   std::string  fixedImageFile;  /* -f option */
   std::string  movingImageFile; /* -m option */
   std::string  inputFieldFile;  /* -b option */
   std::string  inputTransformFile;  /* -p option */
   std::string  outputImageFile; /* -o option */
   std::string  outputTransformFile; /* -O option */
   std::string  outputFieldFile; /* -O option */
   unsigned int registrationType; /* -r option */
   unsigned int verbosity;       /* -v option */


   arguments () :
      fixedImageFile(""),
      movingImageFile(""),
      inputFieldFile(""),
      inputTransformFile(""),
      outputImageFile("output.mha"),
      outputTransformFile("output.mat"),
      registrationType(0),
      verbosity(0u)
   {
   }

   friend std::ostream& operator<< (std::ostream& o, const arguments& args)
   {
      std::ostringstream osstr;
      std::string iterstr = "[ " + osstr.str() + "]";

         
      return o
         <<"Arguments structure:"<<std::endl
         <<"  Fixed image file: "<<args.fixedImageFile<<std::endl
         <<"  Moving image file: "<<args.movingImageFile<<std::endl
         <<"  Input field file: "<<args.inputFieldFile<<std::endl
         <<"  Input transform file: "<<args.inputTransformFile<<std::endl
         <<"  Output image file: "<<args.outputImageFile<<std::endl
         <<"  Output field file: "<<args.outputFieldFile<<std::endl
         <<"  Output transform file: "<<args.outputTransformFile<<std::endl
	 <<"  Registration Type: "<<args.registrationType<<std::endl	
         <<"  Verbosity: "<<args.verbosity;
   }
};

static const char *optString = "f:m:r:b:p:t:o:O::v::h?";

static const struct option longOpts[] = {
   { "fixed-image", required_argument, NULL, 'f' },
   { "moving-image", required_argument, NULL, 'm' },
   { "input-field", required_argument, NULL, 'b' },
   { "input-transform", required_argument, NULL, 'p' },
   { "output-image", required_argument, NULL, 'o' },
   { "output-field", optional_argument, NULL, 'O' },
   { "output-transform", required_argument, NULL, 't' },
   { "registration-type", optional_argument, NULL, 'r' },
   { "verbose", optional_argument, NULL, 'v' },
   { "help", no_argument, NULL, 'h' },
   { NULL, no_argument, NULL, 0 }
};

/*
  Display program usage, and exit.
*/
void display_usage( const std::string progname )
{   
   struct arguments defargs = arguments();

   std::ostringstream osstr;
   std::string iterstr = "[ " + osstr.str() + "]";
   
   std::cout<<std::endl;
   std::cout<<progname<<" - register 2 images using the registration factory"<<std::endl;
   std::cout<<"Usage: "<<progname<<" [OPTION...]"<<std::endl;
   
   std::cout<<"  -f/--fixed-image=STRING    Fixed image filename - mandatory"<<std::endl;
   std::cout<<"  -m/--moving-image=STRING   Moving image filename - mandatory"<<std::endl;
   std::cout<<"  -r/--registration-type(=UINT) Registration Type (0 : affine / 1 : rigid) - default : 0"<<std::endl;
   std::cout<<"  -b/--input-field=STRING    Input field filename - default: empty"<<std::endl;
   std::cout<<"  -p/--input-transform=STRING Input transform filename - default: empty"<<std::endl;
   std::cout<<"  -o/--output-image=STRING   Output image filename - default: "<<defargs.outputImageFile<<std::endl;
   std::cout<<"  -O/--output-field(=STRING) Output field filename - default: OUTPUTIMAGENAME-field.mha"<<std::endl;
   std::cout<<"  -p/--output-transform=STRING Output transform filename - default: output.mat"<<std::endl;
   std::cout<<"  -v/--verbose(=UINT)        Verbosity - default: "<<defargs.verbosity<<"; without argurment: 1"<<std::endl;
   std::cout<<"  -h/--help                  Display this message and exit"<<std::endl;

   std::cout<<std::endl;
   std::cout<<"Copyright (c) 2008 INRIA."<<std::endl;
   std::cout<<"Code: Nicolas Toussaint."<<std::endl;
   std::cout<<"Report bugs to <nicolas.toussaint@sophia.inria.com>."<<std::endl;
   
   exit( EXIT_FAILURE );
}



std::vector<unsigned int> parseUIntVector( const std::string & str)
{
   std::vector<unsigned int> vect;
   
   std::string::size_type crosspos = str.find('x',0);

   if (crosspos == std::string::npos)
   {
      // only one uint
      vect.push_back( static_cast<unsigned int>( atoi(str.c_str()) ));
      return vect;
   }

   // first uint
   vect.push_back( static_cast<unsigned int>(
                      atoi( (str.substr(0,crosspos)).c_str()  ) ));

   while(true)
   {
      std::string::size_type crossposfrom = crosspos;
      crosspos =  str.find('x',crossposfrom+1);

      if (crosspos == std::string::npos)
      {
         vect.push_back( static_cast<unsigned int>(
                            atoi( (str.substr(crossposfrom+1,str.length()-crossposfrom-1)).c_str()  ) ));
         return vect;
      }

      vect.push_back( static_cast<unsigned int>(
                         atoi( (str.substr(crossposfrom+1,crosspos)).c_str()  ) ));
   }
}



void parseOpts (int argc, char **argv, struct arguments & args)
{
   const std::string progname( "ImageRegistrationFactory" );
   
   // Default values.
   args = arguments();

   if (argc == 1)
   {
      display_usage(progname);
   }
   
   int opt = 0; /* it's actually going to hold a char */
   int longIndex = 0;

   while ( (opt = getopt_long(argc, argv, optString, longOpts, &longIndex)) != -1 )
   {
      switch( opt ) {
      case 'f':
         if (! optarg) display_usage(progname);
         args.fixedImageFile = optarg;
         break;
			
      case 'm':
         if (! optarg) display_usage(progname);
         args.movingImageFile = optarg;
         break;

      case 'b':
         if (! optarg) display_usage(progname);
         else args.inputFieldFile = optarg;
         break;

      case 'p':
         if (! optarg) display_usage(progname);
         else args.inputTransformFile = optarg;
         break;

      case 'o':
         if (! optarg) display_usage(progname);
         args.outputImageFile = optarg;
         break;

      case 'O':
         if (! optarg) args.outputFieldFile = "CHANGETHISSTRING";
         else args.outputFieldFile = optarg;
         break;
      case 't':
         if (! optarg) display_usage(progname);
         else args.outputTransformFile = optarg;
         break;
			
      case 'r':
         if (! optarg) display_usage(progname);
         else args.registrationType = static_cast<unsigned int>( atoi(optarg) );
         break;
	 
      case 'v':
         if (! optarg) args.verbosity++;
         else args.verbosity = static_cast<unsigned int>( atoi(optarg) );
         break;
			
      case 'h':	/* fall-through is intentional */
      case '?':   /* fall-through is intentional */
      default:
         display_usage(progname);
         break;
		}
   }

   if ( args.outputFieldFile=="CHANGETHISSTRING" )
   {
      
      unsigned int pos = args.outputImageFile.find(".");
      if ( pos < args.outputFieldFile.size() )
      {
         args.outputFieldFile = args.outputImageFile;
         args.outputFieldFile.replace(pos, args.outputFieldFile.size(), "-field.mha");
      }
      else
      {
         args.outputFieldFile = args.outputImageFile + "-field.mha";
      }
         
   }

   if ( !args.inputFieldFile.empty() &&
        !args.inputTransformFile.empty() )
   {
      std::cout<<"You cannot impose both an initial deformation field and an initial spatial transformation."<<std::endl;
      display_usage(progname);
   }
}











//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
//
template <class TPixel=float, unsigned int VImageDimension=3>
class CommandIterationUpdate : public itk::Command 
{
public:
   typedef  CommandIterationUpdate   Self;
   typedef  itk::Command             Superclass;
   typedef  itk::SmartPointer<Self>  Pointer;

   itkNewMacro( Self );

private:
   std::ofstream m_Fid;
   bool m_headerwritten;

public:
   
   void Execute(itk::Object *caller, const itk::EventObject & event)
   {
      Execute( (const itk::Object *)caller, event);
   }

   void Execute(const itk::Object * object, const itk::EventObject & event)
   {
      if( !(itk::IterationEvent().CheckEvent( &event )) )
      {
         return;
      }

      unsigned int iter = -1;
      
 //      if ( const DiffeomorphicDemonsRegistrationFilterType * filter = dynamic_cast< const DiffeomorphicDemonsRegistrationFilterType * >( object ) )
      if ( 1)
      {
      }
      else
      {
	return;
      }
      std::cout<<iter<<std::endl;
      
      
      
      if (this->m_Fid.is_open())
      {
	if (! m_headerwritten)
	{
	  this->m_Fid<<"Iteration"
		     <<", MSE before"
		     <<", Harmonic energy"
		     <<", min|Jac|"
		     <<", 0.2% |Jac|"
		     <<", 01% |Jac|"
		     <<", 99% |Jac|"
		     <<", 99.8% |Jac|"
		     <<", max|Jac|"
		     <<", ratio(|Jac|<=0)";               
	  
	  this->m_Fid<<std::endl;
          
	  m_headerwritten = true;
	}
        
	this->m_Fid<<iter;
	
	this->m_Fid<<std::endl;
      }
   }
  
protected:   
   CommandIterationUpdate() :
      m_Fid( "metricvalues.csv" ),
      m_headerwritten(false)
   {
   };

   ~CommandIterationUpdate()
   {
      this->m_Fid.close();
   }
};








template <class TPixel, unsigned int Dimension>
void ImageRegistrationFactoryFunction( arguments args )
{


  // Declare the types of the images (float or double only)
  typedef TPixel PixelType;
  typedef itk::OrientedImage< PixelType, Dimension >  ImageType;

  typedef itk::Vector< PixelType, Dimension >    VectorPixelType;
  typedef typename itk::Image < VectorPixelType, Dimension > DeformationFieldType;

  typedef itk::ImageRegistrationFactory<ImageType> ImageRegistrationFactoryType;
  

  // Set up the file readers
  typedef itk::ImageFileReader< ImageType > FixedImageReaderType;
  typedef itk::ImageFileReader< ImageType > MovingImageReaderType;
  typedef itk::ImageFileReader< DeformationFieldType > FieldReaderType;
  typedef itk::TransformFileReader TransformReaderType;
  typedef typename ImageRegistrationFactoryType::TransformType InputTransformType;
  
  // registration methods to be added to the factory.
  typedef typename itk::RegistrationMethod<ImageType>       MethodType;
  typedef typename itk::AffineRegistrationMethod<ImageType> AffineMethodType;
  typedef typename itk::RigidRegistrationMethod<ImageType>  RigidMethodType;
//   typedef typename itk::DiffeomorphicDemonsRegistrationMethod<ImageType> DiffeomorphicDemonsMethodType;
  
  // Images we use
  typename ImageType::Pointer fixedImage = 0;
  typename ImageType::Pointer movingImage = 0;
  typename DeformationFieldType::Pointer inputDefField = 0;
  typename InputTransformType::Pointer  inputTransform = 0;

  
  typename FixedImageReaderType::Pointer fixedImageReader
    = FixedImageReaderType::New();
  typename MovingImageReaderType::Pointer movingImageReader
    = MovingImageReaderType::New();
  
  fixedImageReader->SetFileName( args.fixedImageFile.c_str() );
  movingImageReader->SetFileName( args.movingImageFile.c_str() );

  // Update the reader
  try
   {
     fixedImageReader->Update();
     movingImageReader->Update();
   }
  catch( itk::ExceptionObject& err )
   {
     std::cout << "Could not read one of the input images." << std::endl;
     std::cout << err << std::endl;
     exit( EXIT_FAILURE );
   }

  fixedImage = fixedImageReader->GetOutput();
  movingImage = movingImageReader->GetOutput();
  
  
  if ( ! args.inputFieldFile.empty() )
  {
    // Set up the file readers
    typename FieldReaderType::Pointer fieldReader = FieldReaderType::New();
    fieldReader->SetFileName(  args.inputFieldFile.c_str() );
    
    // Update the reader
    try
    {
      fieldReader->Update();
    }
      catch( itk::ExceptionObject& err )
      {
         std::cout << "Could not read the input field." << std::endl;
         std::cout << err << std::endl;
         exit( EXIT_FAILURE );
      }
    
    inputDefField = fieldReader->GetOutput();
    inputDefField->DisconnectPipeline();
  }
  else if ( ! args.inputTransformFile.empty() )
  {
    std::cout<<"entering the transform file"<<std::endl;
    
    // Set up the transform reader
    //itk::TransformFactory<BSplineTransformType>::RegisterTransform();
    typedef typename ImageRegistrationFactoryType::LinearTransformType LinearTransformType;
    
    itk::TransformFactory<LinearTransformType>::RegisterTransform ();

    typename TransformReaderType::Pointer transformReader
      = TransformReaderType::New();    
    transformReader->SetFileName(  args.inputTransformFile.c_str() );
    
    // Update the reader
    try
    {
      transformReader->Update();
    }
    catch( itk::ExceptionObject& err )
    {
      std::cout << "Could not read the input transform." << std::endl;
      std::cout << err << std::endl;
      exit( EXIT_FAILURE );
    }
    
    typedef typename TransformReaderType::TransformType BaseTransformType;
    BaseTransformType* baseTrsf(0);
    
    const typename TransformReaderType::TransformListType* trsflistptr
      = transformReader->GetTransformList();
    if ( trsflistptr->empty() )
    {
      std::cout << "Could not read the input transform." << std::endl;
      exit( EXIT_FAILURE );
    }
    else if (trsflistptr->size()>1 )
    {
      std::cout << "The input transform file contains more than one transform, we use the first one." << std::endl;
      exit( EXIT_FAILURE );
    }
    
    baseTrsf = trsflistptr->front();
    if ( !baseTrsf )
    {
      std::cout << "Could not read the input transform." << std::endl;
      exit( EXIT_FAILURE );
    }
    

    // Set up the TransformToDeformationFieldFilter
    typedef itk::TransformToDeformationFieldFilter
      <DeformationFieldType> FieldGeneratorType;
    typedef typename FieldGeneratorType::TransformType TransformType;

    inputTransform = dynamic_cast<InputTransformType*>(baseTrsf);
    if ( !inputTransform )
    {
      std::cout << "Could not cast input transform to a usable transform for the factory." << std::endl;
      exit( EXIT_FAILURE );
    }
    
    TransformType* trsf = dynamic_cast<TransformType*>(baseTrsf);
    if ( !trsf )
    {
      std::cout << "Could not cast input transform to a usable transform." << std::endl;
      exit( EXIT_FAILURE );
    }

    

    typename FieldGeneratorType::Pointer fieldGenerator
      = FieldGeneratorType::New();
    
    fieldGenerator->SetTransform( trsf );
    fieldGenerator->SetOutputRegion(
				    fixedImageReader->GetOutput()->GetRequestedRegion());
    fieldGenerator->SetOutputSpacing(
				     fixedImageReader->GetOutput()->GetSpacing());
    fieldGenerator->SetOutputOrigin(
				    fixedImageReader->GetOutput()->GetOrigin());
    
    // Update the fieldGenerator
    try
    {
      fieldGenerator->Update();
    }
    catch( itk::ExceptionObject& err )
    {
      std::cout << "Could not generate the input field." << std::endl;
      std::cout << err << std::endl;
      exit( EXIT_FAILURE );
    }
    
    inputDefField = fieldGenerator->GetOutput();
    inputDefField->DisconnectPipeline();
  }
  
  

  typename ImageRegistrationFactoryType::Pointer factory =
    itk::ImageRegistrationFactory<ImageType>::New();

  factory->UseFixedImageOff();
  factory->UseInnerTransformationOff ();

  typename MethodType::Pointer method = NULL;
  switch( args.registrationType )
  {
  default:
  case 0:
		method = AffineMethodType::New();
		break;
  case 1:
	  method = RigidMethodType::New();
	  break;
  }

  factory->AddRegistrationMethod (0, method.GetPointer());
  
  factory->SetFixedImage (fixedImage);
  factory->SetMovingImage (movingImage);
  
  std::cout << factory->CheckImagesConsistency(fixedImage, movingImage) << std::endl;  
  
  if (!inputTransform.IsNull())
  {
    factory->SetInitialTransform (inputTransform.GetPointer());
  }
  
  // Update the factory
  try
  {
    factory->Initialize();
    if (!factory->CheckImagesConsistency (fixedImage, movingImage))
    {
      factory->SkipResamplingOff();
      factory->UpdateLargestPossibleRegion();
    }
    
    
    factory->ApplyRegistrationMethod (0);
    factory->Update();
    factory->WriteOutput(args.outputImageFile.c_str());
    if ( ! args.outputTransformFile.empty() )
    {
      factory->WriteGlobalLinearTransform (args.outputTransformFile.c_str());
    }    
    
  }
  catch( itk::ExceptionObject& err )
  {
    std::cout << "Could not register images !." << std::endl;
    std::cout << err << std::endl;
    exit( EXIT_FAILURE );
  }
  
  factory->RemoveRegistrationMethod(0);
};
   



int main( int argc, char *argv[] )
{
   struct arguments args;
   parseOpts (argc, argv, args);

   itk::MultiThreader::SetGlobalDefaultNumberOfThreads(2);
   
   std::cout<<"Starting registration with the following arguments:"<<std::endl;
   std::cout<<args<<std::endl<<std::endl;
 
   // Get the image dimension
   itk::ImageIOBase::Pointer imageIO = 0;
   
   try
   {
     
     imageIO =
       itk::ImageIOFactory::CreateImageIO(
					  args.fixedImageFile.c_str(), itk::ImageIOFactory::ReadMode);
     imageIO->SetFileName(args.fixedImageFile.c_str());
     
     imageIO->ReadImageInformation();
   }
   catch(itk::ExceptionObject &e)
   {
     std::cerr << e;
     exit( EXIT_FAILURE );
   }

   
   switch ( imageIO->GetNumberOfDimensions() )
   {
   case 3:
     ImageRegistrationFactoryFunction<float, 3>(args);
      break;
   default:
      std::cerr << "Unsupported dimension" << std::endl;
      exit( EXIT_FAILURE );
   }

   
   return EXIT_SUCCESS;
}

