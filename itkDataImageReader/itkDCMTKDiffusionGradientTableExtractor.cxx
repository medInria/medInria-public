/* itkDcmtkReaderTest.cxx --- 
 * 
 * Author: Pierre Fillard
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  2 21:46:08 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct  2 21:47:00 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 3
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <iostream>

#include <itkDCMTKImageIO.h>
#include <itkImageFileReader.h>
#include <itkMetaDataDictionary.h>



int main (int narg, char* arg[])
{

    if( narg<3 )
    {
        std::cerr << "Usage: " << arg[0] << " <dicom1 dicom2 ... dicomN> <output>\n";
        return -1;
    }
  
    std::vector<std::string> fileNames;
    for( int i=1; i<narg-1; i++){
        std::cout << "Inserting: " << arg[i] << std::endl;
        fileNames.push_back ( arg[i] );
    }

    const char* output = arg[narg-1];
    
  
    itk::DCMTKImageIO::Pointer io = itk::DCMTKImageIO::New();
    io->SetFileNames ( fileNames );

    try
    {
      io->ReadImageInformation();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << e;
      return -1;
    }



    //itk::MetaDataDictionary& dicomDictionary = io->GetMetaDataDictionary();
    const itk::DCMTKImageIO::StringVectorType vecx = io->GetMetaDataValueVectorString ("(0019,10bb)");
    const itk::DCMTKImageIO::StringVectorType vecy = io->GetMetaDataValueVectorString ("(0019,10bc)");
    const itk::DCMTKImageIO::StringVectorType vecz = io->GetMetaDataValueVectorString ("(0019,10bd)");

    
    std::vector< std::vector<double> > gradients;
      
    for (unsigned int i=0; i<vecx.size(); i++) {
      std::vector<double> vec (3);
      vec[0] = atof (vecx[i].c_str());
      vec[1] = atof (vecy[i].c_str());
      vec[2] = atof (vecz[i].c_str());
      //std::cout << vecx[i] << " " << vecy[i] << " " << vecz[i] << std::endl;
      gradients.push_back (vec);
    }

    std::vector< std::vector<double> >::iterator new_end = std::unique (gradients.begin(), gradients.end());

    std::vector< std::vector<double> >::iterator it = gradients.begin();

    std::ofstream buf (output);
    while (it!=new_end) {
      buf << (*it)[0] << " " << (*it)[1] << " " << (*it)[2] << "\n";
      ++it;
    }
  
    return 0;  
}
