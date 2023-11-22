## build resources
##   the autoreconf complains about the $(shell) construct, fix see configure.ac  -Wno-portability.
##   But this seems the best way to keep this portable

PKGCONFIG = pkg-config
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)

RESOURCE_XML = background.gresources.xml
RESOURCE_C = ../src/resources.c
NOMAD_RESOURCES = $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=. --generate-dependencies $(RESOURCE_XML))

all:$(RESOURCE_C)

$(RESOURCE_C): $(RESOURCE_XML) $(NOMAD_RESOURCES)
	$(GLIB_COMPILE_RESOURCES) --target=$(RESOURCE_C) --sourcedir=. --generate-source $(RESOURCE_XML)

# The desktop files
desktopdir = $(datadir)/applications
desktop_in_files=background.desktop.in
desktop_DATA=$(desktop_in_files:.desktop.in=.desktop)

# application icon
appicondir = $(datadir)/icons/hicolor/64x64/apps
appicon_DATA = background.png
appdir_icon=$(appicondir)/$(appicon_DATA)

dist_pkgdata_DATA = hipparcos.json mw.json SnT_constellation.txt

# include source for build
EXTRA_DIST = $(RESOURCE_XML) $(RESOURCES) $(desktop_in_files) $(appicon_DATA) $(dist_pkgdata_DATA)

clean-local:
	-rm -rf $(RESOURCE_C)
	-rm -rf $(desktop_DATA)

$(desktop_DATA): $(desktop_in_files)
	@regex_cmd@ -e "s,\@APP_ICON\@,$(appdir_icon),g" $? > $@
