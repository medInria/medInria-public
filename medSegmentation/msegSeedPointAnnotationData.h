/*
 * msegSeedPointAnnotationData.h
 *
 *  Created on: 31 mai 2011 10:36:54
 *      Author: jstark
 */

#ifndef MSEGSEEDPOINTANNOTATIONDATA_H_
#define MSEGSEEDPOINTANNOTATIONDATA_H_

#include <medAnnotationData.h>

#include <QVector3D>

namespace mseg {

class SeedPointAnnotationData : public medAnnotationData {
    Q_OBJECT;
public:
    SeedPointAnnotationData();
    virtual ~SeedPointAnnotationData();

    qreal radiusScene() const { return m_radiusScene; }
    void setRadiusScene(qreal val);
    QColor color() const { return m_color; }
    void setColor(QColor val);

    QVector3D centerWorld() const { return m_centerWorld; }
    void setCenterWorld(QVector3D val);

    static QString s_description();

protected:
    qreal m_radiusScene;
    QColor m_color;
    QVector3D m_centerWorld;

};

}

#endif /* MSEGSEEDPOINTANNOTATIONDATA_H_ */

