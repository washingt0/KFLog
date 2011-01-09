/***********************************************************************
 **
 **   mainwindow.h
 **
 **   This file is part of KFLog4.
 **
 ************************************************************************
 **
 **   Copyright (c):  2001 by Heiner Lamprecht, Florian Ehinger
 **
 **   This file is distributed under the terms of the General Public
 **   License. See the file COPYING for more information.
 **
 **   $Id$
 **
 ***********************************************************************/

/**
 * \class MainWindow
 *
 * \author Heiner Lamprecht, Florian Ehinger, Axel Pauli
 *
 * \brief This class provides the main window of KFLog.
 *
 * This class provides the main window of KFLog. All needed GUI stuff
 * is initialized and handled here.
 *
 * \date 2001-2010
 *
 * \version $Id$
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QAction>
#include <QActionGroup>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QProgressBar>
#include <QUrl>

#include "dataview.h"
#include "evaluationdialog.h"
#include "helpwindow.h"
#include "map.h"
#include "mapcontrolview.h"
#include "objecttree.h"
#include "translationlist.h"
#include "topolegend.h"
#include "waypoints.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

  private:

  Q_DISABLE_COPY ( MainWindow )

public:

  MainWindow( QWidget * parent = 0, Qt::WindowFlags flags = 0 );

  virtual ~MainWindow();

  /**
   * \return The requested pixmap.
   */
  QPixmap getPixmap( const QString& pixmapName );

  /**
   * Creates the application data directory, if it not exists.
   *
   * \return True on success otherwise false
   */
  bool createApplicationDataDirectory();

  /**
   * \return The application's data directory.
   */
  QString getApplicationDataDirectory();

  signals:

  /**
  * Emitted, when the user selects a new flightdatatype.
  *
  * @param  type  The id of the selected data-type
  */
  void flightDataTypeChanged(int type);

public slots:

  /** Reimplemented from QWidget */
  void closeEvent(QCloseEvent *e);
  /**
  * Display dialog to ask for coordinates and center map on that point.
  */
  void slotCenterTo();
  /**
   * Opens a dialog for configuration of KFLog.
   */
  void slotConfigureKFLog();
  /**
   * Opens the printing-dialog to print the map.
   */
  void slotFilePrint();
  /** */
  void slotFlightPrint();
  /** */
  void slotFlightViewIgc3D();
  /** Called, if window is closed by the user. */
  void slotFlightViewIgc3DClosed();
  /** */
  void slotFlightViewIgcOpenGL();
  /** set menu items enabled/disabled */
  void slotModifyMenu();
  /**
   * Opens a file-open-dialog.
   */
  void slotOpenFile();
  /**
   * Opens the file given in url.
   */
  void slotOpenFile(const char* surl);
  /**
   * Opens a task-file-open-dialog.
   */
  void slotOpenTask();
  /**
   * Opens a selected recently opened flight.
   */
  void slotOpenRecentFile();
  /** */
  void slotOpenRecorderDialog();
  /** optimize flight for OLC declaration*/
  void slotOptimizeFlightOLC();
  /** */
  void slotOptimizeFlight();
  /** Connects the dialogs addWaypoint signal to the waypoint object. */
  void slotRegisterWaypointDialog(QWidget * dialog);

  /**
   * Called, when the user selects a data-type from the menu. Emits
   * flightDataTypeChanged(int)
   *
   * @param  menuItem  The id of the selected listitem.
   *
   * @see #flightDataTypeChanged(int)
   */
  void slotSelectFlightData( const int index );
  /**
   * Updates the recent file list.
   */
  void slotSetCurrentFile(const QString &fileName);
  /**
   * Displays the position of the mouse cursor and some info (time,
   * altitude, speed, vario) about the selected flight point in the
   * status bar.
   *
   * @param mouseP   The lat/lon position under the mouse cursor.
   * @param flightP  Pointer to the flight point.
   */
  void slotSetPointInfo(const QPoint& mousePosition,
                         const flightPoint& point);
  /**
   * Displays the position of the mouse cursor in the status bar and
   * deletes the text of the other status bar-fields.
   */
  void slotSetPointInfo(const QPoint&);
  /**
   * Updates the progress bar in the status bar.
   *
   * @param  value  The new value of the progress bar, given in percent;
   */
  void slotSetProgress(int value);
  /**
   * Displays a message in the status bar.
   *
   * @param  text  The message to be displayed.
   */
  void slotSetStatusMsg(const QString& text);
  /** */
  void slotSetWaypointCatalog(QString catalog);
  /**
   * Checks the status of all dock-widgets and updates the menu.
   */
  void slotCheckDockWidgetStatus();
  /** */
  void slotSavePixmap(QUrl url, int width, int height);
  /**
   * Shows or hides the dataview-widget.
   */
  void slotToggleDataView();
  /**
    * Shows or hides the Evaluation window
    */
  void slotToggleEvaluationWindow( bool flag );
  /**
   * Called is the visibility of the Evaluation window was changed.
   */
  void slotEvaluationWindowVisibilityChanged( bool flag );
  /**
   * Shows or hides the Help Window.
   */
  void slotToggleHelpWindow();
  /**
   * Shows or hides the object-widget.
   */
  void slotToggleLegendDock();
//  /**
//   * Shows or hides the map-widget.
//   */
//  void slotToggleMap();
  /**
   * Shows or hides the mapcontrol-widget.
   */
  void slotToggleMapControl();
  /**
   * Shows or hides the legend-widget.
   */
  void slotToggleObjectTreeDock();
  /**
   * Shows or hides the toolbar.
   */
  void slotToggleToolBar();
  /**
   * Shows or hides the status bar.
   */
  void slotToggleStatusBar();
  /**
   * Shows or hides the waypoints-widget.
   */
  void slotToggleWaypointsDock();
  /**
   * Called to the What's This? mode.
   */
  void slotWhatsThis();
  /**
   * insert available flights into menu
   */
  void slotWindowsMenuAboutToShow();

  /**
   * Called, if an action of the group is triggered.
   */
  void slotFlightDataTypeGroupAction( QAction *action );

protected:
  /**
   * Writes the window-geometry, statusbar- and toolbar state and the
   * layout state of the doc kwidgets.
   */
  void saveOptions();
  /**
   * Reads the window-geometry, statusbar- and toolbar state and the
   * layout state of the dock widgets.
   */
  void readOptions();

private:
  /** Initializes all QDockWindows */
  void initDockWindows();
  /** Initializes QMenuBar */
  void initMenuBar();
  /** Initializes the QStatusBar */
  void initStatusBar();
  /** Initializes surface types */
  void initSurfaceTypes();
  /** Initializes task types*/
  void initTaskTypes();
  /** Initializes toolbar*/
  void initToolBar();
  /** Initializes waypoint types*/
  void initWaypointTypes();
  /**
   * Dockwidget to handle the dataview-widget.
   * The dataview-widget. Embedded in dataViewDock
   */
  QDockWidget* dataViewDock;
  DataView* dataView;
  /**
   * Dockwidget to handle the EvaluationWindow.
   * The evalutionWindow. Embedded in evaluationWindowDock
   */
  QDockWidget* evaluationWindowDock;
  EvaluationDialog* evaluationWindow;
  /**
   * Dockwidget to handle the helpWindow.
   * The helpWindow. Embedded in helpWindowDock
   */
  QDockWidget* helpWindowDock;
  HelpWindow* helpWindow;
  /**
   * Dockwidget to handle the legend-widget.
   *
   * @see TopoLegend
   */
  QDockWidget* legendDock;
  TopoLegend* legend;
  /**
   * Dockwidget to handle the map.
   * The map-widget.
   */
  QDockWidget* mapViewDock;
  Map* map;
  /**
   * Dockwidget to handle the mapcontrol.
   * The mapcontrol-widget. Embedded in mapControlDock
   */
  QDockWidget* mapControlDock;
  MapControlView* mapControl;
  /**
   * Dockwidget to handle the object view
   *
   * @see ObjectView
   */
  QDockWidget* objectTreeDock;
  ObjectTree* objectTree;
  /**
   * Dockwidget to handle the waypoints-widget.
   * The waypoints-widget.
   */
  QDockWidget* waypointsDock;
  Waypoints* waypoints;
  QToolBar* toolBar;
  /**
   * Actions for the menu File
   */
  QAction* fileNewWaypoint;
  QAction* fileNewTask;
  QAction* fileNewFlightGroup;
  QAction* fileOpenFlight;
  QAction* fileOpenTask;
  Q3PopupMenu* fileOpenRecent;
  QAction* fileClose;
  QAction* fileSavePixmap;
  QAction* filePrint;
  QAction* filePrintFlight;
  QAction* fileOpenRecorder;
  QAction* fileQuit;
  /**
   * Actions for the menu View
   */
  QAction* viewCenterTask;
  QAction* viewCenterFlight;
  QAction* viewCenterHomesite;
  QAction* viewCenterTo;
  QAction* viewZoomIn;
  QAction* viewZoomOut;
  QAction* viewZoom;
  QAction* viewRedraw;
  QAction* viewMoveNW;
  QAction* viewMoveN;
  QAction* viewMoveNE;
  QAction* viewMoveW;
  QAction* viewMoveE;
  QAction* viewMoveSW;
  QAction* viewMoveS;
  QAction* viewMoveSE;
  /**
   * Actions for the menu Flight
   */
  QAction* flightEvaluationWindowAction;
  QAction* flightOptimizationAction;
  QAction* flightOptimizationOLCAction;
  QAction* flightIgc3DAction;
  QAction* flightIgcOpenGLAction;
  QAction* flightAnimateStartAction;
  QAction* flightAnimateStopAction;
  QAction* flightAnimateNextAction;
  QAction* flightAnimatePrevAction;
  QAction* flightAnimate10NextAction;
  QAction* flightAnimate10PrevAction;
  QAction* flightAnimateHomeAction;
  QAction* flightAnimateEndAction;

  QActionGroup* flightDataTypeGroupAction;
  QAction *altitudeAction;
  QAction *cyclingAction;
  QAction *speedAction;
  QAction *varioAction;
  QAction *solidAction;

  /**
   * Action for the menu Window
   */
  Q3PopupMenu* windowMenu;
  /**
   * Actions for the menu Settings
   */
  Q3PopupMenu* settings;
  QAction* settingsEvaluationWindow;
  QAction* settingsFlightData;
  QAction* settingsHelpWindow;
  QAction* settingsLegend;
//  QAction* settingsMap;
  QAction* settingsMapControl;
  QAction* settingsObjectTree;
  QAction* settingsStatusBar;
  QAction* settingsToolBar;
  QAction* settingsWaypoints;

  /**
   * The progressbar in the statusbar. Used during drawing the map to display
   * the percentage of what is already drawn.
   *
   * @see slotSetProgress
   */
  QProgressBar* statusProgress;
  /**
   * The label to display a message in the statusbar.
   *
   * @see slotStatusMsg
   */
  QLabel* statusLabel;
  /**
   * The label to display the time of a selected flight-point.
   *
   * @see slotShowPointInfo
   */
  QLabel* statusTimeL;
  /**
   * The label to display the altitude of a selected flight-point.
   *
   * @see slotShowPointInfo
   */
  QLabel* statusAltitudeL;
  /**
   * The label to display the vario of a selected flight-point.
   *
   * @see slotShowPointInfo
   */
  QLabel* statusVarioL;
  /**
   * The label to display the speed of a selected flight-point.
   *
   * @see slotShowPointInfo
   */
  QLabel* statusSpeedL;
  /**
   * The label to display the latitude of the position under the mouse cursor.
   *
   * @see slotShowPointInfo
   */
  QLabel* statusLatL;
  /**
   * The label to display the longitude of the position under the mouse cursor.
   *
   * @see slotShowPointInfo
   */
  QLabel* statusLonL;
  /**
   * The flight-directory.
   */
  QString flightDir;
  /**
   * The task and waypoints directory.
   */
  QString taskDir;
};

#endif // MAIN_WINDOW_H
