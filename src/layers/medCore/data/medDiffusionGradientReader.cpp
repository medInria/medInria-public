#include "medDiffusionGradientReader.h"

#include <fstream>
#include <sstream>

class medDiffusionGradientReaderPrivate
{
public:
    medDiffusionGradientReader::VectorType bValues;
    medDiffusionGradientReader::GradientsVectorType gradients;
};

medDiffusionGradientReader::medDiffusionGradientReader()
    : d(new medDiffusionGradientReaderPrivate)
{
    d->bValues.clear();
    d->gradients.clear();
}

medDiffusionGradientReader::~medDiffusionGradientReader()
{
}

void medDiffusionGradientReader::read(QString gradientFileName, QString bvaluesFileName)
{
    this->readBValues(bvaluesFileName);
    this->readGradients(gradientFileName);
}

void medDiffusionGradientReader::readBValues(QString bvaluesFileName)
{
    std::ifstream bvalFile(bvaluesFileName.toStdString());

    if (!bvalFile.is_open())
        return;

    d->bValues.clear();
    if (bvaluesFileName.endsWith("bvals") || bvaluesFileName.endsWith("bval")) // bval file
    {
        char tmpStr[8192];
        bvalFile.getline(tmpStr,8192);
        QString workStr(tmpStr);
        workStr = workStr.trimmed();

        std::istringstream iss(workStr.toStdString());
        std::string shortStr;
        do
        {
            iss >> shortStr;
            QString convertStr(shortStr.c_str());
            d->bValues.push_back(convertStr.toDouble());
        }
        while (!iss.eof());
    }
    else // text file
    {
        while (!bvalFile.eof())
        {
            char tmpStr[2048];
            bvalFile.getline(tmpStr,2048);

            if (strcmp(tmpStr,"") == 0)
                continue;

            QString shortStr = tmpStr;
            d->bValues.push_back(shortStr.toDouble());
        }
    }

    bvalFile.close();
}

void medDiffusionGradientReader::readGradients(QString gradientFileName)
{
    if (gradientFileName.endsWith("bvec") || gradientFileName.endsWith("bvecs"))
    {
        this->readBVecFile(gradientFileName);
        return;
    }

    // Read as text file
    std::ifstream gradFile(gradientFileName.toStdString());
    if (!gradFile.is_open())
        return;

    unsigned int numGrads;
    gradFile >> numGrads;

    VectorType gradTmp(3,0);
    d->gradients.clear();

    while (!gradFile.eof())
    {
        char tmpStr[2048];
        gradFile.getline(tmpStr,2048);

        if (strcmp(tmpStr,"") == 0)
            continue;

        float a0, a1, a2;
        sscanf(tmpStr,"%f %f %f",&a0,&a1,&a2);
        gradTmp[0] = a0;
        gradTmp[1] = a1;
        gradTmp[2] = a2;

        d->gradients.push_back(gradTmp);
    }

    gradFile.close();
}

void medDiffusionGradientReader::readBVecFile(QString gradientFileName)
{
    std::ifstream gradFile(gradientFileName.toStdString());

    d->gradients.clear();
    std::vector < std::vector <double> > trVecs(3);

    for (unsigned int i = 0;i < 3;++i)
    {
        if (gradFile.eof())
            return;

        char tmpStr[8192];
        tmpStr[0] = '\0';

        while((strcmp(tmpStr,"") == 0)&&(!gradFile.eof()))
            gradFile.getline(tmpStr,8192);

        QString workStr(tmpStr);
        workStr = workStr.trimmed();

        std::istringstream iss(workStr.toStdString());
        std::string shortStr;
        do
        {
            iss >> shortStr;
            QString convertStr(shortStr.c_str());
            trVecs[i].push_back(convertStr.toDouble());
        }
        while (!iss.eof());
    }

    unsigned int numDirections = static_cast<unsigned int>(trVecs[0].size());

    d->gradients.resize(numDirections);
    for (unsigned int i = 0;i < numDirections;++i)
    {
        d->gradients[i].resize(3);
        for (unsigned int j = 0;j < 3;++j)
            d->gradients[i][j] = trVecs[j][i];
    }

    gradFile.close();
}

medDiffusionGradientReader::GradientsVectorType &medDiffusionGradientReader::gradients()
{
    return d->gradients;
}

medDiffusionGradientReader::VectorType &medDiffusionGradientReader::bvalues()
{
    return d->bValues;
}
