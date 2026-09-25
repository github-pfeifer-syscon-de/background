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
#pragma once

#include <memory>

#include "BackConfig.hpp"
#include "Layout.hpp"
#include "Module.hpp"
#include "FileLoader.hpp"

class StarWin;

class ModulePaint {
public:
    ModulePaint(StarWin* starWin);
    explicit ModulePaint(const ModulePaint& other) = delete;
    virtual ~ModulePaint() = default;

    std::vector<PtrModule> createModules();
    std::vector<PtrModule> getModules();
    void drawModules(Cairo::RefPtr<Cairo::Context>& ctx
        , Layout& layout);


    std::vector<PtrModule> findModules(const char* pos);
protected:
    void drawTop(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules);
    void drawMiddle(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules);
    void drawBottom(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules);


    StarWin* m_starWin;
    std::shared_ptr<BackConfig> m_config;
    std::vector<PtrModule> m_modules;
    std::shared_ptr<FileLoader> m_fileLoader;
};
