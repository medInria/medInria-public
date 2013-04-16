#!/bin/sh

DASHBOARDDIR=/net/servers/www-sop/asclepios/software/vtkINRIA3D/dashboard
cd $DASHBOARDDIR
make NightlyDashboardStart
make NightlyDashboardEnd
