/* Widget
 * Copyright (C) 2016,2019 tarosuke<webmaster@tarosuke.net>
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

#include <widget.h>

#include <wOLIB/message.h>
#include <wOLIB/widget.h>

#include "../app/app.h"



namespace wodm{

	/** wODM::Widget series Factory
	 */
	class Widget : public App::Resource{
	public:
		struct Param{
			App& app;
			vr_core::Widget* parent;
			const wO::Widget::CommandPack& pack;
			int x;
			int y;
			int z;
			unsigned w;
			unsigned h;
			unsigned attr;
		};

		static Widget* New(
			class App&,
			vr_core::Widget* parent,
			const wO::Widget::CommandPack&);

	protected:
		static unsigned VRWAttr(unsigned);
		Widget(App&, unsigned id);
		void SendEvent(
			wO::Message::Types,
			const vr_core::Widget::MouseEvent&);
		void SendEvent(
			wO::Message::Types,
			const vr_core::Widget::KeyEvent&);
	};

}
