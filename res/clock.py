# created RPf 17.4.2025
# allows tweaking by placing a copy in $home/.local/share/background/

class Clock:
    def __init__(self):
        return

    def drawRadial(self,ctx,value,full,emphasis,radius):
        import cairo
        import math

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

    def drawHand(self,ctx,value,full,radius,width):
        import cairo
        import math

        angleRad = math.radians(360) * value / full
        xv = math.sin(angleRad)
        yv = -math.cos(angleRad)
        ctx.set_line_width(width)
        ctx.move_to(0,0)
        ctx.line_to(radius * xv, radius * yv)
        ctx.stroke()

    def draw(self,ctx,radius):
        import cairo
        import math
        import datetime

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

