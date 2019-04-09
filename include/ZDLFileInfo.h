/*
 * This file is part of qZDL
 * Copyright (C) 2018-2019  Lcferrum
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
 
#ifndef _ZDLIFILEINFO_H_
#define _ZDLIFILEINFO_H_

#include <QFileInfo>

class ZDLFileInfo: public QFileInfo {
	public: 
		ZDLFileInfo();
		ZDLFileInfo(const QString &file);
		virtual QString GetFileDescription();
};

class ZDLIwadInfo: public ZDLFileInfo {
	public: 
		ZDLIwadInfo();
		ZDLIwadInfo(const QString &file);
		virtual QString GetFileDescription();
};

class ZDLAppInfo: public ZDLFileInfo {
	public: 
		ZDLAppInfo();
		ZDLAppInfo(const QString &file);
		virtual QString GetFileDescription();
};

#endif