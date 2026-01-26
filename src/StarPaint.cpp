/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf
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


#include "HipparcosFormat.hpp"
#include "ConstellationFormat.hpp"
#include "Moon.hpp"
#include "Sun.hpp"
#include "Math.hpp"
#include "BackgroundApp.hpp"
#include "FileLoader.hpp"
#include "config.h"
#include "Planets.hpp"
#include "MessierLoader.hpp"
#include "Module.hpp"
#include "ClockModule.hpp"
#include "InfoModule.hpp"
#include "CalendarModule.hpp"
#include "StarWin.hpp"
#include "Renderer.hpp"

#include "StarPaint.hpp"

StarPaint::StarPaint(StarWin* starWin)
: m_starWin{starWin}
{
    m_config = m_starWin->getConfig();
    m_fileLoader = starWin->getFileLoader();
    m_starFormat = std::make_shared<HipparcosFormat>(m_fileLoader);
    m_constlFormat = std::make_shared<ConstellationFormat>(m_fileLoader);
    m_milkyway = std::make_shared<Milkyway>(m_fileLoader);
    m_messier =  std::make_shared<MessierLoader>(m_fileLoader);
    m_modules = createModules();
}


// as messier objects appear in some places close together, show names concatenated
std::vector<NamedPoint>
StarPaint::cluster(const std::vector<NamedPoint>& points, double distance)
{
    std::vector<NamedPoint> ret;
    ret.reserve(points.size());
    for (auto& p : points) {
        bool found{false};
        for (auto& r : ret) {
            auto dist = p.getPoint().dist(r.getPoint());
            if (dist < distance) {
                r.add(p);
                found = true;
                break;
            }
        }
        if (!found) {
            ret.push_back(p);
        }
    }
    return ret;
}

void
StarPaint::draw_messier(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
{
    renderer->save();
    auto starDesc = getStarFont();
    auto text = renderer->createText(starDesc);
    //double width, height;
    auto messiers = m_messier->getMessiers();
    const auto messierVMagMin = getMessierVMagMin();
    std::vector<NamedPoint> points;
    points.reserve(128);
	for (auto& messier : messiers) {
#       ifdef DEBUG
        std::cout << "StarPaint::draw_messier " << messier->getIdent() << std::endl;
#       endif
        if (messier->getVmagnitude() < messierVMagMin) {
            auto raDec = messier->getRaDec();
            auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
            if (azAlt->isVisible()) {
                auto p = azAlt->toScreen(layout);
                MagPoint magP(p, messier->getVmagnitude());
                points.emplace_back(std::move(NamedPoint(magP, messier->getName())));
            }
        }
	}
    const auto messierRadius{layout.getMin() / MESSIER_FACTOR};
    const auto clusterRadius{layout.getMin() / CLUSTER_FACTOR};
#   ifdef DEBUG
    std::cout << "messierRadius " << messierRadius << " fix " << MESSIER_RADIUS << std::endl;
    std::cout << "StarPaint::draw_messier"
              << " height " <<  layout.getHeight()
              << " dist " <<  clusterRadius << std::endl;
#   endif
    auto clusters = cluster(points, clusterRadius);
#   ifdef DEBUG
    std::cout << "StarPaint::draw_messier clusters " << clusters.size() << std::endl;
#   endif
    for (auto& cluster : clusters) {
        double xMax(-layout.getWidth()),yMax(-layout.getHeight());
        for (auto& point : cluster.getMagPoints()) {
            auto brightness = Math::mix(TEXT_GRAY_EMPHASIS, TEXT_GRAY_LOW, (point.getVmagnitude() - 4.0) / 3.0);
            RenderColor start(brightness, brightness, brightness, 1.0);
            RenderColor stop(brightness, brightness, brightness, 0.0);
            renderer->diffuseDot(point.getX(), point.getY(), messierRadius, start, stop);
            xMax = std::max(xMax, point.getX() + messierRadius);
            yMax = std::max(yMax, point.getY());
        }
        RenderColor textColor(TEXT_GRAY_MID, TEXT_GRAY_MID, TEXT_GRAY_MID);
        renderer->setSource(textColor);
        text->setText(cluster.getName());
        renderer->showText(text, xMax, yMax, TextAlign::LeftTop);
    }
    renderer->restore();
}

void
StarPaint::draw_planets(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
{
    auto starDesc = getStarFont();
    auto text = renderer->createText(starDesc);
    Planets planets;
    const auto planetRadius{layout.getMin() / PLANET_FACTOR};
    RenderColor grayEmph(TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS);
    RenderColor grayText(TEXT_GRAY_MID, TEXT_GRAY_MID, TEXT_GRAY_MID);
	for (auto& planet : planets.getOtherPlanets()) {
#       ifdef DEBUG
        std::cout << "StarPaint::draw_planets " << planet->getName() << std::endl;
#       endif
	    auto raDec = planet->getRaDecPositon(jd);
	    auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
	    if (azAlt->isVisible()) {
            auto p = azAlt->toScreen(layout);
            renderer->setSource(grayEmph);
            renderer->dot(p.getX(), p.getY(), planetRadius);
            renderer->setSource(grayText);
            text->setText(Glib::ustring::sprintf("%s %.1fAU", planet->getName(), raDec->getDistanceAU()));
            renderer->showText(text, p.getX() + planetRadius, p.getY(), TextAlign::LeftTop);
	    }
	}
}

void
StarPaint::draw_sun(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
{
    auto raDec = Sun::position(jd);
    //std::cout << "Sun ra " << raDec->getRaDegrees() << " dec " << raDec->getDecDegrees() << std::endl;
    auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
    //std::cout << "Sun az " << azAlt->getAzimutDegrees() << " az " << azAlt->getAltitudeDegrees() << std::endl;
    if (azAlt->isVisible()) {
        auto p = azAlt->toScreen(layout);
        RenderColor sunColor(TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS, TEXT_GRAY_LOW);
        renderer->setTrueSource(sunColor);
        renderer->dot(p.getX(), p.getY(), getSunMoonRadius(layout));
    }
}

void
StarPaint::draw_moon(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
{
    auto raDec = Moon::position(jd);
    auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
    if (azAlt->isVisible()) {
        Moon moon;
        auto p = azAlt->toScreen(layout);
        renderer->showPhase(moon.getPhase(jd), p.getX(), p.getY(), getSunMoonRadius(layout));
    }
}


void
StarPaint::draw_milkyway(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
{
    double lineWidth = getLineWidth(layout);
    renderer->setLineWidth(lineWidth);
    for (auto poly : m_milkyway->getBounds()) {
        //renderer->beginNewPath(); this is important if we decide to only partly draw the shapes
        std::list<std::shared_ptr<AzimutAltitude>> azAlts;  // saving intermediate saves us recalculation
        bool anyVisible = false;
        for (auto raDec : poly->getPoints()) {
            auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
            azAlts.push_back(azAlt);
            anyVisible |= azAlt->isVisible();
        }
        uint32_t skipped{};
        if (anyVisible) {       // do not draw if outside
            int intens = poly->getIntensity();
            double dintens = 0.1 + (double)intens / 20.0;
            RenderColor milkyColor(dintens, dintens, 0.25 + dintens);
            renderer->setTrueSource(milkyColor);
            bool move = true;
            //std::cout << "Starting poly" << std::endl;
            for (auto azAlt : azAlts) {
                //if (azAlt->isVisible()) { // stop drawing beyond horizont, with some additional to allow closing
                    auto p = azAlt->toScreen(layout);
                    if (move) {
                        //std::cout << "  move " << p.getX() << " y " << p.getY() << std::endl;
                        renderer->moveTo(p.getX(), p.getY());
                        move = false;
                    }
                    else {
                        //std::cout << "  line " << p.getX() << " y " << p.getY() << std::endl;
                        renderer->lineTo(p.getX(), p.getY());
                    }
                //}
                //else {
                //    move = true;
                //    ++skipped;
                //}
            }
        }
        else {
            skipped = static_cast<uint32_t>(azAlts.size());
        }
        //if (skipped > 0) {
        //    std::cout << __FILE__ << "::draw_milkyway"
        //              << " intens " << intens
        //              << " skipped " << skipped << "/" << azAlts.size() << std::endl;
        //}
        // the given data wraps nicely onto a sphere,
        //   but here we have a disc view (world),
        //    so we stick to some abstraction
        //ctx->close_path();
        //ctx->fill();
        renderer->stroke();
    }
    auto raDec = m_milkyway->getGalacticCenter();
    auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
    if (azAlt->isVisible()) {
        RenderColor centColor(TEXT_GRAY, TEXT_GRAY, TEXT_GRAY);
        renderer->setSource(centColor);
        auto p = azAlt->toScreen(layout);
        auto w = static_cast<double>(layout.getMin()) / (SUNMOON_FACTOR / 2.0);
        renderer->setLineWidth(getLineWidth(layout));
        renderer->moveTo(p.getX()-w,p.getY());
        renderer->lineTo(p.getX()+w,p.getY());
        renderer->moveTo(p.getX(),p.getY()-w);
        renderer->lineTo(p.getX(),p.getY()+w);
        renderer->stroke();
        auto starDesc = getStarFont();
        auto text = renderer->createText(starDesc);
        text->setText("Gal.cent.");
        renderer->showText(text, p.getX()+w, p.getY(), TextAlign::LeftTop);
    }
}

void
StarPaint::draw_stars(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
{
    RenderColor starColor(TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS);
    renderer->setSource(starColor);
    auto minStarRadius = static_cast<double>(layout.getMin()) / MIN_STAR_FACTOR;
    auto maxStarRadius = static_cast<double>(layout.getMin()) / MAX_STAR_FACTOR;
    for (auto s : m_starFormat->getStars()) {
        auto raDec = s->getRaDec();
        auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
        if (azAlt->isVisible()) {
            auto p = azAlt->toScreen(layout);
            auto rs = Math::mix(maxStarRadius, minStarRadius, ((s->getVmagnitude() - 3.0) / 2.0));
            //std::cout << "x " << p.getX() << " y " << p.getY() << " rs " << rs << "\n";
            renderer->dot(p.getX(), p.getY(), rs);
        }
    }
}

double
StarPaint::getLineWidth(const Layout& layout)
{
    auto lineWidth = static_cast<double>(layout.getMin()) / LINE_WIDTH_FACTOR;
#   ifdef DEBUG
    std::cout << "LineWidth " << lineWidth << std::endl;
#   endif
    return lineWidth;
}

double
StarPaint::getSunMoonRadius(const Layout& layout)
{
    auto sunRadius = static_cast<double>(layout.getMin()) / SUNMOON_FACTOR;
#   ifdef DEBUG
    std::cout << "SunRadius " << sunRadius << " fix " << SUN_MOON_RADIUS << std::endl;
#   endif
    return sunRadius;
}

void
StarPaint::draw_constl(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
{
    auto starDesc = getStarFont();
    auto text = renderer->createText(starDesc);
    //double width, height;
    //text->setText("M");
    //text->getSize(width, height);
    auto lineWidth = getLineWidth(layout);
    for (auto c : m_constlFormat->getConstellations()) {
        Point2D sum;
        bool anyVisible = false;
        auto polylines = c->getPolylines();
#       ifdef DEBUG
        std::cout << "Constl " << c->getName() << std::endl;
#       endif
        uint32_t count{};
        for (auto l : polylines) {
            int prio = l->getWidth();
            auto gray = Math::mix(TEXT_GRAY_EMPHASIS, TEXT_GRAY_LOW, (prio - 1) / 3.0);
            RenderColor grayColor(gray, gray, gray);
            renderer->setSource(grayColor);
            renderer->setLineWidth((prio <= 1) ? lineWidth * 1.5 : lineWidth);
            bool visible = false;
            for (auto raDec : l->getPoints()) {
                auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
                if (azAlt->isVisible()) {
                    visible = true;
                }
            }
            if (visible) {
                anyVisible = true;
                bool first{true};
                for (auto raDec : l->getPoints()) {
                    auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
                    auto p = azAlt->toScreen(layout);
                    sum.add(p);
                    ++count;
                    if (first) {
                        renderer->moveTo(p.getX(), p.getY());
                        first = false;
                    }
                    else {
                        renderer->lineTo(p.getX(), p.getY());
                    }
                }
                renderer->stroke();
            }
        }
        if (anyVisible) {
            RenderColor gray(TEXT_GRAY, TEXT_GRAY, TEXT_GRAY);
            renderer->setSource(gray);
            double avgX = sum.getX() / (double)count;
            double avgY = sum.getY() / (double)count;
            text->setText(c->getName());
            renderer->showText(text, avgX, avgY, TextAlign::LeftTop);
        }
    }
}

void
StarPaint::drawSky(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
{
    renderer->save();
    const double r = layout.getMin() / 2.0;
    auto grad = renderer->createRadialGradient((layout.getWidth()/2), (layout.getHeight()/2), r / 3.0, (layout.getWidth()/2), (layout.getHeight()/2), r);
    auto startColor = getStartColor();
    auto stopColor = getStopColor();
    grad->addColorStop(0.0, startColor);
    grad->addColorStop(1.0, stopColor);
    renderer->setSource(grad);
    renderer->rectangle(layout.getXOffs(), layout.getYOffs(), layout.getWidth(), layout.getHeight());
    renderer->fill();
    renderer->translate((layout.getXOffs() + layout.getWidth()/2)
                      , (layout.getYOffs() + layout.getHeight()/2));
    renderer->circle(0.0, 0.0, r);
    renderer->clip();    // as we draw some lines beyond the horizon
    if (isShowMilkyway()) {
        draw_milkyway(renderer, jd, geoPos, layout);
    }
    draw_constl(renderer, jd, geoPos, layout);
    draw_stars(renderer, jd, geoPos, layout);
    draw_moon(renderer, jd, geoPos, layout);
    draw_sun(renderer, jd, geoPos, layout);
    draw_planets(renderer, jd, geoPos, layout);
    draw_messier(renderer, jd, geoPos, layout);

    RenderColor gray(TEXT_GRAY, TEXT_GRAY, TEXT_GRAY);
    renderer->setSource(gray);
    auto starFont = getStarFont();
    scale(starFont, 1.75);
    auto text = renderer->createText(starFont);
    //double width, height;
    //text->getSize(width, height);
    auto r1 = r - 1.0;   // avoid placing out of clipping
    text->setText("S");
    renderer->showText(text, 0.0, r1, TextAlign::LeftBottom);
    text->setText("E");
    renderer->showText(text, -r1, 0.0, TextAlign::LeftMid);
    text->setText("N");
	renderer->showText(text, 0.0, -r1, TextAlign::LeftTop);
    text->setText("W");
    renderer->showText(text, r1, 0.0, TextAlign::RightMid);
    renderer->restore();
}



std::vector<PtrModule>
StarPaint::findModules(const char* pos)
{
    std::vector<PtrModule> mods;
    mods.reserve(m_modules.size());
    for (auto& mod : m_modules) {
        if (mod->getPosition() == pos) {
            mods.push_back(mod);
        }
    }
    return mods;
}

void
StarPaint::drawTop(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules)
{
    Point2D pos(40.0, 20.0);
    for (auto& mod : modules) {
        ctx->save();
        ctx->translate(pos.getX(), pos.getY());
        mod->display(ctx, m_starWin);
        ctx->restore();
        Point2D p(0.0, mod->getHeight(ctx, m_starWin));
        pos.add(p);
    }
}

void
StarPaint::drawMiddle(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules)
{
    int sumHeight{};
    for (auto& mod : modules) {
        sumHeight += mod->getHeight(ctx, m_starWin);
    }
    Point2D pos(40.0, (layout.getHeight() - sumHeight) / 2.0);
    for (auto& mod : modules) {
        ctx->save();
        ctx->translate(pos.getX(), pos.getY());
        mod->display(ctx, m_starWin);
        ctx->restore();
        Point2D p(0.0, mod->getHeight(ctx, m_starWin));
        pos.add(p);
    }
}

void
StarPaint::drawBottom(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules)
{
    int sumHeight{0};
    for (auto& mod : modules) {
        sumHeight += mod->getHeight(ctx, m_starWin);
    }
    Point2D pos(40.0, layout.getHeight() - sumHeight - 20.0);
    for (auto& mod : modules) {
        ctx->save();
        ctx->translate(pos.getX(), pos.getY());
        mod->display(ctx, m_starWin);
        ctx->restore();
        Point2D p(0.0, mod->getHeight(ctx, m_starWin));
        pos.add(p);
    }
}

void
StarPaint::scale(Pango::FontDescription& starFont, double scale)
{
    starFont.set_size(static_cast<int>(starFont.get_size() * scale));
}

void
StarPaint::brighten(Gdk::RGBA& color, double factor)
{
    color.set_red(color.get_red() * factor);
    color.set_green(color.get_green() * factor);
    color.set_blue(color.get_blue() * factor);
}

std::shared_ptr<KeyConfig>
StarPaint::getConfig()
{
    return m_config;
}

bool
StarPaint::isShowMilkyway()
{
    return m_config->getBoolean(MAIN_GRP, SHOW_MILKYWAY_KEY, true);
}

void
StarPaint::setShowMilkyway(bool showMilkyway)
{
    m_config->setBoolean(MAIN_GRP, SHOW_MILKYWAY_KEY, showMilkyway);
}

double
StarPaint::getMessierVMagMin()
{
    return m_config->getDouble(MAIN_GRP, MESSIER_VMAGMIN_KEY, 5.0);
}

void
StarPaint::setMessierVMagMin(double messierVmagMin)
{
    m_config->setDouble(MAIN_GRP, MESSIER_VMAGMIN_KEY, messierVmagMin);
}

Pango::FontDescription
StarPaint::getStarFont()
{
    return m_config->getFont(MAIN_GRP, STAR_FONT_KEY, DEFAULT_STAR_FONT);
}

void
StarPaint::setStarFont(const Pango::FontDescription& descr)
{
    m_config->setFont(MAIN_GRP, STAR_FONT_KEY, descr);
}

Gdk::RGBA
StarPaint::getStartColor()
{
    Gdk::RGBA dfltStart{"rgb(6%,6%,15%)"};
    return m_config->getColor(MAIN_GRP, START_COLOR_KEY, dfltStart);
}

void
StarPaint::setStartColor(const Gdk::RGBA& startColor)
{
    m_config->setColor(MAIN_GRP, START_COLOR_KEY, startColor);
}


Gdk::RGBA
StarPaint::getStopColor()
{
    Gdk::RGBA dfltStop{"rgb(10%,10%,20%)"};
    return m_config->getColor(MAIN_GRP, STOP_COLOR_KEY, dfltStop);
}

void
StarPaint::setStopColor(const Gdk::RGBA& stopColor)
{
    m_config->setColor(MAIN_GRP, STOP_COLOR_KEY, stopColor);
}

std::vector<PtrModule>
StarPaint::createModules()
{
#   ifdef USE_PYTHON
    auto pyWrapper = std::make_shared<PyWrapper>();
#   else
    auto pyWrapper = std::shared_ptr<PyWrapper>();
#   endif
    std::vector<PtrModule> mods;
    mods.reserve(4);
    mods.emplace_back(
        std::move(
            std::make_shared<InfoModule>(m_config, pyWrapper)));
    mods.emplace_back(
        std::move(
            std::make_shared<ClockModule>(m_config, pyWrapper)));
    mods.emplace_back(
        std::move(
            std::make_shared<CalendarModule>(m_config, pyWrapper)));
    return mods;
}

std::vector<PtrModule>
StarPaint::getModules()
{
    return m_modules;
}

void
StarPaint::drawImage(Cairo::RefPtr<Cairo::Context>& ctx
            , const Glib::DateTime& now
            , GeoPosition& pos
            , Layout& layout)
{
    JulianDate jd(now);
    //std::cout << std::fixed << "jd " << jd.getJulianDate() << std::endl;
    ctx->save();
    CairoRenderer cairoRenderer(ctx);
    drawSky(&cairoRenderer, jd, pos, layout);
    ctx->restore();

    drawTop(ctx, layout, findModules(Module::POS_TOP));
    drawMiddle(ctx, layout, findModules(Module::POS_MIDDLE));
    drawBottom(ctx, layout, findModules(Module::POS_BOTTOM));
}