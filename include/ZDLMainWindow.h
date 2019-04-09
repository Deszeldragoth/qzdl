/*
 * This file is part of qZDL
 * Copyright (C) 2007-2010  Cody Harris
 * Copyright (C) 2019  Lcferrum
 * 
 * qZDL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include "ZDLWidget.h"
#include "ZDLInterface.h"
#include "ZDLSettingsTab.h"

class ZDLMainWindow: public QMainWindow{
	Q_OBJECT
public:
	ZDLMainWindow( QWidget *parent=0);
	~ZDLMainWindow();
	void startRead();
	void writeConfig();
	QString getArgumentsString(bool native_sep=false);
	QStringList getArgumentsList();
	QString getExecutable();
	void handleImport();
	QString getWindowTitle();
protected:
	ZDLInterface* intr;
	ZDLSettingsTab* settings;
	QAction *qact2;
public slots:
	void launch();
	void quit();
	void tabChange(int index);
};
#endif
