/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
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

#pragma once

#include <gtkmm.h>

#include "Phase.hpp"

enum class TextAlign
{
      LeftTop
    , LeftBottom
    , LeftMid
    , RightMid
};

class RenderColor
{
public:
    RenderColor(double r, double g, double b, double a = 1.0)
    : m_r{r}
    , m_g{g}
    , m_b{b}
    , m_a{a}
    {
    }
    RenderColor(const RenderColor& other) = default;
    virtual ~RenderColor() = default;
    double getRed() const
    {
        return m_r;
    }
    double getGreen() const
    {
        return m_g;
    }
    double getBlue() const
    {
        return m_b;
    }
    double getAlpha() const
    {
        return m_a;
    }
private:
    double m_r;
    double m_g;
    double m_b;
    double m_a;
};


class RenderGradient
{
public:
    explicit RenderGradient(const RenderGradient& orig) = delete;
    virtual ~RenderGradient() = default;
    virtual void addColorStop(double pos, RenderColor& rgba) = 0;
    virtual void addColorStop(double pos, Gdk::RGBA& rgba) = 0;
protected:
    RenderGradient() = default;

};

class RenderText
{
public:
    explicit RenderText(const RenderText& orig) = delete;
    virtual ~RenderText() = default;
    virtual void setText(const Glib::ustring& text) = 0;
    virtual void getSize(double& width, double& height) = 0;
protected:
    RenderText() = default;

};


class Renderer
{
public:
    virtual ~Renderer() = default;
    virtual std::shared_ptr<RenderGradient> createRadialGradient(double x0, double y0, double r0, double x1, double y1, double r1) = 0;
    virtual void setSource(std::shared_ptr<RenderGradient>& grad) = 0;
    virtual void rectangle(double x0, double y0, double width, double height) = 0;
    virtual void fill() = 0;
    virtual void translate(double x0, double y0) = 0;
    virtual void circle(double x0, double y0, double r) = 0;
    virtual void clip() = 0;
    virtual void setSource(RenderColor& rgba) = 0;
    virtual void setTrueSource(RenderColor& rgba) = 0;
    virtual std::shared_ptr<RenderText> createText(Pango::FontDescription& fontDesc) = 0;
    virtual void showText(std::shared_ptr<RenderText>& text, double x, double y, TextAlign textAlign) = 0;
    virtual void moveTo(double x, double y) = 0;
    virtual void lineTo(double x, double y) = 0;
    virtual void save() = 0;
    virtual void restore() = 0;
    virtual void beginNewPath() = 0;
    virtual void setLineWidth(double width) = 0;
    virtual void curveTo(double x, double y, double e0, double r0, double e1, double r1) = 0;
    virtual void closePath() = 0;
    virtual void stroke() = 0;
    virtual void paint() = 0;


    virtual void dot(double x, double y, double r) = 0;
    virtual void diffuseDot(double x, double y, double r, RenderColor& start, RenderColor& stop) = 0;
    virtual void showPhase(Phase phase, double x, double y, double radius) = 0;

    // https://stackoverflow.com/questions/1734745/how-to-create-circle-with-b%c3%a9zier-curves#27863181
    // for mathematicians this is a approximation for all other people it is a circle segment
    static constexpr auto BEZIER_APPROX = (4.0 * (std::sqrt(2.0) - 1.0) / 3.0);

protected:
    Renderer() = default;
private:

};


class CairoGradient
: public RenderGradient
{
public:
    CairoGradient(Cairo::RefPtr<Cairo::Gradient> gradient);
    explicit CairoGradient(const CairoGradient& orig) = delete;
    virtual ~CairoGradient() = default;

    void addColorStop(double pos, RenderColor& rgba) override;
    void addColorStop(double pos, Gdk::RGBA& rgba) override;
    Cairo::RefPtr<Cairo::Gradient> getGradient();
private:
    Cairo::RefPtr<Cairo::Gradient> m_gradient;
};

class CairoText
: public RenderText
{
public:
    CairoText(Glib::RefPtr<Pango::Layout>& layout);
    explicit CairoText(const CairoText& orig) = delete;
    virtual ~CairoText() = default;
    void setText(const Glib::ustring& text) override;
    void getSize(double& width, double& height) override;
    Glib::RefPtr<Pango::Layout> getLayout();

private:
    Glib::RefPtr<Pango::Layout> m_layout;
};


class CairoRenderer
: public Renderer
{
public:
    CairoRenderer(Cairo::RefPtr<Cairo::Context>& ctx);
    explicit CairoRenderer(const CairoRenderer& orig) = delete;
    virtual ~CairoRenderer() = default;

    std::shared_ptr<RenderGradient> createRadialGradient(double x0, double y0, double r0, double x1, double y1, double r1) override;
    void setSource(std::shared_ptr<RenderGradient>& grad) override;
    void rectangle(double x0, double y0, double width, double height) override;
    void fill() override;
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

    void dot(double x, double y, double r) override;
    void diffuseDot(double x, double y, double r, RenderColor& start, RenderColor& stop) override;
    void showPhase(Phase phase, double x, double y, double radius) override;

protected:
    void halfRight(double radius, double phase);

private:
    Cairo::RefPtr<Cairo::Context> m_ctx;
};

