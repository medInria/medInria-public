/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractViewFiberInteractor.h"

class medAbstractViewFiberInteractorPrivate
{
public:
    QMap<QString, double> meanFAList;
    QMap<QString, double> minFAList;
    QMap<QString, double> maxFAList;
    QMap<QString, double> varFAList;

    QMap<QString, double> meanADCList;
    QMap<QString, double> minADCList;
    QMap<QString, double> maxADCList;
    QMap<QString, double> varADCList;

    QMap<QString, double> meanLengthList;
    QMap<QString, double> minLengthList;
    QMap<QString, double> maxLengthList;
    QMap<QString, double> varLengthList;
};

medAbstractViewFiberInteractor::medAbstractViewFiberInteractor(): d2(new medAbstractViewFiberInteractorPrivate)
{}

medAbstractViewFiberInteractor::~medAbstractViewFiberInteractor()
{
    delete d2;
    d2 = NULL;
}

void medAbstractViewFiberInteractor::setROI(dtkAbstractData *data)
{
}

void medAbstractViewFiberInteractor::setRoiBoolean(int roi, int meaning)
{}

int medAbstractViewFiberInteractor::roiBoolean(int roi)
{
    return 2;
}

void medAbstractViewFiberInteractor::bundleFAStatistics(const QString &name,
                                                        double &mean,
                                                        double &min,
                                                        double &max,
                                                        double &var)
{
    if (!d2->meanFAList.contains(name)) {
        this->computeBundleFAStatistics(name, mean, min, max, var);
        d2->meanFAList[name] = mean;
        d2->minFAList[name] = min;
        d2->maxFAList[name] = max;
        d2->varFAList[name] = var;
    }
    else {
        mean = d2->meanFAList[name];
        min  = d2->minFAList[name];
        max  = d2->maxFAList[name];
        var  = d2->varFAList[name];
    }
}

void medAbstractViewFiberInteractor::bundleADCStatistics(const QString &name,
                                                         double &mean,
                                                         double &min,
                                                         double &max,
                                                         double &var)
{
    if (!d2->meanADCList.contains(name)) {
        this->computeBundleADCStatistics(name, mean, min, max, var);
        d2->meanADCList[name] = mean;
        d2->minADCList[name] = min;
        d2->maxADCList[name] = max;
        d2->varADCList[name] = var;
    }
    else {
        mean = d2->meanADCList[name];
        min  = d2->minADCList[name];
        max  = d2->maxADCList[name];
        var  = d2->varADCList[name];
    }
}


void medAbstractViewFiberInteractor::bundleLengthStatistics(const QString &name,
                                                        double &mean,
                                                        double &min,
                                                        double &max,
                                                        double &var)
{
    if (!d2->meanLengthList.contains(name)) {
        this->computeBundleLengthStatistics(name, mean, min, max, var);
        d2->meanLengthList[name] = mean;
        d2->minLengthList[name] = min;
        d2->maxLengthList[name] = max;
        d2->varLengthList[name] = var;
    }
    else {
        mean = d2->meanLengthList[name];
        min  = d2->minLengthList[name];
        max  = d2->maxLengthList[name];
        var  = d2->varLengthList[name];
    }
}

void medAbstractViewFiberInteractor::setBundleVisibility(const QString &name, bool visibility)
{
}

bool medAbstractViewFiberInteractor::bundleVisibility(const QString &name) const
{
    return false;
}

void medAbstractViewFiberInteractor::setBundleVisibility(bool visibility)
{
}

void medAbstractViewFiberInteractor::computeBundleFAStatistics(const QString &name,
                                                               double &mean,
                                                               double &min,
                                                               double &max,
                                                               double &var)
{
}

void medAbstractViewFiberInteractor::computeBundleADCStatistics(const QString &name,
                                                                double &mean,
                                                                double &min,
                                                                double &max,
                                                                double &var)
{
}

void medAbstractViewFiberInteractor::computeBundleLengthStatistics(const QString &name,
                                                                   double &mean,
                                                                   double &min,
                                                                   double &max,
                                                                   double &var)
{
}

void medAbstractViewFiberInteractor::clearStatistics(void)
{
    d2->meanFAList.clear();
    d2->minFAList.clear();
    d2->maxFAList.clear();
    d2->varFAList.clear();

    d2->meanADCList.clear();
    d2->minADCList.clear();
    d2->maxADCList.clear();
    d2->varADCList.clear();

    d2->meanLengthList.clear();
    d2->minLengthList.clear();
    d2->maxLengthList.clear();
    d2->varLengthList.clear();
}
