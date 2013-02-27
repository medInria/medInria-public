#ifndef MEDVIEWEVENTFILTER_H
#define MEDVIEWEVENTFILTER_H

// /////////////////////////////////////////////////////////////////
// Generated by dtkPluginGenerator
// /////////////////////////////////////////////////////////////////


#include <dtkCore/dtkAbstractObject.h>

#include "medCoreExport.h"

//Forward declarations
class medAbstractView;
class medAbstractViewScene;
class medSegmentationSelectorToolBox;

/** class medViewEventFilter
 * This class can be added as an EventFilter on a medAbstractView.
 * The class allows the retreival of the mouse clicks, including where the mouse was clicked in the coordinates defined by the view.
 *
 * (see QObject::eventFilter & QObject::installEventFilter)
 */
class MEDCORE_EXPORT medViewEventFilter : public dtkAbstractObject
{
    Q_OBJECT;

public:
             medViewEventFilter(dtkAbstractObject * parent = NULL);
    virtual ~medViewEventFilter(void);

    //! Implement dtkAbstractObject.
    virtual QString description(void) const;
    virtual QString identifier(void) const;

    //! Installs the eventFilter(this) in the given view
    void installOnView(medAbstractView * view);
    //! Remove the eventFilter(this) in the given view
    void removeFromView(medAbstractView * view);
    //! Remove this from all views it has been installed on.
    void removeFromAllViews();

protected:
    //! Override QObject
    virtual bool eventFilter(QObject *obj, QEvent *event);

    /** Event handlers, derived classes should override those they need.
     *  \return     true if the event was processed and should not be passed to the next handler.
     *  The default implementations do nothing, returning false. */
    virtual bool mousePressEvent( medAbstractView *view, QMouseEvent *mouseEvent );
    virtual bool mouseReleaseEvent( medAbstractView *view, QMouseEvent *mouseEvent );
    virtual bool mouseMoveEvent( medAbstractView *view, QMouseEvent *mouseEvent );

    /** Event handlers, derived classes should override those they need.
     *  \return     true if the event was processed and should not be passed to the next handler.
     *  The default implementations do nothing, returning false. */
    virtual bool mousePressEvent( medAbstractView *view, QGraphicsSceneMouseEvent *mouseEvent );
    virtual bool mouseReleaseEvent( medAbstractView *view, QGraphicsSceneMouseEvent *mouseEvent );
    virtual bool mouseMoveEvent( medAbstractView *view, QGraphicsSceneMouseEvent *mouseEvent );


    // Which object to actually filter given the input view.
    static QObject * objectToFilter( medAbstractView * view );

protected slots:
    void onViewDestroyed(QObject*);

private:
    static QString s_description();

    medSegmentationSelectorToolBox * m_segmentationController;
    QSet<medAbstractView *> m_views;
    typedef QHash<QObject *, medAbstractView *> FilterObjToViewType;
    FilterObjToViewType m_filterObjToView;
};

#endif // MEDVIEWEVENTFILTER_H
