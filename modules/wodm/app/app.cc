/** wODM用リソース管理
 * Copyright (C) 2017,2019 tarosuke<webmaster@tarosuke.net>
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
#include <wOLIB/debug.h>
#include <wOLIB/widget.h>

#include "../app/app.h"



namespace wodm{

	/** コンストラクタとデストラクタ
	 */
	App::Resource::Resource(App& app, unsigned id) :
		TB::List<Resource>::Node(true),
		comm(app),
		id(id){
		comm.Register(id, *this);
	}
	App::Resource::~Resource(){
		comm.Detach(id, *this);
	}


	/** メッセージ送信
	 * 宛先はアプリケーション側の対応するResource
	 */
	void App::Resource::Send(wO::Message& m){
		m.GetContent().head.id = id;
		comm.Send(m);
	}


	/** 画面側イベントハンドラ
	 * 画面側なのでイベントに対応したメッセージを生成する
	 */
	//ポインタ系
	void App::Resource::SendEvent(
		wO::Message::Types type,
		int x, int y,
		unsigned buttonState,
		unsigned pressedButton,
		unsigned releasedButton){
		wO::Widget::EventPack p;
		p.head.len = sizeof(p);
		p.head.type = type;
		p.x = x;
		p.y = y;
		p.pressed = pressedButton;
		p.released = releasedButton;
		p.buttonState = buttonState;
		wO::Message m((wO::Message::Pack*)&p);
		Send(m);
	}
	void App::Resource::SendEvent(wO::Message::Types type){
		wO::HeadMessage m(type);
		Send(m);
	}
}
