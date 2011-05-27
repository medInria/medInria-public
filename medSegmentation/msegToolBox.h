#ifndef MSEGTOOLBOX_H
#define MSEGTOOLBOX_H

#include <medGui/medToolBox.h>

#include "msegPluginExport.h"

class medProgressionStack;

namespace mseg {

class Controller;

class ToolboxPrivate;

class MEDVIEWSEGMENTATIONPLUGIN_EXPORT Toolbox : public medToolBox
{
    Q_OBJECT;
public:
    Toolbox(QWidget *parent);
    ~Toolbox();

    medProgressionStack * progressionStack();

    void setController( mseg::Controller * controller);

signals:
    void algorithmSelected(const QString & algName);

public slots:
    void update (dtkAbstractView *view);

    void onAlgorithmSelected( int index);

    void onAlgorithmSelected( const QString & algName);
    void onAlgorithmAdded( const QString & algName);
public:
    void setAlgorithmParameterWidget( QWidget * widget );
protected:
    ToolboxPrivate *d;
};

} // namespace mseg

#endif // MSEGTOOLBOX_H

