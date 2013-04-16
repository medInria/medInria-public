/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

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


