# created RPf 17.4.2025
# allows tweaking by placing a copy in $home/.local/share/background/

class Cal:
    def __init__(self):
        return

    # build a array to show month page
    #   fills in 0 for not actual month
    def build(self,year,month):
        import calendar
        import datetime

        # maybe integrate print(f" firstweekday {cal.getfirstweekday()}")
        cal = calendar.Calendar()
        self.weeknums=[]
        self.rows=[]
        cols=[]
        for day in cal.itermonthdays(year, month):
            if (day > 0):
                weekday=calendar.weekday(year, month, day)
                if ((len(self.weeknums) == 0) or (weekday == 0)):
                    self.weeknums.append(datetime.date(year, month, day).isocalendar().week)
                if (weekday == 0):
                     self.rows.append(cols)
                     cols=[]
            cols.append(day)
        if (len(cols) > 0):
            self.rows.append(cols)
        return

    def putText(self,ctx,layout,col,row,halign = 1.0,colSpan = 1):
        import cairo
        import gi
        gi.require_version('Pango', '1.0')
        gi.require_version('PangoCairo', '1.0')
        from gi.repository import Pango, PangoCairo

        #print(f"put text {layout.get_text()} col {col} row {row}")
        x = col * self.m_cellWidth
        y = row * self.m_cellHeight
        size=layout.get_pixel_size()
        baseline = layout.get_baseline()
        #print(f"put width {size.width} height {size.height} base {baseline}")
        cellWidth = colSpan * self.m_cellWidth
        cellHeight = self.m_cellHeight
        ctx.move_to(x + (cellWidth - size.width) * halign
                   ,y + (cellHeight - baseline / 1024))
        PangoCairo.show_layout(ctx, layout)
        return

    def grid(self,ctx,font,year,month,today):
        import cairo
        import calendar
        import locale
        import datetime
        import gi
        gi.require_version('Pango', '1.0')
        gi.require_version('PangoCairo', '1.0')
        from gi.repository import Pango, PangoCairo

        layout = PangoCairo.create_layout(ctx)
        font_description = Pango.font_description_from_string(font)
        layout.set_font_description(font_description)
        small_font=font_description
        small_font.set_size(small_font.get_size() * 0.6)
        #print(f"Using small {small_font.get_size()} ")
        smallLayout = PangoCairo.create_layout(ctx)
        smallLayout.set_font_description(small_font)
        font_bold = Pango.font_description_from_string(font)
        font_bold.set_weight(Pango.Weight.BOLD)
        boldLayout = PangoCairo.create_layout(ctx)
        boldLayout.set_font_description(font_bold)
        layout.set_text("M")
        size=layout.get_pixel_size()
        self.m_cellHeight = size.height
        #print(f"grid width {size.width} height {self.m_cellHeight}")
        self.m_cellWidth = size.width * 2.5
        locale.setlocale(locale.LC_ALL, '')
        tcal = calendar.TextCalendar()
        layout.set_text(tcal.formatmonthname(year, month, width=0, withyear=False))
        self.putText(ctx,layout, 1, 0, 0.5, 7)
        for weekday in range(7):
            layout.set_text(f"{tcal.formatweekday(weekday, 2)}")
            self.putText(ctx,layout, weekday+1, 1)
        for i, row in enumerate(self.rows):
            if (i < len(self.weeknums)):
                #print(f"writing weeknum {self.weeknums[i]}")
                smallLayout.set_text(f"{self.weeknums[i]}")
                self.putText(ctx, smallLayout, 0, i+2)
            for j, day in enumerate(row):
                if (day > 0):
                    #print(f"writing day {day} {j} {i}")
                    dayLayout = layout
                    if (day == today):
                        dayLayout = boldLayout
                    dayLayout.set_text(f"{day}")
                    self.putText(ctx, dayLayout, j+1, i+2)
        return

    def draw(self,ctx,font):
        import math
        import datetime

        now = datetime.datetime.now()
        self.build(now.year, now.month)
        #print(f"draw got rows {len(self.rows)}")
        self.grid(ctx, font, now.year, now.month, now.day);
        return 0


#cal=Cal()
#cal.draw()