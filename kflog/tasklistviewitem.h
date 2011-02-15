/***********************************************************************
**
**   tasklistviewitem.h
**
**   This file is part of KFLog4.
**
************************************************************************
**
**   Copyright (c):  2003 by André Somers
**                   2011 by Axel Pauli
**
**   This file is distributed under the terms of the General Public
**   License. See the file COPYING for more information.
**
**   $Id$
**
***********************************************************************/
/**
  * \class TaskListViewItem
  *
  * \author André Somers, Axel Pauli
  *
  * \brief List view item that contains a task.
  *
  * This class represents a task in the object tree. It manages it's own
  * children, all you need to do is invoke it with the
  * @ref TaskListViewItem(QTreeWidgetItem* parent, Flight* flight) constructor.
  *
  * \date 2003-2011
  *
  * \version $Id$
  */

#ifndef TASK_LIST_VIEW_ITEM_H
#define TASK_LIST_VIEW_ITEM_H

#include <QTreeWidgetItem>

#define TASK_LIST_VIEW_ITEM_TYPEID 10003

class FlightTask;

class TaskListViewItem : public QTreeWidgetItem
{
 public:
  /**
   * Constructor.
   * @param parent Reference to parent @ref QListViewItem
   * @param task Reference to @ref FlightTask object to display
   * @param insertAfter Reference to @ref QTreeWidgetItem after which this item should
   *                    be inserted
   */
  TaskListViewItem(QTreeWidgetItem* parent, FlightTask* task, QTreeWidgetItem* insertAfter=0);
  /**
   * Destructor
   */
  virtual ~TaskListViewItem();
  /**
   * Contains reference to the @ref FlightTask this @ref QTreeWidgetItem is representing
   */
  FlightTask* task;

  /**
   * Called to make the item update itself.
   */
  void update();

protected:
  /**
   * Creates the child nodes for this flight node.
   */
  void createChildren();
};

#endif
