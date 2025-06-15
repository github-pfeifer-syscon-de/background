/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2023 RPf <gpl3@pfeifer-syscon.de>
 *
 * This program is free software: you can redistribute it and/or modify
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
#include <StringUtils.hpp>

#include "BackgroundApp.hpp"
#include "StarDraw.hpp"
#include "ParamDlg.hpp"
#include "TimeDlg.hpp"
#include "StarWin.hpp"
#include "config.h"


StarDraw::StarDraw(BaseObjectType* cobject
                  , const Glib::RefPtr<Gtk::Builder>& /* builder */
                  , StarWin* starWin)
: Gtk::DrawingArea(cobject)
, m_starWin{starWin}
{
    m_starPaint = m_starWin->getStarPaint();
	add_events(Gdk::EventMask::BUTTON_PRESS_MASK);
}

void
StarDraw::update()
{

    auto now = Glib::DateTime::create_now_utc();
    auto pos = m_starWin->getGeoPosition();
    update(now, pos);
}


void
StarDraw::update(Glib::DateTime now, GeoPosition& pos)
{
#   ifdef DEBUG
    std::cout << "StarDraw::update"
              << " date " << now.format_iso8601()
              << " pos " << pos.getLatDegrees() << " " << pos.getLonDegrees()
              << std::endl;
#   endif
    m_displayTimeUtc = now;
    int width = get_allocated_width();
    int height = get_allocated_height();
    if (!m_image
      || m_image->get_width() != width
      || m_image->get_height() != height) {
        m_image = Cairo::ImageSurface::create(Cairo::Format::FORMAT_ARGB32, width, height);
    }
    Layout layout(width, height);
    auto ctx = Cairo::Context::create(m_image);
    m_starPaint->drawImage(ctx, now, pos, layout);
    //std::cout << "draw " << w << " h " << h << "\n";
    queue_draw();
}

bool
StarDraw::on_draw(const Cairo::RefPtr<Cairo::Context>& ctx)
{
    if (!m_image
      || m_image->get_width() != get_allocated_width()
      || m_image->get_height() != get_allocated_height()) {
        update();
    }
    if (m_image) {
        ctx->set_source(m_image, 0, 0);
        ctx->paint();
    }
    return true;
}

StarWin*
StarDraw::getWindow()
{
    return m_starWin;
}

bool
StarDraw::on_button_press_event(GdkEventButton* event)
{
    if (event->button == GDK_BUTTON_SECONDARY) {
		Gtk::Menu* popupMenu = build_popup();
		// deactivate prevent item signals to get generated ...
		// signal_unrealize will never get generated
		popupMenu->attach_to_widget(*this); // this does the trick and calls the destructor
		popupMenu->popup(event->button, event->time);

		return true; // It has been handled.
	}
	return false;
}

Gtk::Menu *
StarDraw::build_popup()
{
	// managed works when used with attach ...
	auto pMenuPopup = Gtk::make_managed<Gtk::Menu>();
    m_starWin->addMenuItems(pMenuPopup);

	pMenuPopup->show_all();
	return pMenuPopup;
}

void
StarDraw::showError(const Glib::ustring& msg, Gtk::MessageType msgType)
{
    if (m_starWin) {
        m_starWin->showMessage(msg, msgType);
    }
    else {
        std::cout << "StarDraw::showError no win " << msg << std::endl;
    }
}
