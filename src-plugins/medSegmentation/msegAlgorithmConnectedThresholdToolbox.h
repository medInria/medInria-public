/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medSegmentationAbstractToolBox.h"

#include "msegPluginExport.h"

#include "msegAlgorithmGeneric.h"

#include <dtkCore/dtkAbstractData.h>

#include <medDataIndex.h>
#include <medViewEventFilter.h>

#include <QVector3D>
#include <QTextEdit>

#include <vector>

class medAbstractData;
class medAbstractView;
class medAnnotationData;

class dtkAbstractProcessFactory;
class medSeedPointAnnotationData;

namespace mseg {

//! Custom segmentation toolbox to control a connected threshold segmentation.
class MEDVIEWSEGMENTATIONPLUGIN_EXPORT AlgorithmConnectedThresholdToolbox : public medSegmentationAbstractToolBox
{
    Q_OBJECT;
public:
    AlgorithmConnectedThresholdToolbox( QWidget *parent );
    ~AlgorithmConnectedThresholdToolbox();

    //! Creator, using new
    static medSegmentationAbstractToolBox * createInstance( QWidget *parent );

    //! Override dtkAbstractObject
    QString description() const { return s_description(); }
    QString identifier() const { return s_identifier(); }

    //! description
    static QString s_description();

    /** Get name to use for this when registering with a factory.*/
    static QString s_identifier();

    //! Get a human readable name for this widget.
    /** \param trObj : Provide an object for the tr() function. If NULL qApp will be used. */
    static QString s_name(const QObject * trObj =  NULL);

public slots:
    void onViewMousePress(medAbstractView *view, const QVector3D &vec);

protected slots:
    //! Callback when add seed point button is pressed.
    void onAddSeedPointPressed();
    //! Callback when remove seed point button is pressed.
    void onRemoveSeedPointPressed();
    //! Callback when apply button is pressed.
    void onApplyButtonPressed();

    void onSeedPointTableSelectionChanged();

    void onDataModified(medAbstractData* attached);

protected:
    void addSeedPoint( medAbstractView *view, const QVector3D &vec );
    void setData( dtkAbstractData *data );

    // update with seed point data.
    void updateTableRow(int row);
private:
    typedef dtkSmartPointer<medSeedPointAnnotationData> SeedPoint;

    QDoubleSpinBox *m_lowThresh;
    QDoubleSpinBox *m_highThresh;
    QTableWidget *m_seedPointTable;
    QPushButton *m_addSeedPointButton;
    QPushButton *m_removeSeedPointButton;
    QPushButton *m_applyButton;
    QTextEdit *m_dataText;

    SeedPoint m_seedPoints;
    dtkSmartPointer< medViewEventFilter > m_viewFilter;

    enum ViewState { ViewState_None, ViewState_PickingSeedPoint };
    ViewState m_viewState;

    dtkSmartPointer<medAbstractData> m_data;
    QString m_noDataText;

};

} // namespace mseg


