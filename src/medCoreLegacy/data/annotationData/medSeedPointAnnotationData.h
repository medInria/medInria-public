/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <medAnnotationData.h>
#include <QtWidgets>
#include <QVector3D>

/** Defines a set of seed points that may be attached to a data
 * One of the points may be in a selected state.
 * A singleViewContainer size and color is set, the selected point has a different color.
 */
class MEDCORE_EXPORT medSeedPointAnnotationData : public medAnnotationData
{
    Q_OBJECT
    MED_DATA_INTERFACE("medSeedPointAnnotationData", "medSeedPointAnnotationData")

public:
    medSeedPointAnnotationData();
    virtual ~medSeedPointAnnotationData();

    //! Get / Set the radius of the seed points in pixels in the scene.
    qreal radiusScene() const { return m_radiusScene; }
    void setRadiusScene(qreal val);


    QColor selectedColor() const { return m_selectedColor; }
    void setSelectedColor(const QColor & val);

    QColor color() const { return m_color; }
    void setColor(QColor val);

    //! Get / Set the location of the ith seed point.
    //! The setter will increase the number of seeds if the given num is larger than the current number of seeds.
    QVector3D centerWorld(int num) const { return m_centerWorld[num]; }
    void setCenterWorld(int num, const QVector3D & val);

    //! Get / Set the the selected seed point.
    int selectedSeed(){ return m_selectedSeed; }
    virtual void setSelectedSeed(int num);

    //! Return true if given num is the selected seed.
    //! Return false if num is out of range.
    bool isSelected(int num) const { return m_selectedSeed == num; }

    //! Get / Set the number of seeds in the set.
    int getNumberOfSeeds() const { return m_centerWorld.size(); }
    void setNumberOfSeeds(int num);

    //! Remove the ith seed. The ids of the higher values shift down.
    void removeSeed(int i);


protected:
    QColor m_color;
    QColor m_selectedColor;
    qreal m_radiusScene;
    QVector<QVector3D> m_centerWorld;
    int m_selectedSeed;
};



