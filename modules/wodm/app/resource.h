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
#pragma once

#include <toolbox/container/list.h>

#include <wOLIB/message.h>
#include <wOLIB/widget.h>



namespace wO{
	class Comm;
}

namespace wodm{

	class App;

	class Resource : public TB::List<Resource>::Node{
		Resource();
		Resource(const Resource&);
		void operator=(const Resource&);

	public:
		struct Params{
			App& app;
			unsigned id;
		};

		struct MouseParams{
			int x;
			int y;
			unsigned state;
			unsigned pressed;
			unsigned released;
		};

		virtual ~Resource();

		/** メッセージ処理ハンドラ
		 */
		virtual void OnMessage(wO::Message&)=0;

	protected:
		App& comm;

		/** コンストラクタ
		 */
		Resource(const Params&);
		Resource(App&, unsigned id);

		/** 適切な宛先へ送信
		 */
		void Send(wO::Message&);
		void SendEvent(
			wO::Message::Types, int x, int y,
			unsigned buttonState = 0,
			unsigned pressed = 0,
			unsigned released = 0);
		void SendEvent(wO::Message::Types, const MouseParams&);
		void SendEvent(wO::Message::Types);

		/** ID取得
		 */
		unsigned GetID(){ return id; };

	private:
		const unsigned id;
	};
}
