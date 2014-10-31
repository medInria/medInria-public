/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractImageViewInteractor.h>

#include <vtkPointSet.h>

#include <vtkDataMeshPluginExport.h>


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

    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual void setInputData(medAbstractData * data);

    bool edgeVisibility() const;
    QColor color() const;
    QString renderingType() const;
    QString attribute() const;
    QString lut() const;

    virtual QWidget* buildLayerWidget();
    virtual QWidget* buildToolBarWidget();
    virtual QWidget* buildToolBoxWidget();
    virtual QList<medAbstractParameter*> linkableParameters();
    virtual QList<medBoolParameter*> mouseInteractionParameters();

public slots:
    void setOpacity(double value);
    void setWindowLevel (QHash<QString,QVariant>);
    void setColor(const QString &color);
    void setColor(QColor color);
    void setVisibility(bool visible);
    void setEdgeVisibility(bool visible);
    void setRenderingType(const QString &type);
    void setAttribute(const QString & attribute);
    void setLut(const QString &lutName);

    virtual void removeData();
    virtual void setUpViewForThumbnail();
    virtual void moveToSlice  (int slice);


    virtual void updateWidgets();
    
protected:
    void updatePipeline ();
    void setLut(vtkLookupTable * lut);

    void setupParameters();

private:
    static QStringList dataHandled();

private slots:
    void updateSlicingParam();
    void updateRange();
    void showRangeWidgets(bool);

private:
    vtkDataMeshInteractorPrivate * d;
};


