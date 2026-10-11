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

#include "Conversion.hpp"
#include "gn250_conv.hpp"

Gn250::Gn250(const std::string& baseName)
: m_baseName{baseName}
{
    m_dbfFile = std::make_shared<psc::dbf::DbfFile>(baseName+ ".dbf");
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

bool
Gn250::next() {
    if (m_pos < m_recordCount) {
        m_record->read(m_pos++);
        return true;
    }
    return false;
}

/*
GN250_p.* Darstellung als Punktgeometrie
GN250_b.* Darstellung als Bounding-B
GN250_n.dbf
AX_Regierungsbezirk Gießen ags 06500000
   utm 483990.343 5612601.691
   geo 50.66504 8.77347
   popul. 1040290 0
AX_KreisRegion Gießen ags 06531000
   utm 486372.070 5602054.845
   geo 50.57025 8.80756
   popul. 268418 0
AX_Ortslage Gießen ags 06531005
   utm 476983.945 5603860.350
   geo 50.58620 8.67488
   popul. 0 59560
AX_Gemeinde Gießen ags 06531005
   utm 476983.947 5603860.301
   geo 50.58620 8.67488
   popul. 89179 0
AX_Verwaltungsgemeinschaft Gießen ags 06531000
   utm 478319.231 5603260.928
   geo 50.58086 8.69377
   popul. 89179 0
Gewaesser Gießen ags
   utm 580804.849 5336383.370
   geo 48.17555 10.08694
   popul. 0 0
AX_Bahnverkehrsanlage Gießen ags
   utm 475901.810 5602984.853
   geo 50.57828 8.65965
   popul. 0 0
---
AX_Ortslage Lich ags 06531011
   utm 487176.329 5596392.465
   geo 50.51935 8.81911
   popul. 0 4620
AX_Gemeinde Lich ags 06531011
   utm 487176.335 5596392.519
   geo 50.51935 8.81911
   popul. 14167 0
AX_Verwaltungsgemeinschaft Lich ags 06531000
   utm 488110.489 5595754.895
   geo 50.51363 8.83231
   popul. 14167 0
---
AX_Gemeinde Altendiez ags 07141002
   utm 427699.667 5580369.354
   geo 50.37094 7.98331
   popul. 2175 0
AX_Ortslage Altendiez ags 07141002
   utm 427699.693 5580369.373
   geo 50.37094 7.98331
   popul. 0 2148
---
AX_Ortslage Katzenelnbogen ags 07141068
   utm 427049.256 5568888.831
   geo 50.26763 7.97639
   popul. 0 2240
AX_Gemeinde Katzenelnbogen ags 07141068
   utm 427049.254 5568888.776
   geo 50.26763 7.97638
   popul. 2287 0
*/
// -> use AX_Gemeinde

static void
read_dbf(const std::string& dir)
{
    psc::geo::Conversion converter;
    auto epgs = converter.getFactory("EPSG");
    auto epgs4326 = converter.createCRS(epgs,"4326");
    // check proj... to match definition here:
    //
    auto utm32 = converter.createFromUser("+proj=utm +zone=32 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs");
    //auto utm33 = converter.createFromUser("+proj=utm +zone=33 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs");
    auto transf32 = converter.getTranform(utm32, epgs4326);
    // the zone33 may be an unused option ... as far as i can see
    //auto transf33 = converter.getTranform(utm33, epgs4326);
    if (!transf32) {
        std::cout << "Could not init convert!" << std::endl;
        return;
    }
    std::string basename = "GN250_p";
    Gn250 gn250(dir + basename );
    auto proj = gn250.getPrj();
    if (proj != "PROJCS[\"ETRS_1989_UTM_Zone_32N\",GEOGCS[\"GCS_ETRS_1989\",DATUM[\"D_ETRS_1989\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",9.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0],AUTHORITY[\"EPSG\",25832]]") {
        std::cout << "The expected projection info was not found " << proj << std::endl;
        return;
    }
    auto root = std::make_shared<psc::json::JsonObj>();
    root->set("type", "FeatureCollection");
    auto features = root->createArr("features", 0);
    while (gn250.next()) {
        auto name = gn250.getName();
        auto bundesland = gn250.getBundesland();
        auto oba = gn250.getOba();
        auto ewz = gn250.getEwz();
        if (oba == "AX_Gemeinde" && ewz > MIN_POPULATION_EXPORT_GEO_JSON) {       // (name == "Altendiez" || name == "Lich" || name == "Gießen" || name ==  "Katzenelnbogen"
             auto dRe = gn250.getRechts();
             auto dHo = gn250.getHoch();
             PJ_COORD c = {{
                 //    49.0,    // latitude in degree
                 //    2.0,     // longitude in degree
                 dRe,
                 dHo,
                 0.0,     // z ordinate. unused
                 HUGE_VAL // time ordinate. unused
             }};
             c = transf32->convert(c);
             //auto ewz_ger = gn250.getEwz_ger();     // errechnete Einwohnerzahl gilt für AX_Ortslage
             auto ags = gn250.getAgs();
             std::cout << std::format("{} {} ags {}\n   utm {:.3f} {:.3f}\n   geo {:.5f} {:.5f}\n   popul. {:.0f} "
                  , oba, name, ags, dRe,dHo,c.v[0],c.v[1], ewz) << std::endl;
             auto feature = std::make_shared<psc::json::JsonObj>();
             // need to ref to keep refcount. as set_array_member "takes" the reference, crashes without
             auto refObj = json_object_ref(feature->getObj());
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
    auto jsonOut = root->generate(2);
    std::ofstream file("gn250.geo.json");
    if (file.is_open()) {
        file.write(jsonOut.c_str(), jsonOut.bytes());
        file.close();
    }
    else {
        std::cout << "Failed to open exp.json" << std::endl;
    }
}
/*
 *
 */
int main(int argc, char** argv)
{
    std::setlocale(LC_ALL, "");      // make locale dependent, and make glib accept u8 const !!!
    Glib::init();
    if (argc >= 2) {
        read_dbf(argv[1]);
    }
    else {
        std::cout << "Provide directory from download as argument" << std::endl;
    }
}