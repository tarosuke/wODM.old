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
#include "widget.h"

#include <syslog.h>

#include "../app/app.h"

#include <texturedWidget.h>



namespace wodm{

	// r_coreの各種Widgetに対応したwODM対応Widget
	template<class W> class SomeWidget : public W, public Widget{
	public:
		SomeWidget(const Param& p) :
			W(p.parent, p.x, p.y, p.w, p.h,
				VRWAttr(p.attr)),
			Widget(p.app, p.pack.head.id){};

	private:
		void OnMessage(wO::Message& m) final{
			wO::Widget::CommandPack& pack(m);
			switch(pack.head.type){
			case wO::Message::moveTo:
				vr_core::PositionWidget::MoveTo(
					pack.left, pack.top, pack.depth);
				break;
			case wO::Message::jumpTo:
				vr_core::PositionWidget::JumpTo(
					pack.left, pack.top, pack.depth);
				break;
			case wO::Message::move:
				vr_core::PositionWidget::Move(
					pack.left, pack.top, pack.depth);
				break;
			case wO::Message::jump:
				vr_core::PositionWidget::Jump(
					pack.left, pack.top, pack.depth);
				break;
#if 0
			case wO::Message::focus:
				Focused(*this);
				break;
			case wO::Message::pick:
				Pick();
				break;
			case wO::Message::show:
				visible = true;
				Pick();
				break;
			case wO::Message::hide:
				visible = false;
				ReDepth();
				break;
#endif
			default:
				syslog(
					LOG_WARNING,
					"--unknown(%x:%08x)--\n",
					pack.head.type,
					pack.head.id);
//                m.Dump();
				break;
			}
		};

	};


	Widget* Widget::Widget::New(
		App& app,
		vr_core::Widget* parent,
		const wO::Widget::CommandPack& pack){

		const Param p = {
			app: app,
			parent: parent,
			pack: pack,
			x: pack.left,
			y: pack.top,
			z: pack.depth,
			w: pack.width,
			h: pack.height,
		};

		if(p.pack.attributes & wO::Widget::hasContent){
			//中身あり
			new SomeWidget<vr_core::TexturedWidget>(p);
        }else if(p.pack.attributes & wO::Widget::hasBorder){
			//境界あり
			new SomeWidget<vr_core::BorderWidget>(p);
        }else if(p.pack.attributes & wO::Widget::hasPosition){
			//位置のみ
			new SomeWidget<vr_core::PositionWidget>(p);
        }
		return 0;
	}


	Widget::Widget(App& app, unsigned id) :
		Resource(app, id){}

	unsigned Widget::VRWAttr(unsigned woAttr){
		unsigned attr(0);

		// フォーカスしない
		if(woAttr & wO::Widget::noFocusMe){
			attr |= vr_core::Widget::noFocus;
		}

		return attr;
	}

}
