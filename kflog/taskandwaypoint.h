/***************************************************************************
                          taskandwaypoint.h  -  description
                             -------------------
    begin                : Fri Nov 30 2001
    copyright            : (C) 2001 by Harald Maier
    email                : harry@kflog.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TASKANDWAYPOINT_H
#define TASKANDWAYPOINT_H

#include "waypointimpfilterdialog.h"
#include "waypointdialog.h"
#include "waypointcatalog.h"
#include "translationlist.h"
#include "guicontrols/kfloglistview.h"

#include <qframe.h>
#include <qsplitter.h>
#include <qcombobox.h>
#include <qlist.h>

#include <kpopupmenu.h>

/**
  *@author Harald Maier
  */

class TaskAndWaypoint : public QFrame  {
   Q_OBJECT
public:
	TaskAndWaypoint(QWidget *parent=0, const char *name=0);
	~TaskAndWaypoint();
	/* save changes in catalogs, return success */
  bool saveChanges();
private: // Private methods
  /** No descriptions */
  void initSurfaces();
  /** No descriptions */
  void initTypes();
  /** No descriptions */
  void addWaypointWindow(QWidget *parent);
  /** No descriptions */
  void addPopupMenu();
  /** No descriptions */
  void fillWaypoints();
  /** read filter and store in catalog */
  void getFilterData();
private: // Private attributes
  TranslationList surfaces;
  TranslationList waypointTypes;
  /** popup menu for waypoint's */
  KPopupMenu *wayPointPopup;
  /**  */
  QComboBox *catalogName;
  KFLogListView *waypoints;
  /** some menu items */
  int idWaypointCatalogSave;
  int idWaypointCatalogImport;
  int idWaypointCatalogClose;
  int idWaypointImportFromMap;
  int idWaypointNew;
  int idWaypointEdit;
  int idWaypointDelete;
  int idWaypointCopy2Task;
  /** all loaded catalogs */
  QList<WaypointCatalog> waypointCatalogs;
  /**  */
  WaypointDialog *waypointDlg;
  WaypointImpFilterDialog *importFilterDlg;

  /** column index for waypoints */
  int colName;
  int colDesc;
  int colICAO;
  int colType;
  int colLat;
  int colLong;
  int colElev;
  int colFrequency;
  int colLandable;
  int colRunway;
  int colLength;
  int colSurface;
  int colComment;

private slots: // Private slots
  void slotNotHandledItem();
  void slotNewWaypoint();
  void slotDeleteWaypoint();
  void slotEditWaypoint();
  /** insert waypoint from waypoint dialog */
  void slotAddWaypoint();
  /** create a new catalog */
  void slotNewWaypointCatalog();
  /** open a catalog and set it active */
  void slotOpenWaypointCatalog();
  void slotSaveWaypointCatalog();
  void slotCloseWaypointCatalog();
  void slotImportWaypointCatalog();
  void slotSwitchWaypointCatalog(int idx);
  void showWaypointPopup(QListViewItem *, const QPoint &, int);
  void slotImportWaypointFromMap();
  void slotCopyWaypoint2Task();
public slots: // Public slots
  /** filter waypoints to display */
  void slotFilterWaypoints();
  /** add a new waypoint from outside */
  void slotAddWaypoint(WaypointElement *w);
signals: // Signals
  /** No descriptions */
  void copyWaypoint2Task(WaypointElement *);
};

#endif
