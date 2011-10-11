/*
 * msegSeedPointAnnotationData.h
 *
 *  Created on: 31 mai 2011 10:36:54
 *      Author: jstark
 */

#ifndef MSEGSEEDPOINTANNOTATIONDATA_H_
#define MSEGSEEDPOINTANNOTATIONDATA_H_

#include "msegPluginExport.h"

#include <medAnnotationData.h>

#include <QVector3D>

namespace mseg {

class SeedPointAnnotationData : public medAnnotationData {
    Q_OBJECT;
public:
    SeedPointAnnotationData();
    virtual ~SeedPointAnnotationData();

    QString description(void) const MED_OVERRIDE;

    qreal radiusScene() const { return m_radiusScene; }
    void setRadiusScene(qreal val);

    QColor selectedColor() const { return m_selectedColor; }
    void setSelectedColor(QColor val);

    QColor color() const { return m_color; }
    void setColor(QColor val);

    QVector3D centerWorld() const { return m_centerWorld; }
    void setCenterWorld(QVector3D val);

    static QString s_description();

    virtual void setSelected(bool value);
    bool isSelected() const { return m_isSelected; }

protected:
    qreal m_radiusScene;
    QColor m_selectedColor;
    QColor m_color;
    QVector3D m_centerWorld;
    bool m_isSelected;
};

}

#endif /* MSEGSEEDPOINTANNOTATIONDATA_H_ */

