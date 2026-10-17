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

/******************************************
 structure Oct.26
NNID[16] C
DATUM[10] C
OBA[50] C
OBA_WERT[50] C
NAME[200] C
SPRACHE[15] C
GENUS[10] C
NAME2[200] C
SPRACHE2[15] C
GENUS2[10] C
ZUSATZ[100] C
AGS[8] C
ARS[12] C
HOEHE[10] N
HOEHE_GER[10] N
EWZ[10] N
EWZ_GER[10] N
GEWK[20] C
GEMTEIL[10] C
VIRTUELL[10] C
GEMEINDE[50] C
VERWGEM[50] C
KREIS[50] C
REGBEZIRK[50] C
BUNDESLAND[50] C
STAAT[50] C
RECHTS[108] N
HOCH[108] N
BOX[250] C
*************************************************************/

#include "DbfFile.hpp"

class Gn250
{
public:
    Gn250(const std::string& baseName);
    virtual ~Gn250() = default;
    std::string getPrj();
    bool next();
    void reset();
    std::string getNnid() {
        return m_record->getValue(nnid);
    }
    std::string getDatum() {
        return m_record->getValue(datum);
    }
    std::string getOba() {
        return m_record->getValue(oba);
    }
    std::string getOba_wert() {
        return m_record->getValue(oba_wert);
    }
    std::string getName() {
        return m_record->getValue(name);
    }
    std::string getSprache() {
        return m_record->getValue(sprache);
    }
    std::string getGenus() {
        return m_record->getValue(genus);
    }
    std::string getName2() {
        return m_record->getValue(name2);
    }
    std::string getSprache2() {
        return m_record->getValue(sprache2);
    }
    std::string getGenus2() {
        return m_record->getValue(genus2);
    }
    std::string getZusatz() {
        return m_record->getValue(zusatz);
    }
    std::string getAgs() {
        return m_record->getValue(ags);
    }
    std::string getArs() {
        return m_record->getValue(ars);
    }
    double getHoehe() {
        return m_record->getDouble(hoehe);
    }
    double getHoehe_ger() {
        return m_record->getDouble(hoehe_ger);
    }
    double getEwz() {
        return m_record->getDouble(ewz);
    }
    double getEwz_ger() {
        return m_record->getDouble(ewz_ger);
    }
    std::string getGewk() {
        return m_record->getValue(gewk);
    }
    std::string getGemteil() {
        return m_record->getValue(gemteil);
    }
    std::string getVirtuell() {
        return m_record->getValue(virtuell);
    }
    std::string getGemeinde() {
        return m_record->getValue(gemeinde);
    }
    std::string getVerwgem() {
        return m_record->getValue(verwgem);
    }
    std::string getKreis() {
        return m_record->getValue(kreis);
    }
    std::string getRegbezirk() {
        return m_record->getValue(regbezirk);
    }
    std::string getBundesland() {
        return m_record->getValue(bundesland);
    }
    std::string getStaat() {
        return m_record->getValue(staat);
    }
    double getRechts() {
        return m_record->getDouble(rechts);
    }
    double getHoch() {
        return m_record->getDouble(hoch);
    }
    std::string getBox() {
        return m_record->getValue(box);
    }
    psc::dbf::PtrDbfRecord getRecord()
    {
        return m_record;
    }
    std::vector<psc::dbf::PtrDbfColumn> getColumns();
private:
    std::string m_baseName;
    std::shared_ptr<psc::dbf::DbfFile> m_dbfFile;
    psc::dbf::PtrDbfColumn nnid;
    psc::dbf::PtrDbfColumn datum;
    psc::dbf::PtrDbfColumn oba;
    psc::dbf::PtrDbfColumn oba_wert;
    psc::dbf::PtrDbfColumn name;
    psc::dbf::PtrDbfColumn sprache;
    psc::dbf::PtrDbfColumn genus;
    psc::dbf::PtrDbfColumn name2;
    psc::dbf::PtrDbfColumn sprache2;
    psc::dbf::PtrDbfColumn genus2;
    psc::dbf::PtrDbfColumn zusatz;
    psc::dbf::PtrDbfColumn ags;
    psc::dbf::PtrDbfColumn ars;
    psc::dbf::PtrDbfColumn hoehe;
    psc::dbf::PtrDbfColumn hoehe_ger;
    psc::dbf::PtrDbfColumn ewz;
    psc::dbf::PtrDbfColumn ewz_ger;
    psc::dbf::PtrDbfColumn gewk;
    psc::dbf::PtrDbfColumn gemteil;
    psc::dbf::PtrDbfColumn virtuell;
    psc::dbf::PtrDbfColumn gemeinde;
    psc::dbf::PtrDbfColumn verwgem;
    psc::dbf::PtrDbfColumn kreis;
    psc::dbf::PtrDbfColumn regbezirk;
    psc::dbf::PtrDbfColumn bundesland;
    psc::dbf::PtrDbfColumn staat;
    psc::dbf::PtrDbfColumn rechts;
    psc::dbf::PtrDbfColumn hoch;
    psc::dbf::PtrDbfColumn box;
    psc::dbf::PtrDbfRecord m_record;
    int m_pos{};
    int m_recordCount;
};

static constexpr auto MIN_POPULATION_EXPORT_GEO_JSON{10000.0};
static constexpr auto LONGITUE_NAME{"LONGITUDE"};
static constexpr auto LATITUDE_NAME{"LATITUDE"};
static constexpr auto LONLATHEIGHT_NAME{"LONLATHEIGHT"};