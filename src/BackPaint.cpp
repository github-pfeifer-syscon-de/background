/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2026 RPf
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


#include "StarWin.hpp"
#include "background_config.h"
#include "BackPaint.hpp"
#include "ClockModule.hpp"
#include "InfoModule.hpp"
#include "CalendarModule.hpp"


BackPaint::BackPaint(StarWin* starWin)
: m_starWin{starWin}
{
    m_fileLoader = starWin->getFileLoader();

    m_config = m_starWin->getConfig();
    m_modules = createModules();
}

std::vector<PtrModule>
BackPaint::createModules()
{
#   ifdef USE_PYTHON
    auto pyWrapper = std::make_shared<PyWrapper>();
#   else
    auto pyWrapper = std::shared_ptr<PyWrapper>();
#   endif
    std::vector<PtrModule> mods;
    mods.reserve(4);
    mods.emplace_back(
        std::move(
            std::make_shared<InfoModule>(m_config, pyWrapper)));
    mods.emplace_back(
        std::move(
            std::make_shared<ClockModule>(m_config, pyWrapper)));
    mods.emplace_back(
        std::move(
            std::make_shared<CalendarModule>(m_config, pyWrapper)));
    return mods;
}

std::vector<PtrModule>
BackPaint::getModules()
{
    return m_modules;
}


void
BackPaint::drawModules(Cairo::RefPtr<Cairo::Context>& ctx
    , Layout& layout)
{
    drawTop(ctx, layout, findModules(Module::POS_TOP));
    drawMiddle(ctx, layout, findModules(Module::POS_MIDDLE));
    drawBottom(ctx, layout, findModules(Module::POS_BOTTOM));
}

std::vector<PtrModule>
BackPaint::findModules(const char* pos)
{
    std::vector<PtrModule> mods;
    mods.reserve(m_modules.size());
    for (auto& mod : m_modules) {
        if (mod->getPosition() == pos) {
            mods.push_back(mod);
        }
    }
    return mods;
}

void
BackPaint::drawTop(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules)
{
    Point2D pos(40.0, 20.0);
    for (auto& mod : modules) {
        ctx->save();
        ctx->translate(pos.getX(), pos.getY());
        mod->display(ctx, m_starWin);
        ctx->restore();
        Point2D p(0.0, mod->getHeight(ctx, m_starWin));
        pos.add(p);
    }
}

void
BackPaint::drawMiddle(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules)
{
    int sumHeight{};
    for (auto& mod : modules) {
        sumHeight += mod->getHeight(ctx, m_starWin);
    }
    Point2D pos(40.0, (layout.getHeight() - sumHeight) / 2.0);
    for (auto& mod : modules) {
        ctx->save();
        ctx->translate(pos.getX(), pos.getY());
        mod->display(ctx, m_starWin);
        ctx->restore();
        Point2D p(0.0, mod->getHeight(ctx, m_starWin));
        pos.add(p);
    }
}

void
BackPaint::drawBottom(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules)
{
    int sumHeight{0};
    for (auto& mod : modules) {
        sumHeight += mod->getHeight(ctx, m_starWin);
    }
    Point2D pos(40.0, layout.getHeight() - sumHeight - 20.0);
    for (auto& mod : modules) {
        ctx->save();
        ctx->translate(pos.getX(), pos.getY());
        mod->display(ctx, m_starWin);
        ctx->restore();
        Point2D p(0.0, mod->getHeight(ctx, m_starWin));
        pos.add(p);
    }
}

void
BackPaint::scale(Pango::FontDescription& starFont, double scale)
{
    starFont.set_size(static_cast<int>(starFont.get_size() * scale));
}

void
BackPaint::brighten(Gdk::RGBA& color, double factor)
{
    color.set_red(color.get_red() * factor);
    color.set_green(color.get_green() * factor);
    color.set_blue(color.get_blue() * factor);
}

std::shared_ptr<KeyConfig>
BackPaint::getConfig()
{
    return m_config;
}
