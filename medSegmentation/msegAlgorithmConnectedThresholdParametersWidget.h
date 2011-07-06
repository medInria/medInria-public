#ifndef MSEGALGORITHMCONNECTEDTHRESHOLDPARAMETERSWIDGET_H
#define MSEGALGORITHMCONNECTEDTHRESHOLDPARAMETERSWIDGET_H


#include "msegPluginExport.h"

#include "msegAlgorithmParametersWidget.h"
#include "msegAlgorithmGeneric.h"
#include "msegView.h"

#include <dtkCore/dtkAbstractData.h>

#include <medCore/medDataIndex.h>

#include <QVector3D>
#include <QTextEdit>

class medAbstractView;

class dtkAbstractData;
class dtkAbstractProcessFactory;

namespace mseg {
    class SeedPointAnnotationData;


class MEDVIEWSEGMENTATIONPLUGIN_EXPORT AlgorithmConnectedThresholdParametersWidget : public AlgorithmParametersWidget
{
    Q_OBJECT;
public:
    AlgorithmConnectedThresholdParametersWidget( Controller *controller, QWidget *parent );
    ~AlgorithmConnectedThresholdParametersWidget();

    static AlgorithmParametersWidget * createAlgorithmParametersWidget( Controller *controller, QWidget *parent );

public slots:
    void onAddSeedPointPressed();
    void onRemoveSeedPointPressed();
    void onApplyButtonPressed();

    void onViewMousePress(medAbstractView *view, const QVector3D &vec);

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
    dtkSmartPointer< View > m_viewFilter;
    dtkSmartPointer< dtkAbstractData > m_inputData;

    enum ViewState { ViewState_None, ViewState_PickingSeedPoint };
    ViewState m_viewState;

    dtkSmartPointer<dtkAbstractData> m_data;
    QString m_noDataText;

};

} // namespace mseg

#endif // MSEGALGORITHMCONNECTEDTHRESHOLDPARAMETERSWIDGET_H
