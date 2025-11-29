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

#include <iostream>
#include <StringUtils.hpp>

#include "HaruRenderer.hpp"
#include "PdfExport.hpp"
#include "PdfFont.hpp"
#include "Math.hpp"


HaruGradient::HaruGradient(const std::shared_ptr<psc::pdf::PdfPage>& page)
: RenderGradient()
, m_page{page}
{

}
void
HaruGradient::addColorStop(double pos, RenderColor& rgba)
{

}

void
HaruGradient::addColorStop(double pos, Gdk::RGBA& rgba)
{

}

HaruText::HaruText(const std::shared_ptr<psc::pdf::PdfPage>& page
                , const std::shared_ptr<psc::pdf::PdfFont>& font
                , float fontSize)
: RenderText()
, m_page{page}
, m_font{font}
, m_fontSize{fontSize}
{

}

void
HaruText::setText(const Glib::ustring& text)
{
    m_text = text;
}

Glib::ustring
HaruText::getText()
{
    return m_text;
}

float
HaruText::getFontSize()
{
    return m_fontSize;
}


void
HaruText::getSize(double& width, double& height)
{
    m_font->setSize(getFontSize());
    m_page->setFont(m_font);
    width = m_page->getTextWidth(m_text);
    height = m_font->getAscent();
}

HaruRenderer::HaruRenderer()
: Renderer()
, m_pdfExport{std::make_shared<psc::pdf::PdfExport>()}
{
    m_font = m_pdfExport->createFontTTFMatch("sans-serif", ENCODING, false);
    m_page = m_pdfExport->createPage();
#   ifdef DEBUG
    std::cout << "HaruRenderer::HaruRenderer" << std::endl;
#   endif

}

static float
toFloat(double d)
{
    return static_cast<float>(d);
}

float
HaruRenderer::toX(double x)
{
    return static_cast<float>(x);
}

float
HaruRenderer::toY(double y)
{
    return static_cast<float>(m_invertY ? -y : y);
}


std::shared_ptr<RenderGradient>
HaruRenderer::createRadialGradient(double x0, double y0, double r0, double x1, double y1, double r1)
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::createRadialGradient" << std::endl;
#   endif
    return std::make_shared<HaruGradient>(m_page);
}

void
HaruRenderer::setSource(std::shared_ptr<RenderGradient>& grad)
{
    // use to fill primary rect, so not used here
}

void
HaruRenderer::rectangle(double x0, double y0, double width, double height)
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::rectangle" << std::endl;
#   endif
    // use to fill primary rect, so not used here
}

void
HaruRenderer::fill()
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::fill" << std::endl;
#   endif
    //m_page->fill();   // use to fill primary rect, so not used here
}

void
HaruRenderer::translate(double x0, double y0)
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::translate x " << x0 << " y " << y0  << std::endl;
#   endif
    m_page->translate(toFloat(x0), toFloat(y0));
}

void
HaruRenderer::circle(double x0, double y0, double r)
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::arc x " << x0 << " y " << y0
              << " r " << r
              << std::endl;
#   endif
    m_page->circle(toX(x0), toY(y0), toFloat(r));
}

void
HaruRenderer::clip()
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::clip" << std::endl;
#   endif
    m_page->clip();
}

void
HaruRenderer::setSource(RenderColor& rgba)
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::setSource" << std::endl;
#   endif
    // invert as we draw on white here
    m_page->setRgb(toFloat(1.0-rgba.getRed()), toFloat(1.0-rgba.getGreen()), toFloat(1.0-rgba.getBlue()));
}

void
HaruRenderer::setTrueSource(RenderColor& rgba)
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::setTrueSource" << std::endl;
#   endif
    // use as given sun, moon...
    m_page->setRgb(toFloat(rgba.getRed()), toFloat(rgba.getGreen()), toFloat(rgba.getBlue()));
}

std::shared_ptr<RenderText>
HaruRenderer::createText(Pango::FontDescription& fontDesc)
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::createText" << std::endl;
#   endif
    Layout layout = getLayout();
    // since we use the font configured for the screen, and here we have a smaller canvas, adjust the font, corrections seems necessary
    double factor = (static_cast<float>(layout.getMin()) / m_reference) * 1.5;
    double realSize = static_cast<float>(fontDesc.get_size()) / 1024.0;
    auto fontSize = realSize * factor;
    //std::cout << "Font realSize " << realSize
    //          << " factor " << factor
    //          << " fontSize " << fontSize << std::endl;
    return std::make_shared<HaruText>(m_page, m_font, fontSize);
}

void
HaruRenderer::moveTo(double x, double y)
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::moveTo" << std::endl;
#   endif
    m_page->moveTo(toX(x), toY(y));
}

void
HaruRenderer::lineTo(double x, double y)
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::lineTo" << std::endl;
#   endif
    m_page->lineTo(toX(x), toY(y));
    ++m_points;
}

void
HaruRenderer::showText(std::shared_ptr<RenderText>& text, double x, double y, TextAlign textAlign)
{
    auto haruText = std::dynamic_pointer_cast<HaruText>(text);
    if (haruText) {
#       ifdef DEBUG
        std::cout << "HaruRenderer::showText " << haruText->getText()
                  << " x " << x << " y " << y << " align " << static_cast<int>(textAlign)
                  << std::endl;
#       endif
        double width;
        double height;
        haruText->getSize(width, height);
        switch (textAlign) {
            case TextAlign::LeftTop:
                y += height;    // we want to align the top, so shift
                break;
            case TextAlign::LeftBottom:
                //y -= height;    // here we still in positive coordinates to make this visible, shift to center
                break;
            case TextAlign::LeftMid:
                y -= height / 2.0;
                break;
            case TextAlign::RightMid:
                x -= width;
                y -= height / 2.0;
                break;
        }
        // use as extra offset as we use externally the top, and here we reference the baseline
        m_page->setFont(m_font);
        m_page->drawText(haruText->getText(), toX(x), toY(y));
    }
    else {
        std::cout << "HaruRenderer::showText wrong instance given!" << std::endl;
    }
}

void
HaruRenderer::save()
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::save" << std::endl;
#   endif
    m_page->save();
}

void
HaruRenderer::restore()
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::restore" << std::endl;
#   endif
    m_page->restore();
}

void
HaruRenderer::beginNewPath()
{
    std::cout << "HaruRenderer::beginNewPath" << std::endl;

}

void
HaruRenderer::setLineWidth(double width)
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::setLineWidth " << width << std::endl;
#   endif
    m_page->setLineWidth(toFloat(width * 0.75)); // as lines look stronger in black
}

void
HaruRenderer::curveTo(double x, double y, double e0, double r0, double e1, double r1)
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::curveTo" << std::endl;
#   endif
    m_page->curveTo(toX(x), toY(y), toX(e0), toY(r0), toX(e1), toY(r1));
    ++m_points;
}

void
HaruRenderer::closePath()
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::closePath" << std::endl;
#   endif
    m_page->closePath();
}

void
HaruRenderer::stroke()
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::stroke pnts " << m_points << std::endl;
#   endif
    if (m_points > 0) {
        m_page->stroke();
        m_points = 0;
    }
}

void
HaruRenderer::paint()
{
    std::cout << "HaruRenderer::paint not implemented" << std::endl;
}

void
HaruRenderer::setInvertY(bool invertY)
{
    m_invertY = invertY;
}

void
HaruRenderer::dot(double x, double y, double r)
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::dot" << std::endl;
#   endif
    m_page->circle(toX(x), toY(y), toFloat(r * 0.75));
    m_page->fill();
}

void
HaruRenderer::diffuseDot(double x, double y, double r, RenderColor& start, RenderColor& stop)
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::diffuseDot" << std::endl;
#   endif
    // since this is intended to work with alpha that we don't use
    RenderColor diffused(start.getRed() * 0.5, start.getGreen() * 0.5, start.getBlue() * 0.5);
    setSource(diffused);
    m_page->circle(toX(x), toY(y), toFloat(r/2.0)); // since we don't fade out reduce size
    m_page->fill();
}

void
HaruRenderer::halfRight(double radius, double phase)
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
//	// add fixed part
	curveTo(radius + bezierCircApprox, radius2,
				radius2, radius + bezierCircApprox,
				radius2, radius);
	curveTo(radius2, radius - bezierCircApprox,
				radius + bezierCircApprox, 0.0,
				radius, 0.0);
//	closePath(); this is counterproductive for this case
}

void
HaruRenderer::showPhase(Phase phase, double x, double y, double radius)
{
    if (false) {
        auto color = RenderColor(0.5, 0.5, 0.5);
        setSource(color);
        dot(x, y, radius);
    }
    else {
        save();
        m_page->translate(toX(x-radius), toY(y-radius));
        RenderColor bright(0.6, 0.6, 0.6);
        RenderColor dark(0.3, 0.3, 0.3);
        if (phase.isWanning()) {
            setTrueSource(bright);
            circle(radius, radius, radius);
            m_page->fill();
            setTrueSource(dark);
            halfRight(radius, 2.0 - (2.0 * phase.getPhase()));
            m_page->fill();
        }
        else {
            setTrueSource(dark);
            circle(radius, radius, radius);
            m_page->fill();
            setTrueSource(bright);
            halfRight(radius, 2.0 * phase.getPhase());
            m_page->fill();
        }
        restore();
    }
}


void
HaruRenderer::save(const Glib::ustring& file)
{
#   ifdef DEBUG
    std::cout << "HaruRenderer::save" << std::endl;
#   endif
    m_pdfExport->save(file);
}

Layout
HaruRenderer::getLayout()
{
    int width = static_cast<int>(m_page->getWidth() * 0.8f);
    int height = static_cast<int>(m_page->getHeight() * 0.8f);
    Layout layout(width, height);
    layout.setXOffs(static_cast<int>(m_page->getWidth() * 0.1f));
    layout.setYOffs(static_cast<int>(m_page->getHeight() * 0.1f));
    return layout;
}

void
HaruRenderer::setReference(double ref)
{
    m_reference = ref;
}
