#include <QEvent>

/**
 * @class LoggerMessageEvent
 * @author Michael Knopke
 * @brief Custom message event to exchange log messages threadsafe
 */
class LoggerMessageEvent: public QEvent 
{
public:

    LoggerMessageEvent(QString message) : QEvent(LoggerMessageEvent::custom_type()), m_message(message)
    {
        setAccepted(false);
    }
 
    static QEvent::Type custom_type() 
    {
        static int type = QEvent::registerEventType();
        return (QEvent::Type) type;
    }
 
    QString getMessage() 
    {
        return m_message;
    }
 
 
private:
    QString m_message;

};
