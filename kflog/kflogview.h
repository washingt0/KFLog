/***********************************************************************
**
**   kflogview.h
**
**   This file is part of KFLog2.
**
************************************************************************
**
**   Copyright (c):  2001 by Heiner Lamprecht
**
**   This file is distributed under the terms of the General Public
**   Licence. See the file COPYING for more information.
**
**   $Id$
**
***********************************************************************/

#ifndef KFLOGVIEW_H
#define KFLOGVIEW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

// include files for Qt
#include <qwidget.h>

/** The KFLogView class provides the view widget for the KFLogApp instance.	
 * The View instance inherits QWidget as a base class and represents the view object of a KTMainWindow. As KFLogView is part of the
 * docuement-view model, it needs a reference to the document object connected with it by the KFLogApp class to manipulate and display
 * the document structure provided by the KFLogDoc class.
 * 	
 * @author Source Framework Automatically Generated by KDevelop, (c) The KDevelop Team.
 * @version KDevelop version 0.4 code generation
 */
class KFLogView : public QWidget
{
  Q_OBJECT
  public:
    /** Constructor for the main view */
    KFLogView(QWidget *parent = 0, const char *name=0);
    /** Destructor for the main view */
    ~KFLogView();
	
  private:
	
};

#endif // KFLOGVIEW_H
