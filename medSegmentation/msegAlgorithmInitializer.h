/*
 * msegAlgorithmInitializer.h
 *
 *  Created on: 31 mai 2011 13:38:31
 *      Author: jstark
 */

#ifndef MSEGALGORITHMINITIALIZER_H_
#define MSEGALGORITHMINITIALIZER_H_

namespace mseg {
    class Controller;

//! Register all algorithms with factory.
class AlgorithmInitializer {
public:
    AlgorithmInitializer(Controller * controller);
    virtual ~AlgorithmInitializer();

    static bool initialize();
    static bool initializeProcesses();
    static bool initializeWidgets();

private:
    Controller * m_controller;
};

} // namespace mseg
#endif /* MSEGALGORITHMINITIALIZER_H_ */

