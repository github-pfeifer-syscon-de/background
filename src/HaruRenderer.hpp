/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
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

#pragma once

#include <glibmm.h>
#include "Renderer.hpp"

#include "PdfPage.hpp"
#include "Layout.hpp"
#include "Phase.hpp"

class HaruGradient
: public RenderGradient
{
public:
    HaruGradient(const std::shared_ptr<psc::pdf::PdfPage>& page);
    explicit HaruGradient(const HaruGradient& orig) = delete;
    virtual ~HaruGradient() = default;
    void addColorStop(double pos, RenderColor& rgba) override;
    void addColorStop(double pos, Gdk::RGBA& rgba) override;
private:
    std::shared_ptr<psc::pdf::PdfPage> m_page;

};

class HaruText
: public RenderText
{
public:
    HaruText(const std::shared_ptr<psc::pdf::PdfPage>& page
            , const std::shared_ptr<psc::pdf::PdfFont>& font
            , float fontSize);
    explicit HaruText(const RenderText& orig) = delete;
    virtual ~HaruText() = default;
    void setText(const Glib::ustring& text) override;
    void getSize(double& width, double& height) override;
    Glib::ustring getText();
    float getFontSize();
    double getHeight();

private:
    std::shared_ptr<psc::pdf::PdfPage> m_page;
    std::shared_ptr<psc::pdf::PdfFont> m_font;
    Glib::ustring m_text;
    float m_fontSize;
};

class HaruRenderer
: public Renderer
{
public:
    HaruRenderer();
    explicit HaruRenderer(const HaruRenderer& orig) = delete;
    virtual ~HaruRenderer() = default;

    std::shared_ptr<RenderGradient> createRadialGradient(double x0, double y0, double r0, double x1, double y1, double r1) override;
    void setSource(std::shared_ptr<RenderGradient>& grad) override;
    void rectangle(double x0, double y0, double width, double height) override;
    void fill() override;
    float toX(double x);
    float toY(double x);
    void translate(double x0, double y0) override;
    void circle(double x0, double y0, double r) override;
    void clip() override;
    void setSource(RenderColor& rgba) override;
    void setTrueSource(RenderColor& rgba) override;
    std::shared_ptr<RenderText> createText(Pango::FontDescription& fontDesc) override;
    void moveTo(double x, double y) override;
    void lineTo(double x, double y) override;
    void showText(std::shared_ptr<RenderText>& text, double x, double y, TextAlign textAlign) override;
    void save() override;
    void restore() override;
    void beginNewPath() override;
    void setLineWidth(double width) override;
    void curveTo(double x, double y, double e0, double r0, double e1, double r1) override;
    void closePath() override;
    void stroke() override;
    void paint() override;
    void setInvertY(bool convert);

    void dot(double x, double y, double r) override;
    void diffuseDot(double x, double y, double r, RenderColor& start, RenderColor& stop) override;
    void showPhase(Phase phase, double x, double y, double radius) override;

    void save(const Glib::ustring& file);
    Layout getLayout();
    void setReference(double ref);

    static constexpr auto ENCODING{"UTF-8"};
protected:
    void halfRight(double radius, double phase);

private:
    std::shared_ptr<psc::pdf::PdfExport> m_pdfExport;
    std::shared_ptr<psc::pdf::PdfFont> m_font;
    std::shared_ptr<psc::pdf::PdfPage> m_page;
    uint32_t m_points{};
    bool m_invertY{false};
    double m_reference{1024.0};
};

