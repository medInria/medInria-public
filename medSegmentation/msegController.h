#ifndef MSEGCONTROLLER_H
#define MSEGCONTROLLER_H

#include <QObject>

#include "msegPluginExport.h"

class medAbstractViewScene;

class dtkAbstractData;
class dtkAbstractView;

class QGraphicsSceneMouseEvent;

namespace mseg {
    class ControllerPrivate;
    class Configuration;
    class AlgorithmGeneric;
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

    void beginAddSeedPoint();
    bool mousePressEvent( QObject *obj, QGraphicsSceneMouseEvent *mouseEvent);
    bool mouseMoveEvent( QObject *obj, QGraphicsSceneMouseEvent *mouseEvent);
    bool mouseReleaseEvent( QObject *obj, QGraphicsSceneMouseEvent *mouseEvent);

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
private:

    ControllerPrivate * d;

};

} // namespace mseg

#endif // MSEGCONTROLLER_H
