/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#pragma once

#include <memory>

#include "Star.hpp"
#include "RaDec.hpp"

class HipparcosStar
: public Star
{
public:
    HipparcosStar();
    explicit HipparcosStar(const HipparcosStar& orig) = delete;
    virtual ~HipparcosStar() = default;

    long getNumber() const;

    void setNumber(long number);

    std::shared_ptr<RaDec> getRaDec();

    void setRaDec(const std::shared_ptr<RaDec>& raDec);

    double getRaDegree() const override;

    double getDecDegree() const override;

    double getVmagnitude() const override;

    void setVmagnitude(double vmagnitude);

    void setIdent(const std::string& ident);

    void setFlag(int flag);

private:

    long number{0l};
    std::string ident;
    double vmagnitude{0.0};
    int flag{0};
    std::shared_ptr<RaDec> raDec;

};

