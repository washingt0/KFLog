# KFLog4 qmake project file
# $Id$

QT += qt3support \
      network \
      xml

CONFIG += qt \
    warn_on \
    debug
    
TEMPLATE = app

SOURCES = \
    airfield.cpp \
    airspace.cpp \
    altitude.cpp \
    authdialog.cpp \
    baseflightelement.cpp \
    basemapelement.cpp \
    centertodialog.cpp \
    configdrawelement.cpp \
    configprintelement.cpp \
    da4record.cpp \
    dataview.cpp \
    distance.cpp \
    downloadmanager.cpp \
    elevationfinder.cpp \
    evaluationdialog.cpp \
    evaluationframe.cpp \
    evaluationview.cpp \
    flight.cpp \
    flightdataprint.cpp \
    flightgroup.cpp \
    flightgrouplistviewitem.cpp \
    flightlistviewitem.cpp \
    flightloader.cpp \
    flightrecorderpluginbase.cpp \
    flightselectiondialog.cpp \
    flighttask.cpp \
    helpwindow.cpp \
    httpclient.cpp \
    igc3ddialog.cpp \
    igc3dflightdata.cpp \
    igc3dpolyhedron.cpp \
    igc3dview.cpp \
    igc3dviewstate.cpp \
    igcpreview.cpp \
    isohypse.cpp \
    isolist.cpp \
    kflogconfig.cpp \
    lineelement.cpp \
    main.cpp \
    mainwindow.cpp \
    map.cpp \
    mapcalc.cpp \
    mapconfig.cpp \
    mapcontents.cpp \
    mapcontrolview.cpp \
    mapmatrix.cpp \
    mapprint.cpp \
    objecttree.cpp \
    openairparser.cpp \
    optimization.cpp \
    optimizationwizard.cpp \
    projectionbase.cpp \
    projectioncylindric.cpp \
    projectionlambert.cpp \
    radiopoint.cpp \
    recorderdialog.cpp \
    runway.cpp \
    singlepoint.cpp \
    taskdataprint.cpp \
    taskdialog.cpp \
    tasklistviewitem.cpp \
    topolegend.cpp \
    translationelement.cpp \
    translationlist.cpp \
    waypoint.cpp \
    waypointcatalog.cpp \
    waypointdialog.cpp \
    waypointimpfilterdialog.cpp \
    waypoints.cpp \
    welt2000.cpp \
    wgspoint.cpp \
    whatsthat.cpp \
    guicontrols/coordedit.cpp \
    guicontrols/kfloglistview.cpp
    
HEADERS = \
    airfield.h \
    airspace.h \
    altitude.h \
    authdialog.h \
    baseflightelement.h \
    basemapelement.h \
    centertodialog.h \
    configdrawelement.h \
    configprintelement.h \
    da4record.h \
    dataview.h \
    distance.h \
    downloadmanager.h \
    elevationfinder.h \
    evaluationdialog.h \
    evaluationframe.h \
    evaluationview.h \
    flight.h \
    flightdataprint.h \
    flightgroup.h \
    flightgrouplistviewitem.h \
    flightlistviewitem.h \
    flightloader.h \
    flightrecorderpluginbase.h \
    flightselectiondialog.h \
    flighttask.h \
    frstructs.h \
    gliders.h \
    helpwindow.h \
    httpclient.h \
    igc3ddialog.h \
    igc3dflightdata.h \
    igc3dpolyhedron.h \
    igc3dview.h \
    igc3dviewstate.h \
    igcpreview.h \
    isolist.h \
    isohypse.h \
    kflogconfig.h \
    lineelement.h \
    mainwindow.h \
    map.h \
    mapcalc.h \
    mapconfig.h \
    mapcontents.h \
    mapcontrolview.h \
    mapdefaults.h \
    mapmatrix.h \
    mapprint.h \
    objecttree.h \
    openairparser.h \
    optimization.h \
    optimizationwizard.h \
    projectionbase.h \
    projectioncylindric.h \
    projectionlambert.h \
    radiopoint.h \
    recorderdialog.h \
    singlepoint.h \
    resource.h \
    runway.h \
    taskdataprint.h \
    taskdialog.h \
    tasklistviewitem.h \
    topolegend.h \
    translationelement.h \
    translationlist.h \
    waypoint.h \
    waypointcatalog.h \
    waypointdialog.h \
    waypointimpfilterdialog.h \
    waypoints.h \
    welt2000.h \
    wgspoint.h \
    whatsthat.h \
    wp.h \
    guicontrols/coordedit.h \
    guicontrols/kfloglistview.h
    
FORMS = optimizationwizard.ui

# Note! qmake do prefix the .path variable with $(INSTALL_ROOT)
# in the generated makefile. If the .path variable starts not with
# a slash, $(INSTALL_ROOT) followed by the current path is added as
# prefix to it.

pics.path  = /
pics.files = pics

mapicons.extra = install -d $(INSTALL_ROOT)/mapicons/small
mapicons.path  = /
mapicons.files = mapicons

landscape.path  = /mapdata/landscape
landscape.files = ../README-MAP

airfields.path  = /mapdata/airfields
airfields.files = ../README-AIRFIELDS

airspaces.path  = /mapdata/airspaces
airspaces.files = ../README-AIRSPACE

INSTALLS += pics mapicons landscape airfields airspaces

DESTDIR = ../release/bin
