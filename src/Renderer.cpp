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


#include <iostream>

#include "Renderer.hpp"
#include "Math.hpp"


CairoGradient::CairoGradient(Cairo::RefPtr<Cairo::Gradient> gradient)
: RenderGradient()
, m_gradient{gradient}
{
}

void
CairoGradient::addColorStop(double pos, RenderColor& rgba)
{
    m_gradient->add_color_stop_rgba(pos, rgba.getRed(), rgba.getGreen(), rgba.getBlue(), rgba.getAlpha());
}

void
CairoGradient::addColorStop(double pos, Gdk::RGBA& rgba)
{
    m_gradient->add_color_stop_rgba(pos, rgba.get_red(), rgba.get_green(), rgba.get_blue(), rgba.get_alpha());
}


Cairo::RefPtr<Cairo::Gradient>
CairoGradient::getGradient()
{
    return m_gradient;
}

CairoText::CairoText(Glib::RefPtr<Pango::Layout>& layout)
: m_layout{layout}
{

}

void
CairoText::setText(const Glib::ustring& text)
{
    m_layout->set_text(text);
}

void
CairoText::getSize(double& width, double& height)
{
    int iwidth{}, iheigh{};
    m_layout->get_pixel_size(iwidth, iheigh);
    width = static_cast<double>(iwidth);
    height = static_cast<double>(iheigh);
}

Glib::RefPtr<Pango::Layout>
CairoText::getLayout()
{
    return m_layout;
}

CairoRenderer::CairoRenderer(Cairo::RefPtr<Cairo::Context>& ctx)
: m_ctx{ctx}
{

}

std::shared_ptr<RenderGradient>
CairoRenderer::createRadialGradient(double x0, double y0, double r0, double x1, double y1, double r1)
{
    auto gradient = Cairo::RadialGradient::create(x0, y0, r0, x1, y1, r1);
    auto cairoGradient = std::make_shared<CairoGradient>(gradient);
    return cairoGradient;
}

void
CairoRenderer::setSource(std::shared_ptr<RenderGradient>& grad)
{
    auto cairoRenderer = std::dynamic_pointer_cast<CairoGradient>(grad);
    if (cairoRenderer) {
        m_ctx->set_source(cairoRenderer->getGradient());
    }
    else {
        std::cout << "CairoRenderer::setSource wrong instance given!" << std::endl;
    }
}

void
CairoRenderer::rectangle(double x0, double y0, double width, double height)
{
    m_ctx->rectangle(x0, y0, width, height);
}

void
CairoRenderer::fill()
{
    m_ctx->fill();
}

void
CairoRenderer::translate(double x0, double y0)
{
    m_ctx->translate(x0, y0);
}

void
CairoRenderer::circle(double x0, double y0, double r)
{
    m_ctx->arc(x0, y0, r, 0.0, Math::TWO_PI);
}

void
CairoRenderer::clip()
{
    m_ctx->clip();
}

void
CairoRenderer::setSource(RenderColor& rgba)
{
    m_ctx->set_source_rgba(rgba.getRed(), rgba.getGreen(), rgba.getBlue(), rgba.getAlpha());
}

void
CairoRenderer::setTrueSource(RenderColor& rgba)
{
    setSource(rgba);
}

std::shared_ptr<RenderText>
CairoRenderer::createText(Pango::FontDescription& fontDesc)
{
    auto pangoLayout = Pango::Layout::create(m_ctx);
    pangoLayout->set_font_description(fontDesc);
    return std::make_shared<CairoText>(pangoLayout);
}

void
CairoRenderer::moveTo(double x, double y)
{
    m_ctx->move_to(x, y);
}

void
CairoRenderer::lineTo(double x, double y)
{
    m_ctx->line_to(x, y);
}

void
CairoRenderer::showText(std::shared_ptr<RenderText>& text, double x, double y, TextAlign textAlign)
{
    auto cairoText = std::dynamic_pointer_cast<CairoText>(text);
    if (cairoText) {
        double width;
        double height;
        cairoText->getSize(width, height);
        switch (textAlign) {
            case TextAlign::LeftTop:
                break;      // use as is
            case TextAlign::LeftBottom:
                y -= height;
                break;
            case TextAlign::LeftMid:
                y -= height / 2.0;
                break;
            case TextAlign::RightMid:
                x -= width;
                y -= height / 2.0;
                break;
        }
        m_ctx->move_to(x, y);
        auto pangoLayout = cairoText->getLayout();
        pangoLayout->show_in_cairo_context(m_ctx);
    }
    else {
        std::cout << "CairoRenderer::showText wrong instance given!" << std::endl;
    }
}

void
CairoRenderer::save()
{
    m_ctx->save();
}

void
CairoRenderer::restore()
{
    m_ctx->restore();
}

void
CairoRenderer::beginNewPath()
{
    m_ctx->begin_new_path();
}


void
CairoRenderer::setLineWidth(double width)
{
    m_ctx->set_line_width(width);
}

void
CairoRenderer::curveTo(double x, double y, double e0, double r0, double e1, double r1)
{
    m_ctx->curve_to(x, y, e1, r0, e0, r1);
}

void
CairoRenderer::closePath()
{
    m_ctx->close_path();
}

void
CairoRenderer::stroke()
{
    m_ctx->stroke();
}

void
CairoRenderer::paint()
{
    m_ctx->paint();
}

void
CairoRenderer::dot(double x, double y, double r)
{
    circle(x, y, r);
    fill();
}

void
CairoRenderer::diffuseDot(double x, double y, double r, RenderColor& start, RenderColor& stop)
{
    save();
    translate(x-r, y-r);
    auto gradient = createRadialGradient(r, r, 0.0, r, r, r); // the queue word here is concentric
    gradient->addColorStop(0.0, start);
    gradient->addColorStop(1.0, stop);
    rectangle(0.0, 0.0, r*2.0, r*2.0);
    clip();
    setSource(gradient);
    paint();
    restore();
}

/**
 * @param radius
 * @param phase 0 right .. 1 center .. 2 left
 */
void
CairoRenderer::halfRight(double radius, double phase)
{
	double bezierCircApprox = BEZIER_APPROX * radius;
	double phaseInv = 1.0 - phase;
	double extend = radius + radius * phaseInv;
	double radius2 = radius * 2.0;
    moveTo(radius, 0.0);
	curveTo(radius + bezierCircApprox * phaseInv, 0.0,
				extend, radius - bezierCircApprox,
				extend, radius);
	curveTo(extend, radius + bezierCircApprox,
				radius + bezierCircApprox * phaseInv, radius2,
				radius, radius2);
	// add fixed part
	curveTo(radius + bezierCircApprox, radius2,
				radius2, radius + bezierCircApprox,
				radius2, radius);
	curveTo(radius2, radius - bezierCircApprox,
				radius + bezierCircApprox, 0.0,
				radius, 0.0);
	closePath();
}

/**
 * is draw into rect of size and orgin in the top left
 */
void
CairoRenderer::showPhase(Phase phase, double x, double y, double radius)
{
    save();
    translate(x-radius, y-radius);
    circle(radius, radius, radius);
    RenderColor bright(0.6, 0.6, 0.6);
    RenderColor dark(0.3, 0.3, 0.3);
	if (phase.isWanning()) {
	    setSource(bright);
	    fill();
	    setSource(dark);
	    halfRight(radius, 2.0 - (2.0 * phase.getPhase()));
	    fill();
	}
	else {
	    setSource(dark);
	    fill();
	    setSource(bright);
	    halfRight(radius, 2.0 * phase.getPhase());
	    fill();
	}
    restore();
}
