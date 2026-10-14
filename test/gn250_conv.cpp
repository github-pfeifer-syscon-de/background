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
#include <psc_i18n.hpp>
#include <glibmm.h>
#include <format>
#include <string>
#include <fstream>
#include <GeoCoordinate.hpp>
#include <JsonObj.hpp>
#include <JsonHelper.hpp>
#include <filesystem>

#include "Conversion.hpp"
#include "gn250_conv.hpp"
#include "background_config.h"
#ifdef USE_SQLLITE
#include "../src/SqlLite.hpp"
#endif

Gn250::Gn250(const std::string& basename)
: m_baseName{basename}
{
    m_dbfFile = std::make_shared<psc::dbf::DbfFile>(m_baseName+ ".dbf");
    for (size_t col = 0; col < m_dbfFile->getNumCols(); ++col) {
        auto column = m_dbfFile->getColumn(col);
        auto colName = column->getName();
        if (colName == "NNID")             { nnid = column; }
        else if (colName == "DATUM")       { datum = column; }
        else if (colName == "OBA")         { oba = column; }
        else if (colName == "OBA_WERT")    { oba_wert = column; }
        else if (colName == "NAME")        { name = column; }
        else if (colName == "SPRACHE")     { sprache = column; }
        else if (colName == "GENUS")       { genus = column; }
        else if (colName == "NAME2")       { name2 = column; }
        else if (colName == "SPRACHE2")    { sprache2 = column; }
        else if (colName == "GENUS2")      { genus2 = column; }
        else if (colName == "ZUSATZ")      { zusatz = column; }
        else if (colName == "AGS")         { ags = column; }
        else if (colName == "ARS")         { ars = column; }
        else if (colName == "HOEHE")       { hoehe = column; }
        else if (colName == "HOEHE_GER")   { hoehe_ger = column; }
        else if (colName == "EWZ")         { ewz = column; }
        else if (colName == "EWZ_GER")     { ewz_ger = column; }
        else if (colName == "GEWK")        { gewk = column; }
        else if (colName == "GEMTEIL")     { gemteil = column; }
        else if (colName == "VIRTUELL")    { virtuell = column; }
        else if (colName == "GEMEINDE")    { gemeinde = column; }
        else if (colName == "VERWGEM")     { verwgem = column; }
        else if (colName == "KREIS")       { kreis = column; }
        else if (colName == "REGBEZIRK")   { regbezirk = column; }
        else if (colName == "BUNDESLAND")  { bundesland = column; }
        else if (colName == "STAAT")       { staat = column; }
        else if (colName == "RECHTS")      { rechts = column; }
        else if (colName == "HOCH")        { hoch = column; }
        else if (colName == "BOX")         { box = column; }
    }
    m_record = m_dbfFile->getRecord();
    m_recordCount = m_dbfFile->getNumRows();
}

std::string
Gn250::getPrj() {
    auto proj = m_baseName + ".prj";
    std::ifstream file(proj);
    if (!file.is_open()) {
        return "Failed to open file: " + proj;
    }
    std::string line;
    getline(file, line);
    file.close();
    return line;
}

void
Gn250::reset()
{
    m_pos = 0;
}

bool
Gn250::next() {
    if (m_pos < m_recordCount) {
        m_record->read(m_pos++);
        return true;
    }
    return false;
}

std::vector<psc::dbf::PtrDbfColumn>
Gn250::getColumns() {
    std::vector<psc::dbf::PtrDbfColumn> ret;
    ret.reserve(m_dbfFile->getNumCols());
    for (size_t col = 0; col < m_dbfFile->getNumCols(); ++col) {
        auto column = m_dbfFile->getColumn(col);
        ret.emplace_back(std::move(column));
    }
    return ret;
}

#ifdef USE_SQLLITE
psc::sql::PtrSqlColumn
toSqlite(const psc::dbf::PtrDbfColumn& dbfColumn) {
    psc::sql::PtrSqlColumn sqlColumn;
    bool primaryKey = (dbfColumn->getName() == "NNID");
    if (dbfColumn->getType() == 'C') {
        sqlColumn = std::make_shared<psc::sql::SqlVarcharColumn>(
            dbfColumn->getName()
            , dbfColumn->getSize()
            , true
            , primaryKey);
    }
    else {
        sqlColumn = std::make_shared<psc::sql::SqlNumColumn>(
            dbfColumn->getName()
            , std::min(dbfColumn->getSize(), 18)    // some numerical sizes are a bit off, but at least sqlite doesn't care -> check if a full db will be used
            , std::max(dbfColumn->getDecimals(), 0)
            , true
            , primaryKey);
    }
    return sqlColumn;
}

void
createSqlLite(const psc::geo::PtrTransform& transf32
    , const std::shared_ptr<Gn250> gn250
    , const std::string& db)
{
    psc::sql::SqlLite sqlLite(db);
    try {
        auto dbfCols = gn250->getColumns();
        auto sqlTbl = std::make_shared<psc::sql::SqlTable>("gn250");
        std::vector<psc::sql::PtrSqlColumn> idxColsObaEwz;
        psc::sql::PtrSqlColumn nameColumn;
        for (auto& dbfCol : dbfCols) {
            auto sqlCol = toSqlite(dbfCol);
            sqlTbl->addColumns(sqlCol);
            auto name = dbfCol->getName();
            if (name == "OBA" || name == "EWZ") {
                idxColsObaEwz.push_back(sqlCol);
            }
            else if (name == "NAME") {
                nameColumn = sqlCol;
            }
        }
        auto colLong = std::make_shared<psc::sql::SqlNumColumn>(
            "LONGITUDE"
            , 12, 6
            , true);
        sqlTbl->addColumns(colLong);
        auto colLat = std::make_shared<psc::sql::SqlNumColumn>(
            "LATITUDE"
            , 12, 6
            , true);
        sqlTbl->addColumns(colLat);
        auto indexObaEwz = std::make_shared<psc::sql::SqlIndex>(false, "IDX_OBA_EWZ", idxColsObaEwz);
        sqlTbl->addIndex(indexObaEwz);
        auto indexLat = std::make_shared<psc::sql::SqlIndex>(false, "IDX_LONGITUDE", colLong);
        sqlTbl->addIndex(indexLat);
        auto indexLong = std::make_shared<psc::sql::SqlIndex>(false, "IDX_LATITUDE", colLat);
        sqlTbl->addIndex(indexLong);
        auto indexName = std::make_shared<psc::sql::SqlIndex>(false, "IDX_NAME", nameColumn);
        sqlTbl->addIndex(indexName);
        sqlLite.create(sqlTbl);
        gn250->reset();
        sqlLite.beginTransaction(); // this speeds up import dramatically
        auto ins = sqlLite.insert(sqlTbl);
        while (gn250->next()) {
            auto record = gn250->getRecord();
            auto nnid = gn250->getNnid();
            std::cout << "Converting to sqlite " << nnid << std::endl;
            int n{1};
            for (auto& dbfCol : dbfCols) {
                if (dbfCol->getType() == 'C') {
                    ins->setString(n, record->getValue(dbfCol));
                }
                else {
                    ins->setDouble(n, record->getDouble(dbfCol));
                }
                ++n;
            }
            auto dRe = gn250->getRechts();
            auto dHo = gn250->getHoch();
            PJ_COORD c = {{
                dRe,
                dHo,
                0.0,     // z ordinate. unused
                HUGE_VAL // time ordinate. unused
            }};
            c = transf32->convert(c);
            ins->setDouble(n++, c.v[1]);
            ins->setDouble(n++, c.v[0]);
            if (ins->exec() == SQLITE_CONSTRAINT) {
                std::cout << "Insert constraint violation!" << std::endl;
            }
        }
        sqlLite.commit();
    }
    catch (const std::exception& exc) {
        sqlLite.rollback();
        std::cout << "Error converting to sqlite " << exc.what() << std::endl;
    }
}
#endif


static void
read_dbf(const psc::geo::PtrTransform& transf32
    , const std::shared_ptr<Gn250> gn250
    , const std::string& geoJsonFile)
{
    auto root = std::make_shared<psc::json::JsonObj>();
    root->set("type", "FeatureCollection");
    auto features = root->createArr("features", 0);
    gn250->reset();
    while (gn250->next()) {
        auto name = gn250->getName();
        auto bundesland = gn250->getBundesland();
        auto oba = gn250->getOba();
        auto ewz = gn250->getEwz();
        if (oba == "AX_Gemeinde" && ewz > MIN_POPULATION_EXPORT_GEO_JSON) {       // (name == "Altendiez" || name == "Lich" || name == "Gießen" || name ==  "Katzenelnbogen"
             auto dRe = gn250->getRechts();
             auto dHo = gn250->getHoch();
             PJ_COORD c = {{
                 //    49.0,    // latitude in degree
                 //    2.0,     // longitude in degree
                 dRe,
                 dHo,
                 0.0,     // z ordinate. unused
                 HUGE_VAL // time ordinate. unused
             }};
             c = transf32->convert(c);
             //auto ewz_ger = gn250->getEwz_ger();     // errechnete Einwohnerzahl gilt für AX_Ortslage
             auto ags = gn250->getAgs();
             std::cout << std::format("{} {} ags {}\n   utm {:.3f} {:.3f}\n   geo {:.5f} {:.5f}\n   popul. {:.0f} "
                  , oba, name, ags, dRe,dHo,c.v[0],c.v[1], ewz) << std::endl;
             auto feature = std::make_shared<psc::json::JsonObj>();
             features->add(feature);
             feature->set("type", "Feature");
             auto geometry = feature->createObj("geometry");
             geometry->set("type", "Point");
             auto coords = geometry->createArr("coordinates");
             coords->add({c.v[1], c.v[0]});
             auto properties = feature->createObj("properties");
             properties->set("name", Glib::ustring{name});
             properties->set("population", static_cast<int64_t>(ewz));
             properties->set("region", Glib::ustring{bundesland});
         }
    }
    try {
        root->generate(geoJsonFile, 2);
    }
    catch (const std::exception& exc) {
        std::cout << "Error " << exc.what() << " exporting " << geoJsonFile << std::endl;
    }
}

static psc::geo::PtrTransform
createCrsTransform(const std::string& fromUser, const std::string& auth, const std::string& crs)
{
    psc::geo::Conversion converter;
    auto utm32 = converter.createFromUser(fromUser);
    auto epgs = converter.getFactory(auth);
    auto epgs4326 = converter.createCRS(epgs, crs);
    //auto utm33 = converter.createFromUser("+proj=utm +zone=33 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs");
    auto transf32 = converter.getTranform(utm32, epgs4326);
    // the zone33 may be an unused option ... as far as i can see
    //auto transf33 = converter.getTranform(utm33, epgs4326);
    return transf32;
}

/*
GN250_p.* Darstellung als Punktgeometrie
GN250_b.* Darstellung als Bounding-Box
*/
static std::shared_ptr<Gn250>
openGn250(const std::string& dir)
{
    std::filesystem::path path(dir);
    auto dbfPath = path /= "GN250_p";
    auto dbfFile = dbfPath;
    dbfFile += ".dbf";
    if (!std::filesystem::exists(dbfFile)) {
        std::cout << "The expected dbf file " << dbfFile << " was not found" << std::endl;
        return {};
    }
    auto  gn250 = std::make_shared<Gn250>(dbfPath.string());
    auto proj = gn250->getPrj();
    if (proj != "PROJCS[\"ETRS_1989_UTM_Zone_32N\",GEOGCS[\"GCS_ETRS_1989\",DATUM[\"D_ETRS_1989\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",9.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0],AUTHORITY[\"EPSG\",25832]]") {
        std::cout << "The expected projection info was not found " << proj << std::endl;
        return {};
    }
    return gn250;
}

int main(int argc, char** argv)
{
    std::setlocale(LC_ALL, "");      // make locale dependent, and make glib accept u8 const !!!
    Glib::init();
    if (argc >= 2) {
        psc::geo::PtrTransform transf32 = createCrsTransform(
                   "+proj=utm +zone=32 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs"
                  ,"EPSG", "4326");
        if (!transf32) {
            std::cout << "Could not init convert!" << std::endl;
            return 1;
        }
        auto gn250 = openGn250(argv[1]);
        if (!gn250) {
            std::cout << "Could not load gn250-data!" << std::endl;
            return 2;
        }
        read_dbf(transf32, gn250, "gn250.geo.json");
#       ifdef USE_SQLLITE
        createSqlLite(transf32, gn250, "gn250.sqlite");
#       endif
    }
    else {
        std::cout << "Provide directory from download german gn250 data (containing GN250_p.dbf) as argument" << std::endl;
    }
}
