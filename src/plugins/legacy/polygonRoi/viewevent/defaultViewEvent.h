/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef MEDINRIA_DEFAULTVIEWEVENT_H
#define MEDINRIA_DEFAULTVIEWEVENT_H


#include "baseViewEvent.h"

class defaultViewEvent
        : public baseViewEvent
{

public:
    explicit defaultViewEvent(medAbstractImageView *iView, polygonRoiToolBox *toolBox) :
                                baseViewEvent(iView, toolBox), dLabelToolBox(nullptr) {}
    ~defaultViewEvent() override;

    void initialize(medToolBox *medTb, QString &seriesName) override;

    void activateContour(double *mousePosition) override;
    QLineEdit *changeManagerName(polygonLabel* closestManager, QMenu *mainMenu) override;

    void deleteLabel(polygonLabel *manager) override;

    void updateLabelToolBoxState(QString &name) override;

    void switchContourColor(double *mousePosition) override {};

    void loadContours(QVector<medTagContours> &tagContoursSet) override;
private:
    QMenu *createScoreMenu(polygonLabel *pLabel) override {return nullptr;};
    int speciality() override { return 0;};
    defaultLabelToolBox *dLabelToolBox;
};


#endif //MEDINRIA_DEFAULTVIEWEVENT_H
