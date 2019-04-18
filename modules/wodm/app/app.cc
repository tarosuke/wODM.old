/** アプリケーションのプロキシ
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
#include "app.h"

#include <assert.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>

#include <toolbox/prefs.h>

#include <wOLIB/message.h>
#include <wOLIB/widget.h>

#include "resource.h"
#include "../widget/widget.h"
#include "../widget/cursor.h"



namespace{
	TB::Prefs<bool> testMode("+t", false, TB::CommonPrefs::nosave);
}

namespace wodm{

	TB::String CommApp::commonBasePath;
	unsigned CommApp::connections(0);
	TB::List<CommApp> CommApp::apps;


	CommApp::CommApp(const char* const* params) :
		wO::Comm(Pipe(*params, params)),
		TB::PThread(true),
		progress(1){
		Cursor::EnterProgress();
		++connections;

		apps.Add(*this);
		RaiseThread();
	}

	CommApp::~CommApp(){
		while(progress--){
			Cursor::ExitProgress();
		}
		--connections;
	}

	//受信処理
	void CommApp::ThreadBody(){
		while(wO::Message* const m = ReceiveMessage()){
			messages.Add(*m);
		}
		syslog(LOG_ERR, "disconnected");
	}

	/** リソースの登録と削除
	 */
	void CommApp::Register(unsigned id, Resource& r){
		resources.Add(r);
		array[id] = &r;
	}
	void CommApp::Detach(unsigned id, Resource& r){
		assert(array[id] == &r);
		array[id] = 0;
	}


	/** 一次メッセージハンドラ
	 * 受け取ったメッセージを処理したり振り分けたり
	 */
	bool CommApp::Tick(unsigned bingo){
		for(TB::List<CommApp>::I a(apps); ++a;){
			while(wO::Message* m = (*a).messages.Get()){
				(*a).OnMessage(*m);
				delete m;

				//処理数分だけ処理したら終了(ただし最低１つは処理)
				if(!bingo--){
					return false;
				}
			}
		}
		return true;
	}
	bool CommApp::OnMessage(wO::Message& m){
		//パケット抽出
		wO::Message::Pack& p(m);
		wO::Widget::CommandPack& cp(m);

		//メッセージタイプによる振り分け
		switch (p.head.type){
		case wO::Message::widgetCreated:
			{
				auto parent(
					dynamic_cast<vr_core::Widget*>(array[cp.parent]));
				Widget::New(*this, parent, cp);
			}
			break;
		case wO::Message::enterProgress:
			progress++;
			Cursor::EnterProgress();
			break;
		case wO::Message::exitProgress:
			Cursor::ExitProgress();
			progress--;
			break;
		case wO::Message::disconnected : //アプリが切断された
			return false;
			break;
		default:
			//通常メッセージ
			Resource* const res(array[p.head.id]);
			if(res){
				if(p.head.type == wO::Message::disposed){
					delete res;
				}else{
					(*res).OnMessage(m);
				}
			}else{
				syslog(LOG_ERR, "--orphan message(%08x)--\n", p.head.id);
				//			m.Dump();
			}
			break;
		}
		return true;
	}


	/** アプリケーションと接続
	 */
	wO::Comm::Handles CommApp::Pipe(
		const char t[],
		const char* const* params){
		/** 親→子のパイプ生成
		 * p2cHandles[0] ： 親では使わない／子で0番に複製
		 * p2cHandles[1] ： 親で書き込む／子では使わない
		 */
		int p2cHandles[2];
		if(pipe(p2cHandles)){
			syslog(LOG_ERR, "failed to create send pipe.");
			return (Handles){-1, -1};
		}
		/** 子→親のパイプ生成
		 * c2pHandles[0] ： 親では読みだす／子では使わない
		 * c2pHandles[1] ： 親では使わない／子で1番に複製
		 */
		int c2pHandles[2];
		if(pipe(c2pHandles)){
			syslog(LOG_ERR, "failed to create receive pipe");
			close(p2cHandles[0]);
			close(p2cHandles[1]);
			return (Handles){-1, -1};
		}

		//子プロセス起動
		const pid_t pid(fork());
		if(!pid){
			//子プロセスなので相手方パイプをハンドル番号0へ
			if(dup2(p2cHandles[0], 0) < 0 || dup2(c2pHandles[1], 1) < 0){
				syslog(LOG_CRIT, "failed to duplicate handles of pipe.");
				_exit(-1);
			}
			//不要ハンドルを閉じる
			close(p2cHandles[1]);
			close(c2pHandles[0]);

			//対象起動
			execvp(t, const_cast<char* const*>(params));
			syslog(LOG_CRIT, "failed to start process:%s.", t);

			_exit(-1);
		}else if(pid < 0){
			syslog(LOG_CRIT, "failed to fork.");
			return (Handles){-1, -1};
		}

		//不要ハンドルを閉じる
		close(p2cHandles[0]);
		close(c2pHandles[1]);

		Handles pipes = { c2pHandles[0], p2cHandles[1] };

		return pipes;
	}
}
