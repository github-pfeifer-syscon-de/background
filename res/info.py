# created RPf 17.4.2025
#
# suggested by https://aperiodic.net/pip/archives/Geekery/cairo-pango-python/
# docs https://api.pygobject.gnome.org/PangoCairo-1.0/functions.htm
import gi
gi.require_version('Pango', '1.0')
gi.require_version('PangoCairo', '1.0')
from gi.repository import Pango, PangoCairo
import cairo
import platform
import os
import subprocess

class Info:
    def __init__(self):
        return

    def nodeName(self):
        return platform.node()

    def machine(self):
        return platform.machine()

    def osVersion(self):
        inf=os.uname()
        return inf.sysname + inf.release

    def proc(self):
        #import platform
        # this is a empty string
        #return platform.processor()
        file = open("/proc/cpuinfo", "r")
        cpuinfo=""
        for line in file:
            if line.startswith("model name	"):
                line = line.strip()
                pos = line.find(":")
                if pos > 0:
                   pos += 1
                   cpuinfo = line[pos:].strip()
                   break
        file.close()
        return cpuinfo

    def meminfo2mb(self,line):
        pos = line.find(":")
        if (pos > 0):
           pos += 1
           end = line.find("kB", pos)
           if (end > 0):
              return (int(line[pos:end]) / 1024) // 1
        return 0

    def mem(self):
        file = open("/proc/meminfo", "r")
        total=0
        avail=0
        for line in file:
            line = line.strip()
            if line.startswith("MemTotal:"):
                total = self.meminfo2mb(line);
            if line.startswith("MemAvailable:"):
                avail = self.meminfo2mb(line);
        file.close()
        used=(total-avail)
        if (total > 0):
            return f"{used:.0f}MB used of {total:.0f}MB is {used/total:.0%}"
        else:
            return "No infos found..."

    def netConn(self):
        import socket
        # at least with my current understanding of python i can't find
        #   a feasible way to extract all the network info ...
        #   (throwing more packages at it seems not very maintainable)
        #   may require install package e.g. python-netifaces
        #import netifaces
        # Return a list of network interface information
        #intf=socket.if_nameindex()
        #for if_num, if_name in intf:
        #   print(f"net {if_num} name {if_name}")
        #   name=socket.if_indextoname(if_num)
        #   print("   %s" % name)
        #   interfaces = netifaces.interfaces()
        #ips = []
        #interfaces = netifaces.interfaces()
        #for interface in interfaces:
        #   addrs = netifaces.ifaddresses(interface)
        #   if netifaces.AF_INET in addrs:
        #      ips.extend([addr['addr'] for addr in addrs[netifaces.AF_INET]])
        #   if netifaces.AF_INET6 in addrs:
        #      ips.extend([addr['addr'] for addr in addrs[netifaces.AF_INET6]])
        #hostname = socket.gethostname()
        #local_ip = socket.gethostbyname(hostname)
        #print(f"ip {ips[0]}")
        return ""

    # if you like fortune you may include this (need install fortune_mod)
    def fortune(self):
        result = subprocess.run(['fortune'],
                   capture_output = True, # Python >= 3.7 only
                   text = True)
        return result.stdout

    def draw(self,ctx,font,netinfo):
        layout = PangoCairo.create_layout(ctx)
        font_description = Pango.font_description_from_string(font)
        layout.set_font_description(font_description)
        text =  self.nodeName() + "\n"
        text += self.machine() + "\n"
        text += self.osVersion() + "\n"
        text += self.proc() + "\n"
        text += self.mem() + "\n"
        text += netinfo
        layout.set_text(text)
        ctx.move_to(0,0)
        PangoCairo.show_layout(ctx, layout)
        return 0

