/*
 * This file is part of qZDL
 * Copyright (C) 2007-2010  Cody Harris
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
 
#include <iostream>
#include <QtGui>
#include <QApplication>
#include <QMainWindow>

#include "ZDLInterface.h"
#include "ZDLMainWindow.h"
#include "ZDLConfigurationManager.h"
#include "ZDLInfoBar.h"

extern QApplication *qapp;
extern QString versionString;

ZDLMainWindow::~ZDLMainWindow(){
	delete zup;
}

void ZDLMainWindow::manageUpdate(){
	if (zup->hasUpdate()){
		ZDLInfoBar *bar = (ZDLInfoBar*)ZDLConfigurationManager::getInfobar();
		ZDLConfigurationManager::setInfobarMessage("There is an update available.",2);
		connect(bar,SIGNAL(moreclicked()),this,SLOT(newUpdate()));
	}
}

void ZDLMainWindow::newUpdate(){
	QString engine = ZDL_ENGINE_NAME;
	QMessageBox::warning(NULL,ZDL_ENGINE_NAME, "There has been an update posted for "+engine+"\n\nPlease visit the "+engine+" website at http://zdlsharp.vectec.net for more information.",QMessageBox::Ok,QMessageBox::Ok);
}

void ZDLMainWindow::setUpdater(ZDLUpdater *zup){
	this->zup = zup;
	connect(zup, SIGNAL(updateReady()), this, SLOT(manageUpdate()));
}

ZDLMainWindow::ZDLMainWindow(QWidget *parent): QMainWindow(parent){
	QString windowTitle = ZDL_ENGINE_NAME;
	windowTitle += " " + versionString + " - " + ZDLConfigurationManager::getConfigFileName();
	setWindowTitle(windowTitle);
	
	setWindowIcon(ZDLConfigurationManager::getIcon());
	
	
	setContentsMargins(2,2,2,2);
	layout()->setContentsMargins(2,2,2,2);
	QTabWidget *widget = new QTabWidget(this);
	
 	intr = new ZDLInterface(this);
	settings = new zSettingsPane(this);
	
	setCentralWidget(widget);
	widget->addTab(intr, "Main");
	widget->addTab(settings, "Settings");
	//I haven't started on this yet :)
	//widget->addTab(new ZDLInterface(this), "Notifications");
	
	QAction *qact = new QAction(widget);
	qact->setShortcut(Qt::Key_Return);
	widget->addAction(qact);
	connect(qact, SIGNAL(triggered()), this, SLOT(launch()));
	
	qact2 = new QAction(widget);
	qact2->setShortcut(Qt::Key_Escape);
	widget->addAction(qact2);
	connect(qact2, SIGNAL(triggered()), this, SLOT(quit()));
	
	connect(widget, SIGNAL(currentChanged(int)), this, SLOT(tabChange(int)));
	
}

void ZDLMainWindow::tabChange(int newTab){
	if(newTab == 0){
		settings->notifyFromParent(NULL);
		intr->readFromParent(NULL);
	}else if (newTab == 1){
		intr->notifyFromParent(NULL);
		settings->readFromParent(NULL);
	}
}

void ZDLMainWindow::quit(){
	writeConfig();
	close();
}

void ZDLMainWindow::launch(){
	writeConfig();
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	
	QString exec = getExecutable();
	if (exec.length() < 1){
		QMessageBox::critical(this, ZDL_ENGINE_NAME, "Please select a source port");
		return;
	}
	QStringList args = getArguments();
	if (args.join("").length() < 1){
		return;
	}
	
	QProcess *proc = new QProcess(this);
	proc->setProcessChannelMode(QProcess::ForwardedChannels);
	proc->start(exec, args);
	int stat;
	if (zconf->hasValue("zdl.general", "autoclose")){
		QString append = zconf->getValue("zdl.general", "autoclose",&stat);
		if (append == "1" || append == "true"){
			close();
		}
	}
	
	procerr = proc->error();
	
// 	if(proc->state() != QProcess::NotRunning){
// 		std::cout << "ERROR!" << std::endl;
// 		ZDLConfigurationManager::setInfobarMessage("The process ended abnormally.",1);
// 		ZDLInfoBar *bar = (ZDLInfoBar*)ZDLConfigurationManager::getInfobar();
// 		connect(bar,SIGNAL(moreclicked()),this,SLOT(badLaunch()));
// 	}
	
}

void ZDLMainWindow::badLaunch(){
	if(procerr == QProcess::FailedToStart){
		QMessageBox::warning(NULL,"Failed to Start", "Failed to launch the application executable.",QMessageBox::Ok,QMessageBox::Ok);
	}else if(procerr == QProcess::Crashed){
		QMessageBox::warning(NULL,"Process Crashed", "The application ended abnormally (usually due to a crash or error).",QMessageBox::Ok,QMessageBox::Ok);
	}else{
		QMessageBox::warning(NULL,"Unknown error", "There was a problem running the application.",QMessageBox::Ok,QMessageBox::Ok);
	}
}

QStringList ZDLMainWindow::getArguments(){
	QStringList ourString;
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	ZDLSection *section = NULL;
	
	unsigned int iwadIndex = 0;
	int doquotes = 0;
	
	if(zconf->hasValue("zdl.general","quotefiles")){
		int ok;
		string rc = zconf->getValue("zdl.general","quotefiles",&ok);
		if(rc == "enabled"){
			doquotes = 1;
		}	
	}
	
	bool ok;
	int stat;
	if(zconf->hasValue("zdl.save", "efirst")){
		string rc = zconf->getValue("zdl.save", "efirst", &stat);
		if(rc.length() > 0){
			ourString << rc.c_str();
		}
	}
	
	if(zconf->hasValue("zdl.save", "iwad")){
		int index = 0;
		string rc = zconf->getValue("zdl.save", "iwad", &stat);
		if (rc.length() > 0){
			index = atoi((char*)rc.c_str());
		}
		if (index >= 0){
			iwadIndex = index;
		}else{
			QMessageBox::critical(this, ZDL_ENGINE_NAME, "Please select an IWAD");
			return ourString;
		}
	}else{
		QMessageBox::critical(this, ZDL_ENGINE_NAME, "Please select an IWAD");
		return ourString;
	}
	
	section = zconf->getSection("zdl.iwads");
	if (section){
		vector <ZDLLine*> fileVctr;
		section->getRegex("^i[0-9]+f$", fileVctr);
		
		for(unsigned int i = 0; i < fileVctr.size(); i++){
			if (i == iwadIndex){
				ourString << "-iwad";
				if(doquotes){
					QString temp = "\"";
					temp += fileVctr[i]->getValue();
					temp += "\"";
					ourString << temp;
				}else{
					ourString << fileVctr[i]->getValue();
				}
				break;
			}
		}
	}
	
	if (zconf->hasValue("zdl.save", "skill")){
		ourString << "-skill";
		ourString << zconf->getValue("zdl.save", "skill", &stat);
	}
	
	if (zconf->hasValue("zdl.general", "alwaysadd")){
		ourString << zconf->getValue("zdl.general", "alwaysadd", &stat);
	}
	
	if (zconf->hasValue("zdl.save", "warp")){
		ourString << "+map";
		ourString << zconf->getValue("zdl.save", "warp", &stat);
	}
	
	if (zconf->hasValue("zdl.save", "dmflags")){
		ourString << "+set";
		ourString << "dmflags";
		ourString << zconf->getValue("zdl.save", "dmflags", &stat);
	}
	
	if (zconf->hasValue("zdl.save", "dmflags2")){
		ourString << "+set";
		ourString << "dmflags2";
		ourString << zconf->getValue("zdl.save", "dmflags2", &stat);
	}
	
	section = zconf->getSection("zdl.save");
	if (section){
		vector <ZDLLine*> fileVctr;
		section->getRegex("^file[0-9]+$", fileVctr);
		
		if (fileVctr.size() > 0){
			ourString << "-file";
			for(unsigned int i = 0; i < fileVctr.size(); i++){
				//TODO: Fix this
				//Dirty ugly hack.
				if(doquotes){
					QString quoted =  "\"";
					quoted += fileVctr[i]->getValue();
					quoted += "\"";
					ourString << quoted;
				}else{
					ourString << fileVctr[i]->getValue();
				}
			}
		}
	}
	
	if (zconf->hasValue("zdl.save", "extra")){
		ourString << zconf->getValue("zdl.save", "extra", &stat);
	}
	
	if(zconf->hasValue("zdl.save","gametype")){
		QString tGameType = zconf->getValue("zdl.save","gametype",&stat);
		if(tGameType != "0"){
			if (tGameType == "2"){
				ourString << "-deathmath";
			}
			int players = -1;
			if(zconf->hasValue("zdl.save","players")){
				QString tPlayers = zconf->getValue("zdl.save","players",&stat);
				players = tPlayers.toInt(&ok, 10);
			}
			if(players > 0){
				ourString << "-host";
				ourString << QString::number(players);
			}else if(players == 0){
				if(zconf->hasValue("zdl.save","host")){
					ourString << "-join";
					zconf->getValue("zdl.save","host",&stat);
				}
			}
			if(zconf->hasValue("zdl.save","fraglimit")){
				ourString << "+set";
				ourString << "fraglimit";
				ourString << zconf->getValue("zdl.save","fraglimit",&stat);
				
			}
		}
	}
	
	
	
	if(zconf->hasValue("zdl.net","advenabled")){
		QString aNetEnabled = zconf->getValue("zdl.net","advenabled",&stat);
		if(aNetEnabled == "enabled"){
			if(zconf->hasValue("zdl.net","port")){
				ourString << "-port";
				ourString << zconf->getValue("zdl.net","port",&stat);
			}
			if(zconf->hasValue("zdl.net","extratic")){
				QString tExtratic = zconf->getValue("zdl.net","extratic",&stat);
				if(tExtratic == "enabled"){
					ourString << "-extratic";
				}
			}
			if(zconf->hasValue("zdl.net","netmode")){
				QString tNetMode = zconf->getValue("zdl.net","netmode",&stat);
				if(tNetMode == "1"){
					ourString << "-netmode";
					ourString << "0";
				}else if(tNetMode == "2"){
					ourString << "-netmode";
					ourString << "1";
				}
			}
			if(zconf->hasValue("zdl.net","dup")){
				QString tDup = zconf->getValue("zdl.net","dup",&stat);
				if(tDup != "0"){
					ourString << "-dup";
					ourString << tDup;
				}
			}
		}
	}

	return ourString;
}

QString ZDLMainWindow::getExecutable(){
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	int stat;
	unsigned int portIndex = 0;
	if(zconf->hasValue("zdl.save", "port")){
		int index = 0;
		string rc = zconf->getValue("zdl.save", "port", &stat);
		if (rc.length() > 0){
			index = atoi((char*)rc.c_str());
		}
		if (index >= 0){
			portIndex = index;
		}else{
			zconf->setValue("zdl.save", "port", 0);
			portIndex = 0;
		}
	}else{
		return "";
	}
	
	ZDLSection *section = zconf->getSection("zdl.ports");
	if (section){
		vector <ZDLLine*> fileVctr;
		section->getRegex("^p[0-9]+f$", fileVctr);
		
		for(unsigned int i = 0; i < fileVctr.size(); i++){
			if (i == portIndex){
				return fileVctr[i]->getValue();
			}
			
		}
	}
	return "";
}


//Pass through functions.
void ZDLMainWindow::startRead(){
	intr->startRead();
	settings->startRead();
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	zconf->setValue("zdl.general", "engine", ZDL_ENGINE_NAME);
	zconf->setValue("zdl.general", "version", ZDL_VERSION_STRING);
	
	QString windowTitle = ZDL_ENGINE_NAME;
	windowTitle += " " + versionString + " - " + ZDLConfigurationManager::getConfigFileName();
	setWindowTitle(windowTitle);
}

void ZDLMainWindow::writeConfig(){
	intr->writeConfig();
	settings->writeConfig();
}