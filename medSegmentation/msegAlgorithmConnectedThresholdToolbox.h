#ifndef MSEGALGORITHMCONNECTEDTHRESHOLDTOOLBOX_H
#define MSEGALGORITHMCONNECTEDTHRESHOLDTOOLBOX_H

#include "medToolBoxSegmentationCustom.h"

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


class MEDVIEWSEGMENTATIONPLUGIN_EXPORT AlgorithmConnectedThresholdToolbox : public medToolBoxSegmentationCustom
{
    Q_OBJECT;
public:
    AlgorithmConnectedThresholdToolbox( QWidget *parent );
    ~AlgorithmConnectedThresholdToolbox();

    static medToolBoxSegmentationCustom * createInstance( QWidget *parent );

    static QString s_description();

    /** Get name to use for this when registering with a factory.*/
    static QString s_identifier();

    //! Get a human readable name for this widget.
    /** \param trObj : Provide an object for the tr() function. If NULL qApp will be used. */
    static QString s_name(const QObject * trObj =  NULL);

public slots:
    void onAddSeedPointPressed();
    void onRemoveSeedPointPressed();
    void onApplyButtonPressed();

    void onViewMousePress(medAbstractView *view, const QVector3D &vec);

    void onSeedPointTableSelectionChanged();

    void onDataModified(medAnnotationData* attached);

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

#endif // MSEGALGORITHMCONNECTEDTHRESHOLDTOOLBOX_H
