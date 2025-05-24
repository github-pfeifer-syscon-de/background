/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf <gpl3@pfeifer-syscon.de>
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

#pragma once

#include "Module.hpp"

class InfoModule
: public Module
{
public:
    InfoModule(const std::shared_ptr<KeyConfig>& config, const std::shared_ptr<PyWrapper>& pyWrapper)
    : Module{"info", config, pyWrapper}
    {
    }
    explicit InfoModule(const InfoModule& orig) = delete;
    virtual ~InfoModule() = default;

    int getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) override;
    void display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) override;
    void setupParam(const Glib::RefPtr<Gtk::Builder>& builder, StarDraw* starDraw) override;
    void saveParam(bool save) override;
    Glib::ustring getPyScriptName() override;
    static constexpr auto pyClassName{"Info"};
protected:
    std::string getText(SysInfo& sysInfo);

private:
    Gtk::ColorButton* m_infoColor;
    Gtk::FontButton* m_infoFont;
    Gtk::ComboBoxText* m_infoPos;
};