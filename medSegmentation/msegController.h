#ifndef MSEGCONTROLLER_H
#define MSEGCONTROLLER_H

#include <QObject>

#include "msegPluginExport.h"

class medAbstractViewScene;

class dtkAbstractData;
class dtkAbstractView;

class QGraphicsSceneMouseEvent;
class medAbstractViewScene;
class medAnnotationData;
class medAnnotationFactory;

namespace mseg {
    class ControllerPrivate;
    class Configuration;
    class AlgorithmGeneric;
    class View;
}

namespace mseg {

class MEDVIEWSEGMENTATIONPLUGIN_EXPORT Controller : public QObject
{
    Q_OBJECT;

public:
    Controller(Configuration * configuration);
    virtual ~Controller(void);

    static medAbstractViewScene * viewScene( dtkAbstractView * view );
    static dtkAbstractData * viewData( dtkAbstractView * view );

    void addViewEventFilter(View * filter);
    void removeViewEventFilter(View * filter);

    void run( mseg::AlgorithmGeneric* alg );
    bool canUndo() const;
    void undo();

    QString localizedNameForAlgorithm( const QString & algName ) const;

    void addAnnotation( medAnnotationData * annotation );
    void removeAnnotation( medAnnotationData * annotation );

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
    void initializeAnnotations();
    void activateViews( bool isActivate = true );
    medAnnotationFactory * annotationFactory();

    bool addAnnotationToScene( medAbstractViewScene * vscene, medAnnotationData * annotation );
private:

    ControllerPrivate * d;

};

} // namespace mseg

#endif // MSEGCONTROLLER_H
