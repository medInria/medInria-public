#ifndef MSEGALGORITHMCONNECTEDTHRESHOLDTOOLBOX_H
#define MSEGALGORITHMCONNECTEDTHRESHOLDTOOLBOX_H

#include "medToolBoxSegmentationCustom.h"

#include "msegPluginExport.h"

#include "msegAlgorithmGeneric.h"
#include "msegViewFilter.h"

#include <dtkCore/dtkAbstractData.h>

#include <medCore/medDataIndex.h>

#include <QVector3D>
#include <QTextEdit>

class medAbstractData;
class medAbstractView;

class dtkAbstractProcessFactory;

namespace mseg {
    class SeedPointAnnotationData;


class MEDVIEWSEGMENTATIONPLUGIN_EXPORT AlgorithmConnectedThresholdToolbox : public medToolBoxSegmentationCustom
{
    Q_OBJECT;
public:
    AlgorithmConnectedThresholdToolbox( QWidget *parent );
    ~AlgorithmConnectedThresholdToolbox();

    static medToolBoxSegmentationCustom * createInstance( QWidget *parent );

    /** Get name to use for this when registering with a factory.*/
    static QString s_description();

    //! Get a human readable name for this widget.
    /** \param trObj : Provide an object for the tr() function. If NULL qApp will be used. */
    static QString s_localizedName(const QObject * trObj =  NULL);
public slots:
    void onAddSeedPointPressed();
    void onRemoveSeedPointPressed();
    void onApplyButtonPressed();

    void onViewMousePress(medAbstractView *view, const QVector3D &vec);

    void onSeedPointTableSelectionChanged();

protected:
    void addSeedPoint( medAbstractView *view, const QVector3D &vec );
    void setData( dtkAbstractData *data );

private:
    struct SeedPoint { 
        dtkSmartPointer<SeedPointAnnotationData> annotationData;
    };

    QDoubleSpinBox *m_lowThresh;
    QDoubleSpinBox *m_highThresh;
    QTableWidget *m_seedPointTable;
    QPushButton *m_addSeedPointButton;
    QPushButton *m_removeSeedPointButton;
    QPushButton *m_applyButton;
    QTextEdit *m_dataText;

    QVector< SeedPoint > m_seedPoints;
    dtkSmartPointer< ViewFilter > m_viewFilter;

    enum ViewState { ViewState_None, ViewState_PickingSeedPoint };
    ViewState m_viewState;

    dtkSmartPointer<medAbstractData> m_data;
    QString m_noDataText;

};

} // namespace mseg

#endif // MSEGALGORITHMCONNECTEDTHRESHOLDTOOLBOX_H
