#ifndef MSEGALGORITHMCONNECTEDTHRESHOLDPARAMETERSWIDGET_H
#define MSEGALGORITHMCONNECTEDTHRESHOLDPARAMETERSWIDGET_H


#include "msegPluginExport.h"
#include "msegAlgorithmGeneric.h"
#include "msegView.h"

#include <dtkCore/dtkAbstractData.h>

#include <QVector3D>
#include <QTextEdit>

class medAbstractView;

class dtkAbstractData;
class dtkAbstractProcessFactory;

namespace mseg {

class AlgorithmParametersWidget : public QWidget {
public:
    AlgorithmParametersWidget( Controller * controller, QWidget * parent ) : 
      QWidget(parent), 
          m_controller(controller) {}
    virtual ~AlgorithmParametersWidget() {}

protected:
    Controller * controller() { return m_controller; }

private:
    Controller * m_controller;
};

class MEDVIEWSEGMENTATIONPLUGIN_EXPORT AlgorithmConnectedThresholdParametersWidget : public AlgorithmParametersWidget
{
    Q_OBJECT;
public:
    AlgorithmConnectedThresholdParametersWidget( Controller *controller, QWidget *parent );
    ~AlgorithmConnectedThresholdParametersWidget();

public slots:
    void onAddSeedPointPressed();
    void onRemoveSeedPointPressed();
    void onApplyButtonPressed();

    void onViewMousePress(medAbstractView *view, const QVector3D &vec);

private:
    struct SeedPoint { QVector3D vec; };

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
};

} // namespace mseg

#endif // MSEGALGORITHMCONNECTEDTHRESHOLDPARAMETERSWIDGET_H
