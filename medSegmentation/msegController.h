#ifndef MSEGCONTROLLER_H
#define MSEGCONTROLLER_H

#include <QObject>

#include "msegPluginExport.h"

class medAbstractViewScene;

class dtkAbstractData;
class dtkAbstractView;

class QGraphicsSceneMouseEvent;
class medAbstractViewScene;

namespace mseg {
    class ControllerPrivate;
    class Configuration;
    class AlgorithmGeneric;
    class View;
}

namespace mseg {

class MEDVIEWSEGMENTATIONPLUGIN_EXPORT Controller : public QObject
{
    Q_OBJECT

public:
    Controller(Configuration * configuration);
    virtual ~Controller(void);

    static medAbstractViewScene * viewScene( dtkAbstractView * view );
    static dtkAbstractData * viewData( dtkAbstractView * view );

    void addViewEventFilter(View * filter);

    void run( mseg::AlgorithmGeneric* alg );
    bool canUndo() const;
    void undo();

    QString localizedNameForAlgorithm( const QString & algName ) const;

signals:
    void algorithmAdded( QString name );

public slots:
    void onViewAdded(dtkAbstractView* view);
    void onViewRemoved(dtkAbstractView* view);
    void onAlgorithmSelected( const QString & name );

    void onSuccess( QObject * sender );
    void onFailure( QObject * sender );
    void onCancelled( QObject * sender );

protected:
    void initializeAlgorithms();
    void activateViews( bool isActivate = true );

private:

    ControllerPrivate * d;

};

} // namespace mseg

#endif // MSEGCONTROLLER_H
