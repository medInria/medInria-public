//
// Created by Julien Castelneau on 06/04/2021.
//

#ifndef MEDINRIA_UROLOGYVIEWEVENT_H
#define MEDINRIA_UROLOGYVIEWEVENT_H


#include "baseViewEvent.h"
#include <urologyLabelToolBox.h>

class urologyViewEvent
        : public baseViewEvent
{

public:
    explicit urologyViewEvent(medAbstractImageView *iView, polygonRoiToolBox *toolBox):
                                baseViewEvent(iView, toolBox), uLabelToolBox(nullptr) {}
    ~urologyViewEvent() override;

    void initialize(medToolBox *medTb, QString &seriesName) override;

    void activateContour(double *mousePosition) override;

    QLineEdit *changeManagerName(polygonLabel* closestManager, QMenu *mainMenu) override;

    void deleteLabel(polygonLabel *manager) override;

    void switchContourColor(double *mousePosition) override;

    void updateLabelToolBoxState(QString &name) override;

    void loadContours(QVector<medTagContours> &tagContoursSet) override;

private:
    QMenu *createScoreMenu(polygonLabel *pLabel) override;
    QAction *createScoreAction(polygonLabel *manager, const QString &score, const QColor &color);
    int speciality() override { return 1;};
    void setScoreNameAndColor(polygonLabel *label, QString &name) override;
    QString createMaskDescription(polygonLabel *label) override;

    QList<QPair<QString, QColor>> pirads;

    urologyLabelToolBox *uLabelToolBox{};
};


#endif //MEDINRIA_UROLOGYVIEWEVENT_H
