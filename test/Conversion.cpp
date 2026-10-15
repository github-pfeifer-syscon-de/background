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

#include <iostream>
#include "Conversion.hpp"

namespace psc::geo {

Conversion::Conversion()
: m_dbContext{osgeo::proj::io::DatabaseContext::create()}
, m_authFactory{osgeo::proj::io::AuthorityFactory::create(m_dbContext, std::string())}
, m_coord_op_ctxt {osgeo::proj::operation::CoordinateOperationContext::create(m_authFactory, nullptr, 0.0)}
{

    // Instantiate a generic authority factory, that is not tied to a particular
    // authority, to be able to get transformations registered by different
    // authorities. This can only be used for CoordinateOperationContext.

    // Create a coordinate operation context, that can be customized to amend
    // the way coordinate operations are computed. Here we ask for default
    // settings.
}


NS_PROJ::io::AuthorityFactoryPtr
Conversion::getFactory(const std::string& factory)
{
    // Instantiate a authority factory for e.g. EPSG related objects.
    return osgeo::proj::io::AuthorityFactory::create(m_dbContext, factory);
}


NS_PROJ::crs::CRSNNPtr
Conversion::createCRS(NS_PROJ::io::AuthorityFactoryPtr& authFactory, const std::string& crs)
{
    // Instantiate source CRS from EPSG code
    return authFactory->createCoordinateReferenceSystem(crs);
}

NS_PROJ::crs::CRSNNPtr
Conversion::createFromUser(const std::string& userInput)
{
    // Instantiate target CRS from PROJ.4 string (commented out, the equivalent
    // from the EPSG code)
    // auto targetCRS =
    // authFactoryEPSG->createCoordinateReferenceSystem("32631");
    return NN_CHECK_THROW(
                nn_dynamic_pointer_cast<osgeo::proj::crs::CRS>
                    (createFromUserInput(userInput, m_dbContext)));
    ///       "+proj=utm +zone=31 +datum=WGS84 +type=crs", dbContext)));
    // from https://epsg.com.tr/en/25832 e.g. ("EPSG:25832/33");
    // matches https://epsg.io/25832
}

std::shared_ptr<Transform>
Conversion::getTranform(
    NS_PROJ::crs::CRSNNPtr& srcCrs,
    NS_PROJ::crs::CRSNNPtr& targetCrs)
{
    // List operations available to transform from EPSG:4326
    // (WGS 84 latitude/longitude) to EPSG:32631 (WGS 84 / UTM zone 31N).
    auto list =
        osgeo::proj::operation::CoordinateOperationFactory::create()->createOperations(
         srcCrs, targetCrs, m_coord_op_ctxt);

    // Check that we got a non-empty list of operations
    // The list is sorted from the most relevant to the less relevant one.
    // Cf
    // https://proj.org/operations/operations_computation.html#filtering-and-sorting-of-coordinate-operations
    // for more details on the sorting of those operations.
    // For a transformation between a projected CRS and its base CRS, like
    // we do here, there will be only one operation.
    if (list.empty()){
        return std::shared_ptr<Transform>();
    }
    // Create a coordinate transformer from the first operation of the list
    return std::make_shared<TransformCrs>(list[0]);
}

Transform::Transform()
: m_ctx{proj_context_create()}
{
// Create an execution context (must only be used by one thread at a time)
}

Transform::~Transform()
{
    if (m_ctx != nullptr) {    // Destroy execution context
        proj_context_destroy(m_ctx);
        m_ctx = nullptr;
    }
}

TransformCrs::TransformCrs(NS_PROJ::operation::CoordinateOperationNNPtr& transf)
: Transform()
, m_transformer{transf->coordinateTransformer(m_ctx)}
{
}

TransformCrs::~TransformCrs()
{
}

PJ_COORD
TransformCrs::convert(PJ_COORD& coord)
{
    // Perform the coordinate transformation.
    return m_transformer->transform(coord);
}


TransformWkt::TransformWkt(const std::string& src, const std::string& target)
: Transform()
{
    //PJ_GUESSED_WKT_DIALECT dial = proj_context_guess_wkt_dialect(m_ctx, wkt.c_str());
    //if (dial == PJ_GUESSED_NOT_WKT) {
    //    return nullptr;
    //}
    //PROJ_STRING_LIST opt;
    //PROJ_STRING_LIST out_warnings;
    //PROJ_STRING_LIST out_grammar_errors;
    //PJ* pj = proj_create_from_wkt(m_ctx, wkt.c_str(), opt, &out_warnings, &out_grammar_errors);
    //PJ* pj = proj_create(m_ctx, wkt.c_str());      // this works
    // simplest c-api
    m_pj = proj_create_crs_to_crs(m_ctx, src.c_str(), target.c_str(),0 );
}

TransformWkt::~TransformWkt()
{
    if (m_pj != nullptr) {
        proj_destroy(m_pj);
        m_pj = nullptr;
    }
}

PJ_COORD
TransformWkt::convert(PJ_COORD& in)
{
    PJ_COORD out = proj_trans(m_pj, PJ_DIRECTION::PJ_FWD, in);
    //std::cout << "result " << out.xy.x << " " << out.xy.y << std::endl;
    return out;
}

PJ_COORD TransformWkt::reverseConvert(PJ_COORD& in)
{
    PJ_COORD out = proj_trans(m_pj, PJ_DIRECTION::PJ_INV, in);
    //std::cout << "result " << out.xy.x << " " << out.xy.y << std::endl;
    return out;
}


} /* namespace psc::geo */
