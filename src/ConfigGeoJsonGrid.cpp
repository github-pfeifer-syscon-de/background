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
#include "ConfigGeoJsonGrid.hpp"
#include "BackConfig.hpp"
#include "GeoPaint.hpp"

ConfigGeoJsonGrid::ConfigGeoJsonGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder,  BaseConfigListener* baseConfigListener)
: BaseConfigGrid(cobject, refBuilder, baseConfigListener)
{
    auto config = std::dynamic_pointer_cast<BackConfig>(m_sphereView->get_config());
    refBuilder->get_widget("geoFileButton", m_geoJsonButton);
    if (m_geoJsonButton) {
        m_geoJsonButton->set_filename(config->getGeoJsonFile());
        m_geoJsonButton->signal_file_set()
                .connect(sigc::mem_fun(*this, &ConfigGeoJsonGrid::geojsonfile_changed));
    }
    Gtk::Button* geoClearFile{nullptr};
    refBuilder->get_widget("geoClearFile", geoClearFile);
    if (geoClearFile) {
        geoClearFile->signal_clicked()
                .connect(sigc::mem_fun(*this, &ConfigGeoJsonGrid::clearGeoFile));
    }
}



void
ConfigGeoJsonGrid::geojsonfile_changed()
{
    Glib::ustring file = m_geoJsonButton->get_filename();
    auto config = std::dynamic_pointer_cast<BackConfig>(m_sphereView->get_config());
    bool success = dynamic_cast<GeoPaint*>(m_sphereView)->setGeoJsonFile(file);
    if (!success) {
        file = "";
    }
    m_geoJsonButton->set_filename(file);
    config->setGeoJsonFile(file);
}

void
ConfigGeoJsonGrid::clearGeoFile()
{
    auto config = std::dynamic_pointer_cast<BackConfig>(m_sphereView->get_config());
    config->setGeoJsonFile("");
    m_geoJsonButton->set_filename("");
    dynamic_cast<GeoPaint*>(m_sphereView)->setGeoJsonFile("");
}