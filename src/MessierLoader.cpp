/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
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

#include <string>
#include <charconv>
#include <iostream>

#include "MessierLoader.hpp"


MessierLoader::MessierLoader(const std::shared_ptr<FileLoader>& fileLoader)
: m_fileLoader{fileLoader}
{

}

// convert H:M:S.SS or D:M:S.SS to decimal
double
MessierLoader::toDecimal(const Glib::ustring& xms)
{
    double decimal{};
    auto fullEnd = xms.find(':');
    if (fullEnd != xms.npos) {
        auto minStart = fullEnd + 1;
        auto minEnd = xms.find(':', minStart);
        if (minEnd != xms.npos) {
            auto min = std::stoi(xms.substr(minStart, minEnd - minStart));
            ++minEnd;
            double sec;
            auto secs = xms.substr(minEnd);
            auto [ptr, ec] = std::from_chars(secs.data(), secs.data() + secs.length(), sec);
            if (ec == std::errc()) {
                auto full = std::stoi(xms.substr(0, fullEnd));
                decimal = full;
                auto sign = full < 0 ? -1 : 1;
                decimal += static_cast<double>(min * sign) / 60.0;
                decimal += (sec * static_cast<double>(sign)) / 3600.0;
            }
        }
    }
    return decimal;
}

std::shared_ptr<Messier>
MessierLoader::readMessier(JsonObject* m, JsonHelper& jsonHelper)
{
    std::shared_ptr<Messier> messier;
    auto id = json_object_get_int_member(m, "messierNumber");
    if (id > 0) {
        auto mn = Glib::ustring::sprintf("M%d", id);
        auto magnitude = json_object_get_double_member(m, "magnitude");
        auto raHMS = Glib::ustring(json_object_get_string_member(m, "rightAscension"));
        auto decDMS = Glib::ustring(json_object_get_string_member(m, "declination"));
        auto dRaH = toDecimal(raHMS);
        auto dDecD = toDecimal(decDMS);
        //std::cout << mn << " ra " << dRaH << " dec " << dDecD << " mag " << magnitude << std::endl;
        if (dRaH != 0.0 || dDecD != 0.0) {
            messier = std::make_shared<Messier>();
            auto raDec = std::make_shared<RaDec>();
            raDec->setRaHours(dRaH);
            raDec->setDecDegrees(dDecD);
            messier->setRaDec(raDec);
            messier->setName(mn);
            messier->setVmagnitude(magnitude);
        }
    }
    return messier;
}

std::list<std::shared_ptr<Messier>>
MessierLoader::getMessiers()
{
    if (m_messier.empty()) {
        m_messier = readObjects();
    }
    return m_messier;
}

std::list<std::shared_ptr<Messier>>
MessierLoader::readObjects()
{
    std::list<std::shared_ptr<Messier>> messiers;
    auto file = m_fileLoader->find(messierDataFile);
    if (!file.empty()) {
        try {
            JsonHelper jsonHelper;
            jsonHelper.load_from_file(file);
            //std::cout << "MessierLoader::readObjects file " << file << std::endl;
            JsonObject* root = jsonHelper.get_root_object();
            JsonObject* data = jsonHelper.get_object(root, "data");
            auto keys = jsonHelper.get_keys(data);
            //std::cout << "MessierLoader::readObjects keys " << keys.size() << std::endl;
            for (auto key : keys) {
                //std::cout << "MessierLoader::readObjects key " << key << std::endl;
                JsonObject* messierJ = jsonHelper.get_object(data, key);
                auto messier = readMessier(messierJ, jsonHelper);
                if (messier) {
                    messiers.emplace_back(std::move(messier));
                }
                else {
                    std::cout << "Messier object " << key << " not loaded!" << std::endl;
                }
            }
        }
        catch (const JsonException& exc) {
            std::cout << "The messier data was not be loaded exception " << exc.what() << "!" << std::endl;
        }
    }
    else {
        std::cout << "The messier data " << messierDataFile << " was not found!" << std::endl;
    }
    return messiers;
}