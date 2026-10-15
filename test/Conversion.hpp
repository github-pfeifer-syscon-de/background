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
#include <cmath>   // for HUGE_VAL
#include <proj/coordinateoperation.hpp>
#include <proj/crs.hpp>
#include <proj/io.hpp>
#include <proj/util.hpp> // for nn_dynamic_pointer_cast

// real geo-coordinate conversion
namespace psc::geo {

class Transform;


class Conversion {
public:
    Conversion();
    explicit Conversion(const Conversion& other) = delete;
    virtual ~Conversion() = default;

    // use with e.g."EPSG"
    NS_PROJ::io::AuthorityFactoryPtr getFactory(const std::string& factory);
    // use with e.g. "4326"
    NS_PROJ::crs::CRSNNPtr createCRS(NS_PROJ::io::AuthorityFactoryPtr& authFactory, const std::string& crs);
    // use with e.g. "+proj=utm +zone=32 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs"
    //   or wkt can used like almost anything else
    //   https://proj.org/en/stable/development/reference/cpp/io.html
    NS_PROJ::crs::CRSNNPtr createFromUser(const std::string& userInput);

    std::shared_ptr<Transform> getTranform(
        NS_PROJ::crs::CRSNNPtr& srcCrs,
        NS_PROJ::crs::CRSNNPtr& targetCrs);

private:
    NS_PROJ::io::DatabaseContextNNPtr m_dbContext;
    NS_PROJ::io::AuthorityFactoryNNPtr m_authFactory;
    NS_PROJ::operation::CoordinateOperationContextNNPtr m_coord_op_ctxt;
};

class Transform
{
public:
    Transform();
    virtual ~Transform();
    virtual PJ_COORD convert(PJ_COORD& coord) = 0;
protected:
    PJ_CONTEXT* m_ctx;
};

class TransformCrs
: public Transform
{
public:
    TransformCrs(NS_PROJ::operation::CoordinateOperationNNPtr& transf);
    virtual  ~TransformCrs();
    PJ_COORD convert(PJ_COORD& coord);
private:
    NS_PROJ::operation::CoordinateTransformerNNPtr m_transformer;
};

// allows creating from e.g. wkt/prj string to e.g. EPSG:4326
//  use C-Api for reference
class TransformWkt
: public Transform
{
public:
    TransformWkt(const std::string& src, const std::string& target);
    virtual  ~TransformWkt();
    PJ_COORD convert(PJ_COORD& in);
    PJ_COORD reverseConvert(PJ_COORD& in);

private:
    PJ *m_pj;
};


using PtrTransform = std::shared_ptr<Transform>;

} /* namespace psc::geo */
