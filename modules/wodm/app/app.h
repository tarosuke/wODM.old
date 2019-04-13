/** wODM用App
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

#include <toolbox/container/array.h>
#include <toolbox/container/list.h>
#include <toolbox/thread/pthread.h>
#include <toolbox/string.h>

#include <wOLIB/comm.h>
#include <wOLIB/message.h>



namespace wodm{

	class Resource;


	/** Appインターフェイス
	 * あるいはダミーのApp
	 */
	class App{
		App(const App&);
		void operator=(const App&);
	public:

		App(){};
		virtual ~App(){};

		virtual void Send(wO::Message&){};

		//登録と削除
		virtual void Register(unsigned, Resource&){};
		virtual void Detach(unsigned, Resource&){};

	protected:
	};


	/** パイプやソケットなどのApp
	 */
	class CommApp :
		public App,
		public wO::Comm,
		public TB::PThread,
		public TB::List<CommApp>::Node{
		CommApp();
		CommApp(const CommApp&);
		void operator=(const CommApp&);
	public:

		CommApp(const char* appName);
		CommApp(const char* const* params);
		~CommApp();

		//周回処理(処理が完了したら真)
		static bool Tick(unsigned bingo);

		//状態計数
		static unsigned GetConnections(){ return connections; };

		//登録と削除
		void Register(unsigned, Resource&) final;
		void Detach(unsigned id, Resource&) final;

		//基本パス設定
		static void SetCommonBasePath(TB::String bp){
			commonBasePath = bp;
		}

	protected:
		CommApp(const char* executable, const char* basePath);
		bool OnMessage(wO::Message&) override;

	private:

		static TB::String commonBasePath;

		TB::Array<Resource*> array; //array[id]がリソース
		TB::List<Resource> resources;

		static unsigned connections;
		static TB::List<CommApp> apps;
		unsigned progress;
		TB::List<wO::Message> messages;

		//メッセージの送受信と処理
		void Send(wO::Message& m) final { wO::Comm::Send(m); };
		void ThreadBody() final; //Runはメッセージを受信するとOnMessage

		// アプリケーションと接続
		void NewCommApp();
		wO::Comm::Handles Pipe(const char t[], const char* const* params);
	};
}
