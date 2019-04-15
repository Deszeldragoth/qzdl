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
 

#include "ZDLListable.h"
#include "ZDLFileListable.h"
#include <QFileInfo>

ZDLFileListable::ZDLFileListable( QListWidget *parent, int type, QString file):ZDLNameListable(parent, type, file, QFileInfo(file).fileName()){
	fileName = file;
}

QString ZDLFileListable::getFile(){
	return fileName;
}
	


