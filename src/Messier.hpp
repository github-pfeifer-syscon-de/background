/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf
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

#include <string>
#include <glibmm.h>

#include "Star.hpp"

class Messier
: public Star
{
public:
    Messier();
    explicit Messier(const Messier& orig) = delete;
    virtual ~Messier() = default;

    std::shared_ptr<RaDec> getRaDec();
    void setRaDec(const std::shared_ptr<RaDec>& raDec);

    double getVmagnitude() const override;
    void setVmagnitude(double vmagnitude);
    void setName(const Glib::ustring& ident);
    Glib::ustring getName();
private:
    Glib::ustring m_name;
    double m_vmagnitude{};
    std::shared_ptr<RaDec> raDec;
};

