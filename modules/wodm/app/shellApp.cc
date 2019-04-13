/** アプリケーションのプロキシ(shell用)
 * Copyright (C) 2018 tarosuke<webmaster@tarosuke.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <syslog.h>

#include <wOLIB/message.h>

#include <module.h>

#include "shellApp.h"
#include "scenery.h"
#include "../widget/cursor.h"



namespace wodm{


	ShellApp::ShellApp(const char* const* params) : CommApp(params){
		syslog(LOG_DEBUG, "Connecting to Shell(%s)...\n", params[0]);
	};

	bool ShellApp::OnMessage(wO::Message& m){
		//パケット抽出
		wO::Message::Pack& p(m);

		switch(p.head.type){
		case wO::Message::cursorsetCreated:
			//カーソルセット生成
			CursorSet::New(wO::Cursor::Pack(m));
			break;
		case wO::Message::logout:
			vr_core::Module::Quit();
			break;
#if 0
		case wO::Message::stickCenter:
			POSE::StickAzimuth();
			break;
		case wO::Message::lockCenter:
			POSE::ResetAzimuth();
			break;
		case wO::Message::planetCreated:
			new Planet(params);
			break;
#endif
		case wO::Message::createScenery:
			new Scenery(p);
			break;
		case wO::Message::spawn: //プロセス起動
			syslog(LOG_INFO,"spawn:%s", p.body);
			if(p.body[0]){
				const char* const  target[] = {
					(const char*)p.body, 0
				};
				new CommApp(target);
			}
			break;
		default:
			return CommApp::OnMessage(m);
		}
		return true;
	}

}
