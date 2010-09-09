#include <qwidget>

class QTextEdit;
class QEvent;

/**
 * @class LoggerWidget
 * @author Michael Knopke
 * @brief Widget that receives logger messages and writes it to a textedit
 */
class LoggerWidget : public QWidget
{

  Q_OBJECT

  public:

    /**
     * Constructor, parent not needed
     */
    LoggerWidget(QWidget* parent=0);


protected:

    /**
     * Overriding the event processing to catch our message
     */
    virtual bool event(QEvent *e);

  private:

     /**
      * Holds a pointer to the text edit object that displays the log text
      */
     QTextEdit*    m_TextEdit;

};
