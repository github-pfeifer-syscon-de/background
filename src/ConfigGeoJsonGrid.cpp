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
, m_config{std::dynamic_pointer_cast<BackConfig>(m_sphereView->get_config())}
{
    refBuilder->get_widget("geoFileButton", m_geoJsonButton);
    if (m_geoJsonButton != nullptr) {
        m_geoJsonButton->set_filename(m_config->getGeoJsonFile());
        m_geoJsonButton->signal_file_set()
                .connect(sigc::mem_fun(*this, &ConfigGeoJsonGrid::geojsonfile_changed));
    }
    Gtk::Button* geoClearFile{};
    refBuilder->get_widget("geoClearFile", geoClearFile);
    if (geoClearFile != nullptr) {
        geoClearFile->signal_clicked()
                .connect(sigc::mem_fun(*this, &ConfigGeoJsonGrid::clearGeoFile));
    }
    refBuilder->get_widget("imageFileButton", m_imageButton);
    if (m_imageButton != nullptr) {
        m_imageButton->set_filename(m_config->getImageFile());
        m_imageButton->signal_file_set()
                .connect(sigc::mem_fun(*this, &ConfigGeoJsonGrid::imagefile_changed));
    }
    Gtk::Button* imageClearFile{};
    refBuilder->get_widget("imageClearFile",  imageClearFile);
    if (imageClearFile != nullptr) {
        imageClearFile->signal_clicked()
                .connect(sigc::mem_fun(*this, &ConfigGeoJsonGrid::clearImageFile));
    }
    refBuilder->get_widget("geoMargin", m_spinGeoMargin);
    if (m_spinGeoMargin != nullptr) {
        m_spinGeoMargin->signal_value_changed()
                .connect(sigc::mem_fun(*this, &ConfigGeoJsonGrid::borderChanged));
    }
    m_spinGeoMargin->set_value(m_config->getGeoMargin());
    refBuilder->get_widget("spinDayStart", m_spinDayStart);
    m_spinDayStart->set_value(m_config->getDayStart());
    m_spinDayStart->signal_changed().connect( [&] {
       m_config->setDayStart(m_spinDayStart->get_value_as_int());
    });
    refBuilder->get_widget("spinDayEnd", m_spinDayEnd);
    m_spinDayEnd->set_value(m_config->getDayEnd());
    m_spinDayEnd->signal_changed().connect( [&] {
       m_config->setDayEnd(m_spinDayEnd->get_value_as_int());
    });
}

void
ConfigGeoJsonGrid::geojsonfile_changed()
{
    Glib::ustring file = m_geoJsonButton->get_filename();
    auto config = std::dynamic_pointer_cast<BackConfig>(m_sphereView->get_config());
    auto geoPaint = dynamic_cast<GeoPaint*>(m_sphereView);
    bool success = geoPaint->setGeoJsonFile(file);
    if (!success) {
        file = "";
    }
    geoPaint->refresh();
    m_geoJsonButton->set_filename(file);
    config->setGeoJsonFile(file);
}

void
ConfigGeoJsonGrid::clearGeoFile()
{
    auto config = std::dynamic_pointer_cast<BackConfig>(m_sphereView->get_config());
    config->setGeoJsonFile("");
    m_geoJsonButton->set_filename("");
    auto geoPaint = dynamic_cast<GeoPaint*>(m_sphereView);
    geoPaint->setGeoJsonFile("");
    geoPaint->refresh();
}

void
ConfigGeoJsonGrid::imagefile_changed()
{
    Glib::ustring file = m_imageButton->get_filename();
    auto config = std::dynamic_pointer_cast<BackConfig>(m_sphereView->get_config());
    auto geoPaint = dynamic_cast<GeoPaint*>(m_sphereView);
    bool success = geoPaint->setImage(file);
    if (!success) {
        file = "";
    }
    geoPaint->refresh();
    m_imageButton->set_filename(file);
    config->setImageFile(file);
}

void
ConfigGeoJsonGrid::clearImageFile()
{
    auto config = std::dynamic_pointer_cast<BackConfig>(m_sphereView->get_config());
    config->setImageFile("");
    m_imageButton->set_filename("");
    auto geoPaint = dynamic_cast<GeoPaint*>(m_sphereView);
    geoPaint->setImage("");
    geoPaint->refresh();
}

void
ConfigGeoJsonGrid::borderChanged()
{
    double margin = m_spinGeoMargin->get_value();
    auto config = std::dynamic_pointer_cast<BackConfig>(m_sphereView->get_config());
    auto geoPaint = dynamic_cast<GeoPaint*>(m_sphereView);
    geoPaint->setGeoMargin(margin);
    geoPaint->refresh();
    config->setGeoMargin(margin);
}