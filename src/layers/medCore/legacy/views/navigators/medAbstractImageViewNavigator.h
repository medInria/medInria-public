#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include <medAbstractLayeredViewNavigator.h>
#include <medImageViewEnum.h>

class medAbstractImageView;
class medCompositeParameterL;
class medAbstractVector3DParameterL;
class medTimeLineParameterL;

class medAbstractImageViewNavigatorPrivate;
class MEDCORE_EXPORT medAbstractImageViewNavigator : public medAbstractLayeredViewNavigator
{
    Q_OBJECT

public:
    medAbstractImageViewNavigator(medAbstractView* parent);
    virtual ~medAbstractImageViewNavigator();

public:
    virtual medImageView::Orientation orientation() const = 0;

    virtual medCompositeParameterL* cameraParameter();
    virtual medAbstractVector3DParameterL* positionBeingViewedParameter();
    virtual medTimeLineParameterL* timeLineParameter();

    virtual void toXMLNode(QDomDocument *doc, QDomElement *currentNode);

public slots:

    virtual void setOrientation(medImageView::Orientation orientation) = 0;
    virtual bool setRotationAngle(double angle) = 0;
    virtual bool setSlice(int slice) = 0;

    virtual void setCamera(QHash<QString,QVariant>) = 0;
    virtual void moveToPosition (const QVector3D &position) = 0;

    virtual void setCurrentTime (const double &time);

private slots:
    virtual void updateTimeLineParameter();

signals:
    void currentTimeChanged(const double &time);

private:
    medAbstractImageViewNavigatorPrivate *d;
};
