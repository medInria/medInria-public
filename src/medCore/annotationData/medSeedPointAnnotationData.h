/*
 * msegSeedPointAnnotationData.h
 *
 *  Created on: 31 mai 2011 10:36:54
 *      Author: jstark
 */

#ifndef MSEGSEEDPOINTANNOTATIONDATA_H_
#define MSEGSEEDPOINTANNOTATIONDATA_H_

#include "medCoreExport.h"

#include <medAnnotationData.h>

#include <QVector3D>

class MEDCORE_EXPORT medSeedPointAnnotationData : public medAnnotationData {
    Q_OBJECT;
public:
    medSeedPointAnnotationData();
    virtual ~medSeedPointAnnotationData();

    QString description(void) const;
    QString identifier(void) const;

    qreal radiusScene() const { return m_radiusScene; }
    void setRadiusScene(qreal val);

    QColor selectedColor() const { return m_selectedColor; }
    void setSelectedColor(const QColor & val);

    QColor color() const { return m_color; }
    void setColor(QColor val);

    // The setter will increase the number of seeds of hte given num is larger than the current number of seeds.
    QVector3D centerWorld(int num) const { return m_centerWorld[num]; }
    void setCenterWorld(int num, const QVector3D & val);

    int selectedSeed(){ return m_selectedSeed; }
    virtual void setSelectedSeed(int num);

    bool isSelected(int num) const { return m_selectedSeed == num; }

    int getNumberOfSeeds() const { return m_centerWorld.size(); }
    void setNumberOfSeeds(int num);

    void removeSeed(int i);
// static methods
    static QString s_description();
    static QString s_identifier();

protected:
    qreal m_radiusScene;
    QColor m_selectedColor;
    QColor m_color;
    QVector<QVector3D> m_centerWorld;
    int m_selectedSeed;
};

#endif /* MSEGSEEDPOINTANNOTATIONDATA_H_ */

