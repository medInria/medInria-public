#include "itkOutputWindow.h"
#include "itkObjectFactory.h"

namespace itk {

/**
* @class itk::LogForwarder
* @brief This class forwards all itk messages (Warning, Exceptions, TextOutputs) to the dtkLogger class
* You only need to instantiate this class once at the very beginning of you code (that uses itk)
*/
class LogForwarder : public OutputWindow
{
public:

  /** Standard class typedefs. */
  typedef LogForwarder              Self;
  typedef OutputWindow              Superclass;
  typedef SmartPointer<Self>        Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LogForwarder, OutputWindow);


  /** Send a string to display. */
  void DisplayText(const char*);

  /** Send a string as an error message to dtkLogger. */
  void DisplayErrorText(const char *t);

  /** Send a string as a warning message to dtkLogger. */
  void DisplayWarningText(const char *t);;

  /** Send a string as a message to dtkLogger.  */
  void DisplayGenericOutputText(const char *t);

  /** Send a string as a debug message to dtkLogger. */
  void DisplayDebugText(const char *t);;


protected:
  LogForwarder();
  ~LogForwarder();
  void PrintSelf(std::ostream& os, Indent indent) const;
};

} // end namespace itk
