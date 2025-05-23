# created RPf 17.4.2025
#
import cairo
import math
import datetime
import locale
import gi
gi.require_version('Pango', '1.0')
gi.require_version('PangoCairo', '1.0')
from gi.repository import Pango, PangoCairo

class Clock:
    def __init__(self):
        return

    def drawRadial(self,ctx,value,full,emphasis,radius):
        angleRad = math.radians(360) * value / full
        xv = math.sin(angleRad)
        yv = -math.cos(angleRad)
        if (emphasis):
           ctx.set_line_width(2)
           inner = radius * 0.9
        else:
           ctx.set_line_width(1)
           inner = radius * 0.95
        ctx.move_to(inner * xv, inner * yv)
        ctx.line_to(radius * xv, radius * yv)
        ctx.stroke()

    # alternative style for marks
    def drawDot(self,ctx,value,full,emphasis,radius):
        angleRad = math.radians(360) * value / full
        xv = math.sin(angleRad)
        yv = -math.cos(angleRad)
        ctx.arc(xv * radius * 0.9, yv * radius * 0.9, 3.0 if emphasis else 1.5, 0.0, math.radians(360))
        ctx.fill();


    def drawHand(self,ctx,value,full,radius,width):
        angleRad = math.radians(360) * value / full
        xv = math.sin(angleRad)
        yv = -math.cos(angleRad)
        ctx.set_line_width(width)
        ctx.move_to(0,0)
        ctx.line_to(radius * xv, radius * yv)
        ctx.stroke()

    def drawAnalog(self,ctx,radius):
        ctx.save()
        ctx.translate(radius,radius)
        ctx.set_line_width(1)
        ctx.arc(0,0, radius, 0, math.radians(360))
        ctx.stroke()
        now = datetime.datetime.now()
        min = now.minute
        hours = (now.hour % 12) * 60 + min
        self.drawHand(ctx, hours, 12*60, radius*0.6, 2)
        self.drawHand(ctx, min, 60, radius*0.85, 1)
        for i in range(60):
           self.drawRadial(ctx, i, 60, (i % 5) == 0, radius)
        ctx.restore()
        return 0

    def getAnalogHeight(self,ctx,radius):
        return int(2 * radius)

    def getDigitalHeight(self,ctx,font,fmt):
        layout = PangoCairo.create_layout(ctx)
        font_description = Pango.font_description_from_string(font)
        layout.set_font_description(font_description)
        layout.set_text(fmt)
        size = layout.get_pixel_size()
        return int(size.height)

    def drawDigital(self,ctx,font,fmt,radius):
        now = datetime.datetime.now()
        locale.setlocale(locale.LC_ALL, '')
        layout = PangoCairo.create_layout(ctx)
        font_description = Pango.font_description_from_string(font)
        layout.set_font_description(font_description)
        layout.set_line_spacing(1.2)
        layout.set_text(now.strftime(fmt));
        if (radius > 0):
            size = layout.get_pixel_size();
            ctx.move_to(radius - size.width / 2, radius - size.height / 2)
        else:
            ctx.move_to(0,0)
        PangoCairo.show_layout(ctx, layout)
        return 0
