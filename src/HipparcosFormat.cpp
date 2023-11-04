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

#include <map>
#include <iostream>

#include "HipparcosFormat.hpp"
#include "Math.hpp"

HipparcosFormat::HipparcosFormat(const Gtk::Application& appl)
: m_appl{appl}
{
}

std::vector<std::shared_ptr<Star>>
HipparcosFormat::getStars()
{
    if (stars.empty()) {
        stars = readStars();
    }
    std::vector<std::shared_ptr<Star>> rstars;
    rstars.reserve(stars.size());
    for (auto s : stars) {
        rstars.push_back(s);
    }
    return rstars;	// keep internal structure
}

std::vector<std::shared_ptr<HipparcosStar>>
HipparcosFormat::readStars()
{
    std::vector<std::shared_ptr<HipparcosStar>> stars;
    JsonHelper jsonHelper;
    auto resource_path = m_appl.get_resource_base_path() + "/hipparcos.json";
    auto bytes = Gio::Resource::lookup_data_global(resource_path);
    gsize size{0};
    auto data = bytes->get_data(size);
    if (data != nullptr && size > 0u) {
        try {
            auto array = Glib::ByteArray::create();
            array->append(static_cast<const guint8*>(data), static_cast<guint>(size));
            jsonHelper.load_data(array);
            stars.reserve(1700);
            JsonObject* root = jsonHelper.get_root_object();
            JsonArray* metadatas = jsonHelper.get_array(root, "metadata");
            std::map<std::string, std::shared_ptr<Field>> fields;
            for (uint32_t m = 0; m < json_array_get_length(metadatas); ++m) {
                JsonObject* metadata = jsonHelper.get_array_object(metadatas, m);
                auto field = std::make_shared<Field>(metadata, m);
                auto name = field->getName();
                fields.insert(std::pair<std::string, std::shared_ptr<Field>>(name, field));
            }
            JsonArray* data = jsonHelper.get_array(root, "data");
            for (uint32_t i = 0; i < json_array_get_length(data); ++i) {
                JsonArray* jstar = jsonHelper.get_array_array(data, i);
                auto fvmag = fields.find("Hpmag");
                auto fra = fields.find("RArad");
                auto fdec = fields.find("DErad");
                auto fhip = fields.find("HIP");
                if (fvmag != fields.end()
                 && fra != fields.end()
                 && fdec != fields.end()
                 && fhip != fields.end()) {
                    double vmag = (*fvmag).second->getDouble(jstar);
                    double ra = (*fra).second->getDouble(jstar);
                    double dec = (*fdec).second->getDouble(jstar);
                    long hip = (*fhip).second->getLong(jstar);
                    //std::cout << "hip " << hip << " ra " << ra << " dec " << dec << " mag " << vmag << "\n";
                    auto star = std::make_shared<HipparcosStar>();
                    star->setNumber(hip);
                    star->setVmagnitude(vmag);
                    auto raDec = std::make_shared<RaDec>(Math::toRadians(ra), Math::toRadians(dec));
                    star->setRaDec(raDec);
                    stars.push_back(star);
                }
            }
        }
        catch (const JsonException& exc) {
            std::cout << "The star data was not be loaded exception " << exc.what() << "!" << std::endl;
        }
    }
    else {
        std::cout << "The star data " << resource_path << " was not found!" << std::endl;
    }
    return stars;
}
