#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "vtkDataMeshPluginExport.h"

#include <medAbstractImageViewInteractor.h>

#include <vtkPointSet.h>

class vtkDataMeshInteractorPrivate;

class medAbstractData;
class dtkAbstractView;
class vtkLookupTable;

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshInteractor: public medAbstractImageViewInteractor
{
    Q_OBJECT

public:
    vtkDataMeshInteractor(medAbstractView* parent);
    virtual ~vtkDataMeshInteractor();

    QString description() const override;
    QString identifier() const override;
    QStringList handled() const override;

    static bool registered();

    void setInputData(medAbstractData * data) override;

    bool edgeVisibility() const;
    QColor color() const;
    QString renderingType() const;
    QString attribute() const;
    QString lut() const;

    QWidget* buildLayerWidget() override;
    QWidget* buildToolBarWidget() override;
    QWidget* buildToolBoxWidget() override;
    QList<medAbstractParameterL*> linkableParameters() override;
    QList<medBoolParameterL*> mouseInteractionParameters() override;

public slots:
    void setOpacity(double value);
    void setWindowLevel (QHash<QString,QVariant>);
    void setColor(const QString &color);
    void setColor(QColor color);
    void setVisibility(bool visible);
    void setEdgeVisibility(bool visible);
    void setRenderingType(const QString &type);
    void setAttribute(const QString & attribute);

    void initWindowLevelParameters(double * range, int dataType);

    void setLut(const QString &lutName);

    void removeData() override;
    void setUpViewForThumbnail() override;
    virtual void moveToSlice  (int slice);

    void updateWidgets() override;

protected:
    void updatePipeline ();
    void setLut(vtkLookupTable * lut);

    void setupParameters();

private:
    static QStringList dataHandled();

private slots:
    void updateSlicingParam();
    void updateRange();

private:
    vtkDataMeshInteractorPrivate * d;
};
