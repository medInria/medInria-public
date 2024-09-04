/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 All rights reserved.
 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medInrimageImageIO.h"
#include <itkMacro.h>
#include <itkByteSwapper.h>
#include <itkImageIOBase.h>

#include <qmath.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#if defined(_WIN32) && (defined(_MSC_VER) || defined(__BORLANDC__))
#include <stdlib.h>
#include <windows.h>
#define _unlink unlink

LPWSTR convertUTF8to16(char const * pi_pchStringToConvert)
{
    WCHAR * pwcRes = nullptr;
    
    int iWCharBuffSize = MultiByteToWideChar( CP_UTF8, 0, pi_pchStringToConvert, -1, nullptr, 0 );
    if (iWCharBuffSize > 0)
    {
        pwcRes = new WCHAR[iWCharBuffSize+1];
        if (MultiByteToWideChar(CP_UTF8, 0, pi_pchStringToConvert, -1, pwcRes, iWCharBuffSize + 1) != iWCharBuffSize)
        {
            delete[] pwcRes;
            pwcRes = nullptr;
        }
    }

    return pwcRes;
}

gzFile universal_GzOpen(char const * pi_pchPath, char const *pi_pchMode)
{
    gzFile fileRes = nullptr;

    LPWSTR pathU16 = convertUTF8to16(pi_pchPath);
    if (pathU16)
    {
        fileRes = ::gzopen_w(pathU16, pi_pchMode);
        delete[] pathU16;
    }

    return fileRes;
}
#else
#include <unistd.h>
#define universal_GzOpen ::gzopen
#endif


//
// GetExtension from itkAnalyzeImageIO.cxx
//
static std::string GetExtension(const std::string& filename)
{

    const char* c_filename = filename.c_str();
    std::string ext = "";

    int len = strlen(c_filename);
    if (len < 4)
    {
        return ext;
    }

    ext = c_filename + len - 4;

    // search for .inr
    if (ext == ".inr")
    {
        return ext;
    }

    // else we try to find .inr.gz
    if (len < 7)
    {
        return ext;
    }

    ext = c_filename + len - 7;

    return ext;
}

//
// GetRootName from itkAnalyzeImageIO.cxx
//
static std::string GetRootName(const std::string& filename)
{
    const std::string fileExt = GetExtension(filename);

    // Create a base filename
    // i.e Image.gif --> Image
    if (fileExt.length() > 0)
    {
        const std::string::size_type it = filename.find_last_of(fileExt);
        std::string baseName(filename, 0, it - fileExt.length());
        return(baseName);
    }
    else
    {
        // Case when the extension is nothing
        return(filename);
    }
}


// Default constructor
InrimageImageIO::InrimageImageIO()
{
    // by default, only have 2 dimensions
    this->SetNumberOfDimensions(2);

    // Initialization
    this->SetOrigin(0, 0.0);
    this->SetOrigin(1, 0.0);

    this->SetSpacing(0, 1.0);
    this->SetSpacing(1, 1.0);

    // pixel type is UCHAR
    // m_PixelType is a protected member of ImageIOBase (IODataType itk::ImageIOBase::m_PixelType)
    this->SetPixelType(itk::IOPixelEnum::SCALAR);
    this->SetNumberOfComponents(1);
    this->SetComponentType(itk::IOComponentEnum::DOUBLE);


    // Byte order
    this->SetByteOrder((itk::ByteSwapper<int>::SystemIsBigEndian() == true) ? itk::IOByteOrderEnum::BigEndian : itk::IOByteOrderEnum::LittleEndian);

    // additionnal information
    // (inrimage only)
    m_file = NULL;
    //length of header of a multiple of 256
    m_NumberBlocksInHeader = 1;

}



InrimageImageIO::~InrimageImageIO()
{
    //Purposefully left blank
}



void InrimageImageIO::PrintSelf(std::ostream& os, itk::Indent indent) const
{
    Superclass::PrintSelf(os, indent);
    os << "LOCAL PARAMETERS" << std::endl;
    os << indent << "hdr=" << m_NumberBlocksInHeader << std::endl;
    os << "GLOBAL PARAMETERS" << std::endl;
    os << indent << "ByteOrder=" << m_ByteOrder << std::endl;
    os << indent << "PixelType " << m_PixelType << std::endl;
}

const std::type_info& InrimageImageIO::ConvertToTypeInfo(itk::IOComponentEnum t) const
{

    switch (t)
    {
        case itk::IOComponentEnum::UCHAR:
            return typeid(unsigned char);
        case itk::IOComponentEnum::CHAR:
            return typeid(char);
        case itk::IOComponentEnum::USHORT:
            return typeid(unsigned short);
        case itk::IOComponentEnum::SHORT:
            return typeid(short);
        case itk::IOComponentEnum::UINT:
            return typeid(unsigned int);
        case itk::IOComponentEnum::INT:
            return typeid(int);
        case itk::IOComponentEnum::ULONG:
            return typeid(unsigned long);
        case itk::IOComponentEnum::LONG:
            return typeid(long);
        case itk::IOComponentEnum::FLOAT:
            return typeid(float);
        case itk::IOComponentEnum::DOUBLE:
            return typeid(double);
            /*case RGB:
              return typeid(RGBPixel<unsigned char>);
              case RGBA:
              return typeid(RGBAPixel<unsigned char>);
             */
        default:
            itkExceptionMacro("Invalid type: " << m_PixelType);
    }
    return typeid(itk::IOComponentEnum::UNKNOWNCOMPONENTTYPE);

}

// called from CreateImageIO ( Insight/Code/IO/itkImageIOFactory.cxx )
// called from ImageFileReader::GenerateOutputInformation ( Insight/Code/IO/itkImageFileReader.txx )
bool InrimageImageIO::CanReadFile(const char* FileNameToRead)
{
    m_FileName = FileNameToRead;

    // don't care about extensions
    // should be 'inr' but can be everything else

    // try to open the file
    m_file = universal_GzOpen(m_FileName.c_str(), "rb");
    if (m_file == NULL)
    {
        std::string fileExt = GetExtension(m_FileName);
        // if it doesn't work, try by subtracting extension .gz
        if (!fileExt.compare(".inr.gz"))
        {
            std::string ImageFileName = GetRootName(m_FileName);
            m_file = universal_GzOpen(ImageFileName.c_str(), "rb");
            if (m_file == NULL)
            {
                return false;
            }
            m_FileName = ImageFileName;
        }
        // if it still doesn't work, try by adding extension .gz
        else
        {
            std::string ImageFileName = m_FileName;
            ImageFileName += ".gz";
            m_file = universal_GzOpen(ImageFileName.c_str(), "rb");
            if (m_file == NULL)
            {
                return false;
            }
            m_FileName = ImageFileName;
        }
    }

    // here m_file is open
    char buf_header[257];
    int nread;


    // find magic string at beginning
    nread = ::gzread(m_file, buf_header, 256);
    if (nread != 256) {
        ::gzclose(m_file);
        return false;
    }

    if (strncmp(buf_header, "#INRIMAGE-4#{", strlen("#INRIMAGE-4#{")) != 0) {
        ::gzclose(m_file);
        return false;
    }

    buf_header[256] = '\0';
    m_header = buf_header;
    m_NumberBlocksInHeader = 1;

    // find magic string at end of header
    while (strncmp(&(buf_header[252]), "##}\n", strlen("##}\n")) != 0) {
        nread = ::gzread(m_file, buf_header, 256);
        if (nread != 256) {
            ::gzclose(m_file);
            return false;
        }

        buf_header[256] = '\0';
        m_header += buf_header;
        m_NumberBlocksInHeader++;
    }

    // check endianess
    // (as in itkAnalyzeImageIO.cxx)

    ::gzclose(m_file);

    return true;
}

void InrimageImageIO::GetRotationMatrixFromAngles(double rx, double ry, double rz, vnl_matrix <double> &rotationMatrix)
{
    rotationMatrix.set_size(3,3);

    double c, s, k1, k2;
    vnl_vector <double> r(3);
    r[0] = rx;
    r[1] = ry;
    r[2] = rz;

    double theta2 = 0.0;
    for (unsigned int i = 0;i < 3;++i)
        theta2 += r[i] * r[i];

    double theta = std::sqrt(theta2);
    if (theta > 1.0e-5)
    {
        c = std::cos(theta);
        s = std::sin(theta);
        k1 = s / theta;
        k2 = (1 - c) / theta2;
    }
    else
    {
        // Taylor expansion around theta = 0
        k2 = 1.0/2.0 - theta2/24.0;
        c = 1.0 - theta2*k2;
        k1 = 1.0 - theta2/6;
    }

    /* I + M*Mt */
    for (unsigned int i = 0; i < 3; i++)
    {
        for (unsigned int j = 0; j <= i; j++)
        {
            rotationMatrix(i, j) = k2 * r[i] * r[j];
            if (i != j)
                rotationMatrix(j,i) = rotationMatrix(i,j);
            else
                rotationMatrix(i,i) += c;
        }
    }

    double aux = k1 * r[2];
    rotationMatrix(0,1) -= aux;
    rotationMatrix(1,0) += aux;
    aux = k1 * r[1];
    rotationMatrix(0,2) += aux;
    rotationMatrix(2,0) -= aux;
    aux = k1 * r[0];
    rotationMatrix(1,2) -= aux;
    rotationMatrix(2,1) += aux;
}

void InrimageImageIO::GetRotationAnglesFromMatrix(const vnl_matrix <double> &rotationMatrix, std::vector <double> &r)
{
    r.resize(3);

    // Compute the rotation vector
    std::vector <double> sin_r;
    double d, sq, fact;
    double c = 0.5 * (rotationMatrix(0,0) + rotationMatrix(1,1) + rotationMatrix(2,2) - 1.0); // cos(theta)
    if ((d = std::abs(c)) > 1.0)
        c /= d;
    double theta = std::acos(c);

    if (std::abs(theta) <= 1.0e-5)
    {
        // R close to identity : DL around theta = 0
        fact = 0.5 + theta*theta/12.0;
        r[0] = (rotationMatrix(2,1) - rotationMatrix(1,2)) * fact;
        r[1] = (rotationMatrix(0,2) - rotationMatrix(2,0)) * fact;
        r[2] = (rotationMatrix(1,0) - rotationMatrix(0,1)) * fact;
    }
    else if (std::abs(theta - M_PI) <= 0.1)
    {
        // R is close to a symetry : DL around theta = M_PI
        // set values with diag terms of n.nt
        double rNorm = 0.0;
        for (int i = 0; i < 3; i++)
        {
            // approx order 0
            // c = (rot[i][i] - 1.0)*0.5;
            sq = 1.0 + (rotationMatrix(i,i) - 1.0)/(1-c);
            // 0 <=  [ sq = (n[i])^2 ] <= 1
            if (sq < 0.0) sq = 0.;
            if (sq > 1.) sq = 1.;
            r[i] = std::sqrt(sq);
            rNorm += sq;
        }

        rNorm = std::sqrt(rNorm);
        for (int i = 0; i < 3; i++)
            r[i] *= theta / rNorm;

        // set signs with off-diag terms of n.nt
        if ( (rotationMatrix(0,1) + rotationMatrix(1,0) ) < 0.0)
            r[1] *= -1;
        if ( (rotationMatrix(0,2) + rotationMatrix(2,0) ) < 0.0)
            r[2] *= -1;

        // determine wether r = +/- theta*n
        sin_r.push_back( rotationMatrix(2,1) - rotationMatrix(1,2) );
        sin_r.push_back( rotationMatrix(0,2) - rotationMatrix(2,0) );
        sin_r.push_back( rotationMatrix(1,0) - rotationMatrix(0,1) );

        // determine the most significant term
        unsigned int k = 0;
        if ( std::abs(sin_r[1]) > std::abs(sin_r[k]))
            k = 1;
        if ( std::abs(sin_r[2]) > std::abs(sin_r[k]))
            k = 2;

        // choose the sign
        if ( sin_r[k] * r[k] < 0.0 )
        {
            // They do not have the same signs : r = -v
            for (int i = 0; i < 3; i++)
                r[i] *= -1;
        }
    }
    else
    { // At last no numerical problem
        fact = 0.5 * theta / std::sin(theta);
        r[0] = (rotationMatrix(2,1) - rotationMatrix(1,2)) * fact;
        r[1] = (rotationMatrix(0,2) - rotationMatrix(2,0)) * fact;
        r[2] = (rotationMatrix(1,0) - rotationMatrix(0,1)) * fact;
    }
}

void InrimageImageIO::ReadImageInformation()
{
    // m_header should have been read (in CanReadFile)
    if (m_header.length() == 0)
        this->CanReadFile(this->GetFileName());


    // file endianess
    // default = Sytem
    if (itk::ByteSwapper<int>::SystemIsBigEndian() == true)
        this->SetByteOrderToBigEndian();
    else
        this->SetByteOrderToLittleEndian();

    // pixel type
    // to be read
    int pixsize = 0;
    std::string type;
    int ncomponents = this->GetNumberOfComponents();

    // parsing the header line by line
    std::istringstream issh(m_header);
    std::string line;


    double rx = 0, ry = 0, rz = 0;

    double origin[3] = { 0.0, 0.0, 0.0 };
    double voxelsize[3] = { 0.0, 0.0, 0.0 };
    unsigned long dim[3] = { 0, 0, 0 };

    while (std::getline(issh, line)) {

        // skip comments and empty lines
        if (line[0] == '#') continue;
        if (line.length() == 0) continue;

        // cout << "1#" << line << "#1" << endl;

        // translate remaining lines
        // go to "="
        std::istringstream issl(line);
        std::string id;
        std::getline(issl, id, '=');

        int i;
        double r;
        std::string s;


        // to have information about the used methods
        // see http://www.itk.org/Insight/Doxygen/html/classitk_1_1ImageIOBase.html
        // documentation of "Abstract superclass defines image IO interface" itk::ImageIOBase


        switch (id[0]) {

            default:
                break;

            case 'C':
                if (std::string(id, 0, strlen("CPU")).compare("CPU") == 0) {
                    issl >> s;
                    if (std::string(s, 0, strlen("decm")).compare("decm") == 0
                            || std::string(s, 0, strlen("alpha")).compare("alpha") == 0
                            || std::string(s, 0, strlen("linux")).compare("linuxdecm") == 0
                            || std::string(s, 0, strlen("pc")).compare("pc") == 0) {
                        this->SetByteOrderToLittleEndian();
                    }
                    else if (std::string(s, 0, strlen("sun")).compare("sun") == 0
                             || std::string(s, 0, strlen("sgi")).compare("sgi") == 0) {
                        this->SetByteOrderToBigEndian();
                    }
                    else {
                        itkExceptionMacro("Endianess (CPU) Unknown");
                        return;
                    }
                }
                break;

            case 'P':
                if (std::string(id, 0, strlen("PIXSIZE")).compare("PIXSIZE") == 0)
                {
                    issl >> pixsize;
                    issl >> s;

                    // cerr << pixsize << s << endl;
                    if ((pixsize == 0) || (pixsize % 8 != 0) || (std::string(s, 0, strlen("bits")).compare("bits") != 0))
                    {
                        itk::ExceptionObject exception(__FILE__, __LINE__);
                        exception.SetDescription("Pixel Size (PIXSIZE) Unknown");
                        throw exception;
                    }
                }
                break;

            case 'S':
                // SCALE =
                ;
                break;

            case 'T':
                if (std::string(id, 0, strlen("TYPE")).compare("TYPE") == 0) {
                    issl >> type;
                }
                else if (std::string(id, 0, strlen("TX")).compare("TX") == 0) {
                    issl >> r;
                    origin[0] = r;
                }
                else if (std::string(id, 0, strlen("TY")).compare("TY") == 0) {
                    issl >> r;
                    origin[1] = r;
                }
                else if (std::string(id, 0, strlen("TZ")).compare("TZ") == 0) {
                    issl >> r;
                    origin[2] = r;
                }
                break;

            case 'R':
                if (std::string(id, 0, strlen("RX")).compare("RX") == 0) {
                    issl >> rx;
                }
                else if (std::string(id, 0, strlen("RY")).compare("RY") == 0) {
                    issl >> ry;
                }
                else if (std::string(id, 0, strlen("RZ")).compare("RZ") == 0) {
                    issl >> rz;
                }
                break;

            case 'X':
                if (std::string(id, 0, strlen("XDIM")).compare("XDIM") == 0) {
                    issl >> i;
                    dim[0] = i;
                }
                break;

            case 'Y':
                if (std::string(id, 0, strlen("YDIM")).compare("YDIM") == 0) {
                    issl >> i;
                    dim[1] = i;
                }
                break;

            case 'Z':
                if (std::string(id, 0, strlen("ZDIM")).compare("ZDIM") == 0) {
                    issl >> i;
                    dim[2] = i;
                }
                break;

            case 'V':
                if (std::string(id, 0, strlen("VDIM")).compare("VDIM") == 0) {
                    issl >> i;
                    if (i > 1)
                    {
                        ncomponents = i;
                        this->SetNumberOfComponents(ncomponents);
                    }
                }
                else if (std::string(id, 0, strlen("VX")).compare("VX") == 0) {
                    issl >> r;
                    voxelsize[0] = r;
                }
                else if (std::string(id, 0, strlen("VY")).compare("VY") == 0) {
                    issl >> r;
                    voxelsize[1] = r;
                }
                else if (std::string(id, 0, strlen("VZ")).compare("VZ") == 0) {
                    issl >> r;
                    voxelsize[2] = r;
                }

                break;
        }

    }

    // Set principal image information
    if ((voxelsize[2] > 0.0) || (dim[2] > 1))
    {
        this->SetNumberOfDimensions(3);
    }

    this->SetDimensions(0, dim[0]);
    this->SetDimensions(1, dim[1]);

    this->SetSpacing(0, voxelsize[0]);
    this->SetSpacing(1, voxelsize[1]);

    this->SetOrigin(0, origin[0]);
    this->SetOrigin(1, origin[1]);

    if (this->GetNumberOfDimensions() > 2)
    {
        this->SetDimensions(2, dim[2]);
        this->SetSpacing(2, voxelsize[2]);
        this->SetOrigin(2, origin[2]);
    }

    // header parsing is done
    // TYPE is "unsigned fixed", "signed fixed", or "float"

    // Set image cosine directions
    if (this->GetNumberOfDimensions() == 2)
    {
        // Not yet implemented
        std::cerr << "[WARNING] Orientation matrix cannot be read for 2D images. Not yet implemented\n";
    }
    else if (this->GetNumberOfDimensions() == 3)
    {
        vnl_matrix <double> rotationMatrix(3, 3);
        this->GetRotationMatrixFromAngles(rx, ry, rz, rotationMatrix);

        vnl_vector<double> d(3);

        d[0] = std::abs(rotationMatrix(0, 0)) > 1e-10 ? rotationMatrix(0, 0) : 0;
        d[1] = std::abs(rotationMatrix(1, 0)) > 1e-10 ? rotationMatrix(1, 0) : 0;
        d[2] = std::abs(rotationMatrix(2, 0)) > 1e-10 ? rotationMatrix(2, 0) : 0;
        this->SetDirection(0, d);

        d[0] = std::abs(rotationMatrix(0, 1)) > 1e-10 ? rotationMatrix(0, 1) : 0;
        d[1] = std::abs(rotationMatrix(1, 1)) > 1e-10 ? rotationMatrix(1, 1) : 0;
        d[2] = std::abs(rotationMatrix(2, 1)) > 1e-10 ? rotationMatrix(2, 1) : 0;
        this->SetDirection(1, d);

        d[0] = std::abs(rotationMatrix(0, 2)) > 1e-10 ? rotationMatrix(0, 2) : 0;
        d[1] = std::abs(rotationMatrix(1, 2)) > 1e-10 ? rotationMatrix(1, 2) : 0;
        d[2] = std::abs(rotationMatrix(2, 2)) > 1e-10 ? rotationMatrix(2, 2) : 0;
        this->SetDirection(2, d);
    }

    if (std::string(type, 0, strlen("unsigned")).compare("unsigned") == 0) {
        if (pixsize == 8) this->SetComponentType(itk::IOComponentEnum::UCHAR);
        else if (pixsize == 16) this->SetComponentType(itk::IOComponentEnum::USHORT);
        else if (pixsize == 32) this->SetComponentType(itk::IOComponentEnum::UINT);
        else if (pixsize == 64) this->SetComponentType(itk::IOComponentEnum::ULONG);
        else {
            itk::ExceptionObject exception(__FILE__, __LINE__);
            exception.SetDescription("Pixel Size Not Recognized");
            throw exception;
        }
    }
    else if (std::string(type, 0, strlen("signed")).compare("signed") == 0) {
        if (pixsize == 8) this->SetComponentType(itk::IOComponentEnum::CHAR);
        else if (pixsize == 16) this->SetComponentType(itk::IOComponentEnum::SHORT);
        else if (pixsize == 32) this->SetComponentType(itk::IOComponentEnum::INT);
        else if (pixsize == 64) this->SetComponentType(itk::IOComponentEnum::LONG);
        else {
            itk::ExceptionObject exception(__FILE__, __LINE__);
            exception.SetDescription("Pixel Size Not Recognized");
            throw exception;
        }
    }
    else if (std::string(type, 0, strlen("float")).compare("float") == 0) {
        if (pixsize == 32) this->SetComponentType(itk::IOComponentEnum::FLOAT);
        else if (pixsize == 64) this->SetComponentType(itk::IOComponentEnum::DOUBLE);
        else {
            itk::ExceptionObject exception(__FILE__, __LINE__);
            exception.SetDescription("Pixel Size Not Recognized");
            throw exception;
        }
    }

    this->SetPixelType(itk::IOPixelEnum::SCALAR);

    if (0) {
        std::cout << "type " << type << std::endl;
        std::cout << "pixsize " << pixsize << std::endl;
        std::cout << "ComponentType " << this->GetComponentType() << std::endl;
        std::cout << "PixelType ";
        if (this->GetComponentTypeInfo() == typeid(unsigned char))
            std::cout << "UCHAR";

        else if (this->GetComponentTypeInfo() == typeid(unsigned short))
            std::cout << "USHORT";

        else if (this->GetComponentTypeInfo() == typeid(signed short))
            std::cout << "SHORT";

        else
            std::cout << "not handled in switch";
        std::cout << std::endl;
    }

    // I am not sure that this has to be done
    // GM
    if (0) {
        if (this->GetNumberOfComponents() == 3) {
            switch (this->GetComponentType()) {
                default:
                    break;
                case itk::IOComponentEnum::UCHAR:
                    this->SetPixelType(itk::IOPixelEnum::RGB);
                    this->SetNumberOfComponents(1);
                    break;
            }
        }
        else if (this->GetNumberOfComponents() == 4) {
            switch (this->GetComponentType()) {
                default:
                    break;
                case itk::IOComponentEnum::UCHAR:
                    this->SetPixelType(itk::IOPixelEnum::RGBA);
                    this->SetNumberOfComponents(1);
                    break;
            }
        }
    }

    return;
}



void InrimageImageIO::Read(void* buffer)
{
    unsigned int nread;

    if (0) {
        std::cerr << "* ComponentType " << this->GetComponentType() << std::endl;
        std::cerr << "PixelType ";
        if (this->GetComponentTypeInfo() == typeid(unsigned char))
            std::cerr << "UCHAR";
        else if (this->GetComponentTypeInfo() == typeid(unsigned short))
            std::cerr << "USHORT";
        else if (this->GetComponentTypeInfo() == typeid(signed short))
            std::cerr << "=SSHORT";
        else
            std::cerr << "not handled in switch";
        std::cerr << std::endl;
    }

    m_file = universal_GzOpen(m_FileName.c_str(), "rb");
    if (m_file == NULL) {
        itk::ExceptionObject exception(__FILE__, __LINE__);
        exception.SetDescription("Unable to open file");
        throw exception;
    }

    char buf_header[257];
    for (unsigned int i = 0; i < m_NumberBlocksInHeader; i++) {
        nread = ::gzread(m_file, buf_header, 256);
        if (nread != 256) {
            ::gzclose(m_file);
            itk::ExceptionObject exception(__FILE__, __LINE__);
            exception.SetDescription("Unable to skip header");
            throw exception;
        }
    }

    char * p = static_cast<char *>(buffer);
    nread = ::gzread(m_file, p, this->GetImageSizeInBytes() + 10000);
    if (nread != this->GetImageSizeInBytes()) {
        ::gzclose(m_file);
        itk::ExceptionObject exception(__FILE__, __LINE__);
        exception.SetDescription("Unable to read buffer");
        throw exception;
    }

    ::gzclose(m_file);

    SwapBytesIfNecessary(buffer, this->GetImageSizeInComponents());

}

bool InrimageImageIO::CanWriteFile(const char * FileNameToWrite)
{
    m_FileName = FileNameToWrite;

    if (m_FileName != "")
    {
        const std::string fileExt = GetExtension(m_FileName);
        if (strcmp(fileExt.c_str(), ".inr.gz") == 0 || strcmp(fileExt.c_str(), ".inr") == 0)
            return true;

    }

    return false;
}

void InrimageImageIO::WriteImageInformation(void)
{
    return;
}

void
InrimageImageIO
::Write(const void* buffer)
{

    // try to open the file
    bool isGz = true;
    std::string fileExt = GetExtension(m_FileName);
    // if it doesn't work, try by substracting extension .gz
    if (fileExt == (".inr.gz"))
    {
        m_file = universal_GzOpen(m_FileName.c_str(), "wb");
        if (m_file == NULL)
            throw itk::ExceptionObject(__FILE__, __LINE__, "Error in opening file for writing");
    }
    else
    {
        if (fileExt == (".inr"))
        {
            isGz = false;
            m_file = gzopen(m_FileName.c_str(), "wb");
            if (m_file == NULL)
                throw itk::ExceptionObject(__FILE__, __LINE__, "Error in opening file for writing");
        }
        else
            throw itk::ExceptionObject(__FILE__, __LINE__, "Unrecognized extension.");
    }


    std::string type = "signed fixed";
    char scale[20];
    switch (this->GetComponentType())
    {
        case itk::IOComponentEnum::UCHAR:
        case itk::IOComponentEnum::USHORT:
        case itk::IOComponentEnum::UINT:
        case itk::IOComponentEnum::ULONG:
            type = "unsigned fixed";
            sprintf(scale, "SCALE=2**0\n");
            break;

        case itk::IOComponentEnum::CHAR:
        case itk::IOComponentEnum::SHORT:
        case itk::IOComponentEnum::INT:
        case itk::IOComponentEnum::LONG:
            type = "signed fixed";
            sprintf(scale, "SCALE=2**0\n");
            break;

        case itk::IOComponentEnum::FLOAT:
        case itk::IOComponentEnum::DOUBLE:
            type = "float";
            scale[0] = '\0';
            break;

        default:
            throw itk::ExceptionObject(__FILE__, __LINE__, "Error: pixel type not recognized.");
    }


    int pixsize = 8;
    switch (this->GetComponentType())
    {
        case itk::IOComponentEnum::UCHAR:
        case itk::IOComponentEnum::CHAR:
            pixsize = 8;
            break;

        case itk::IOComponentEnum::USHORT:
        case itk::IOComponentEnum::SHORT:
            pixsize = 16;
            break;

        case itk::IOComponentEnum::UINT:
        case itk::IOComponentEnum::INT:
        case itk::IOComponentEnum::FLOAT:
            pixsize = 32;
            break;

        case itk::IOComponentEnum::ULONG:
        case itk::IOComponentEnum::LONG:
        case itk::IOComponentEnum::DOUBLE:
            pixsize = 64;
            break;

        default:
            pixsize = 0;
            break;
    }

    std::string endianness;
    if (this->GetByteOrder() == itk::IOByteOrderEnum::LittleEndian)
        endianness = "decm";
    else
        endianness = "sun";

    char buf[257];

    /* Compute rotation angles from cosine directions */
    std::vector <double> r(3,0.0);
    if (this->GetNumberOfDimensions() == 2)
    {
        // Not yet implemented
        std::cerr << "[WARNING] Orientation matrix cannot be read for 2D images. Not yet implemented\n";
    }
    else
    {
        std::vector<double> d;
        vnl_matrix <double> rotationMatrix(3,3);
        d = this->GetDirection(0);
        for (unsigned int i = 0;i < 3;++i)
        {
            // Direction vectors represent columns of the rotation matrix
            d = this->GetDirection(i);
            for (unsigned int j = 0;j < 3;++j)
                rotationMatrix(j,i) = d[j];
        }

        this->GetRotationAnglesFromMatrix(rotationMatrix, r);
    }

    /* Get spacings. If a spacing is null (pseudo 2D image), we artificially set it to 1 */
    double vx = this->GetSpacing(0);
    if (vx < 1e-9)
        vx = 1.0;

    double vy = this->GetSpacing(1);
    if (vy < 1e-9)
        vy = 1.0;

    double vz = this->GetSpacing(2);
    if (vz < 1e-9)
        vz = 1.0;

    /* write header information */
    sprintf(buf, "#INRIMAGE-4#{\nXDIM=%llu\nYDIM=%llu\nZDIM=%llu\nVDIM=%d\nTYPE=%s\nPIXSIZE=%i bits\n%sCPU=%s\nVX=%f\nVY=%f\nVZ=%f\nTX=%f\nTY=%f\nTZ=%f\nRX=%f\nRY=%f\nRZ=%f\n#GEOMETRY=CARTESIAN\n",
            this->GetDimensions(0), this->GetDimensions(1), this->GetDimensions(2), this->GetNumberOfComponents(),
            type.c_str(), pixsize, scale, endianness.c_str(),
            vx, vy, vz,
            this->GetOrigin(0), this->GetOrigin(1), this->GetOrigin(2),
            r[0], r[1], r[2]);

    int pos = strlen(buf);
    size_t npix = 0;
    // write the header:
    if (isGz)
        npix = ::gzwrite(m_file, (void *)buf, pos);
    else
        npix = fwrite((void *)buf, 1, pos, (FILE*)m_file);

    if (npix != strlen(buf))
    {
        if (isGz)
            ::gzclose(m_file);
        else
            fclose((FILE*)m_file);
        throw itk::ExceptionObject(__FILE__, __LINE__, "Error: bad number of bytes written.");
    }

    /* write end of header */
    pos = pos % 256;
    if (pos > 252) {
        for (int i = pos; i < 256; i++)
            if (isGz)
            {
                if (::gzwrite(m_file, (void *)("\n"), 1) != 1)
                {
                    ::gzclose(m_file);
                    throw itk::ExceptionObject(__FILE__, __LINE__, "Error: bad number of bytes written.");
                }
            }
            else
            {
                if (fwrite((void *)("\n"), 1, 1, (FILE*)m_file) != 1)
                {
                    fclose((FILE*)m_file);
                    throw itk::ExceptionObject(__FILE__, __LINE__, "Error: bad number of bytes written.");
                }
            }

        pos = 0;
    }
    buf[0] = '\0';
    for (int i = pos; i < 252; i++) strcat(buf, "\n");
    strcat(buf, "##}\n");

    if (isGz)
    {
        if (::gzwrite(m_file, (void *)buf, strlen(buf)) != (int)(strlen(buf)))
        {
            ::gzclose(m_file);
            throw itk::ExceptionObject(__FILE__, __LINE__, "Error: bad number of bytes written.");
        }
    }
    else
    {
        if (fwrite((void *)buf, 1, strlen(buf), (FILE*)m_file) != (size_t)(strlen(buf)))
        {
            fclose((FILE*)m_file);
            throw itk::ExceptionObject(__FILE__, __LINE__, "Error: bad number of bytes written.");
        }
    }



    // write the buffer:
    if (isGz)
        npix = ::gzwrite(m_file, (void *)buffer, this->GetImageSizeInBytes());
    else
        npix = fwrite((void *)buffer, 1, this->GetImageSizeInBytes(), (FILE*)m_file);
    if ((unsigned int)npix != this->GetImageSizeInBytes())
    {
        if (isGz)
            ::gzclose(m_file);
        else
            fclose((FILE*)m_file);
        throw itk::ExceptionObject(__FILE__, __LINE__, "Error: bad number of bytes written.");
    }


    if (isGz)
        ::gzclose(m_file);
    else
        fclose((FILE*)m_file);

}



/************************************************************
 *
 * private methods
 *
 ************************************************************/

void InrimageImageIO::SwapBytesIfNecessary(void* buffer, unsigned long numberOfPixels)
{
    if (ImageIOBase::GetByteOrder() == itk::IOByteOrderEnum::LittleEndian)
    {
        switch (m_ComponentType)
        {
            case itk::IOComponentEnum::CHAR:
                itk::ByteSwapper<char>::SwapRangeFromSystemToLittleEndian((char*)buffer,
                                                                     numberOfPixels);
                break;
            case itk::IOComponentEnum::UCHAR:
                itk::ByteSwapper<unsigned char>::SwapRangeFromSystemToLittleEndian
                        ((unsigned char*)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::SHORT:
                itk::ByteSwapper<short>::SwapRangeFromSystemToLittleEndian
                        ((short*)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::USHORT:
                itk::ByteSwapper<unsigned short>::SwapRangeFromSystemToLittleEndian
                        ((unsigned short*)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::INT:
                itk::ByteSwapper<int>::SwapRangeFromSystemToLittleEndian
                        ((int*)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::UINT:
                itk::ByteSwapper<unsigned int>::SwapRangeFromSystemToLittleEndian
                        ((unsigned int*)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::LONG:
                itk::ByteSwapper<long>::SwapRangeFromSystemToLittleEndian
                        ((long*)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::ULONG:
                itk::ByteSwapper<unsigned long>::SwapRangeFromSystemToLittleEndian
                        ((unsigned long*)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::FLOAT:
                itk::ByteSwapper<float>::SwapRangeFromSystemToLittleEndian((float*)buffer,
                                                                      numberOfPixels);
                break;
            case itk::IOComponentEnum::DOUBLE:
                itk::ByteSwapper<double>::SwapRangeFromSystemToLittleEndian
                        ((double*)buffer, numberOfPixels);
                break;
            default:
                itk::ExceptionObject exception(__FILE__, __LINE__);
                exception.SetDescription("Pixel Type Unknown");
                throw exception;
        }
    }
    else
    {
        switch (m_ComponentType)
        {
            case itk::IOComponentEnum::CHAR:
                itk::ByteSwapper<char>::SwapRangeFromSystemToBigEndian((char *)buffer,
                                                                  numberOfPixels);
                break;
            case itk::IOComponentEnum::UCHAR:
                itk::ByteSwapper<unsigned char>::SwapRangeFromSystemToBigEndian
                        ((unsigned char *)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::SHORT:
                itk::ByteSwapper<short>::SwapRangeFromSystemToBigEndian
                        ((short *)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::USHORT:
                itk::ByteSwapper<unsigned short>::SwapRangeFromSystemToBigEndian
                        ((unsigned short *)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::INT:
                itk::ByteSwapper<int>::SwapRangeFromSystemToBigEndian
                        ((int *)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::UINT:
                itk::ByteSwapper<unsigned int>::SwapRangeFromSystemToBigEndian
                        ((unsigned int *)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::LONG:
                itk::ByteSwapper<long>::SwapRangeFromSystemToBigEndian
                        ((long *)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::ULONG:
                itk::ByteSwapper<unsigned long>::SwapRangeFromSystemToBigEndian
                        ((unsigned long *)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::FLOAT:
                itk::ByteSwapper<float>::SwapRangeFromSystemToBigEndian
                        ((float *)buffer, numberOfPixels);
                break;
            case itk::IOComponentEnum::DOUBLE:
                itk::ByteSwapper<double>::SwapRangeFromSystemToBigEndian
                        ((double *)buffer, numberOfPixels);
                break;
            default:
                itk::ExceptionObject exception(__FILE__, __LINE__);
                exception.SetDescription("Pixel Type Unknown");
                throw exception;
        }
    }
}
