/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
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

#include <iostream>
#include <LocaleContext.hpp>

#include "ConstellationFormat.hpp"

ConstellationFormat::ConstellationFormat(const Gtk::Application& appl)
: m_appl{appl}
{
}

/*
#2345                            visual magnitude of star
#    6                           <blank>
#     78901234                   RA (hours and decimals, J2000.0)
#             5                  <blank>
#              67890123          North polar distance (deg. and dec. J2000.0)
#                      4         <blank>
#                       5678     Bayer greek letter, possibly superscript
#                           9    line weight
#                            012 constellation
*/

void
ConstellationFormat::parseLine(const std::string& line, std::map<std::string, std::shared_ptr<Constellation>>& constellations)
{
	if (line.starts_with('#')) {
	    return;
	}
    //std::cout << "ConstellationFormat::parseLine line " << line << std::endl;
	if (line.length() >= 30) {
	    auto mag = line.substr(0, 5);
	    auto ra = line.substr(6, 8);
	    auto pol = line.substr(15, 8);
	    auto greek = line.substr(24, 4);
	    auto width = line.substr(28, 1);
	    auto cons = line.substr(29);
        LocaleContext localectx(LC_NUMERIC);
        auto dRa = localectx.parseDouble(LocaleContext::en_US, ra);
        auto dDec = localectx.parseDouble(LocaleContext::en_US, pol);
	    auto raDec = std::make_shared<RaDec>();
	    raDec->setRaHours(dRa);
	    raDec->setDecDegreesPolar(dDec);
	    auto constlIter = constellations.find(cons);
        std::shared_ptr<Constellation> constl;
	    if (constlIter == constellations.end()) {
            //std::cout << "ConstellationFormat::parseLine adding constl. " << cons << std::endl;
            constl = std::make_shared<Constellation>(cons);
            constellations.insert(std::pair<std::string, std::shared_ptr<Constellation>>(cons, constl));
	    }
        else {
            constl = (*constlIter).second;
        }
        int iwidht = std::stoi(width);
	    constl->addPosition(raDec, iwidht);
	}
    else {
        std::cout << "constellation unusable line " << line << std::endl;
    }
}

std::list<std::shared_ptr<Constellation>>
ConstellationFormat::getConstellations()
{
    if (m_list.empty()) {
        m_list = readConstellations();
        //std::cout << "ConstellationFormat::getConstellations loaded " << m_list.size() << std::endl;
    }
    return m_list;
}

std::list<std::shared_ptr<Constellation>>
ConstellationFormat::readConstellations()
{
	std::map<std::string, std::shared_ptr<Constellation>> constellations;
	try {
        auto resource_path = m_appl.get_resource_base_path() + "/SnT_constellation.txt";
        auto bytes = Gio::Resource::lookup_data_global(resource_path);
        gsize size;
        auto data = bytes->get_data(size);
        //std::cout << "ConstellationFormat::readConstellations bytes " << size << std::endl;
	    if (data != nullptr
         && size > 0) {
            auto memStrm = Gio::MemoryInputStream::create();
            memStrm->add_data(data, size);
            auto dataStrm = Gio::DataInputStream::create(memStrm);
		    while (true) {  // dataStrm->get_available() > 0u
                std::string line;
                dataStrm->read_line_utf8(line);
                if (line.empty()) {
                    break;
                }
                parseLine(line, constellations);
		    }
            dataStrm->close();
	    }
	    else {
            std::cout << "The constellation data " << resource_path << " was not found!" << std::endl;
	    }
	}
	catch (const Gio::Error& exc) {
	    std::cout << "Error " << exc.what() << " reading constellations!" << std::endl;
	}
    std::list<std::shared_ptr<Constellation>> list;
    for (auto pair : constellations) {
        list.push_back(pair.second);
    }
	return list;
}
