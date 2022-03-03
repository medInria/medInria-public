/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

// medInria
#include "defaultLabelToolBox.h"
#include <medLabelProperty.h>

class urologyLabelToolBox
        : public defaultLabelToolBox
{
Q_OBJECT

MED_TOOLBOX_INTERFACE("Contours Management",
                      "urologyLabelToolBox",
                      <<"urologyLabelToolBox")


public:
    explicit urologyLabelToolBox(QWidget *parent = nullptr);
    void initialize(QString &seriesName) override;
    static bool registered();
    void clean() override;
    void rename(int row, QString &newName) override;
    void updateItem(medLabelProperty &info) override;
    void updateScoreState(int position, bool scoreState, bool checked,
                          QString &firstName, QString &secondName);


    int getContourPositionAndColor(QString &name, bool scoreState, QColor &color, QString &scoreName);

    bool hasScoreOnPosition(int row, QString &score, QColor &color);

protected slots:

    void onPlusClicked() override;
    void onMinusClicked() override;

private:
    static bool isTarget() ;
    static void parseJSONFile(QStringList &labelNames, QList<bool> &scores);

    QList<QColor> targetColors;

    static void itemSpecialization(QListWidgetItem *item, bool scoreState, bool checkState = false) ;

};
