
bin_PROGRAMS = background

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
        -DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
        -DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(GLIBMM_CFLAGS) \
	$(GTKMM_CFLAGS) \
	$(GENERICIMG_CFLAGS) \
	$(JSONGLIB1_CFLAGS) \
	$(HARU_CFLAGS)

background_CFLAGS = -Wall -Wconversion -Wextra
background_CXXFLAGS = -Wall -Wconversion -Wextra

background_LDFLAGS = @EXTRA_LDFLAGS@

background_LDADD =  \
	$(GLIBMM_LIBS) \
	$(GTKMM_LIBS) \
	$(GENERICIMG_LIBS) \
	$(JSONGLIB1_LIBS) \
	$(HARU_LIBS)

background_SOURCES = resources.c \
	BackgroundApp.hpp \
	BackgroundApp.cpp \
	StarWin.hpp \
	StarWin.cpp \
	StarDraw.hpp \
	StarDraw.cpp \
	Math.hpp \
	Math.cpp \
	RaDec.hpp \
	RaDec.cpp \
	AzimutAltitude.hpp \
	AzimutAltitude.cpp \
	Point2D.hpp \
	Point2D.cpp \
	Layout.hpp \
	Layout.cpp \
	GeoPosition.hpp \
	GeoPosition.cpp \
	JulianDate.hpp \
	JulianDate.cpp \
	Phase.hpp \
	Phase.cpp \
	Sun.hpp \
	Sun.cpp \
	Moon.hpp \
	Moon.cpp \
	Star.hpp \
	Star.cpp \
	Polyline.hpp \
	Polyline.cpp \
	Constellation.hpp \
	Constellation.cpp \
	HipparcosStar.hpp \
	HipparcosStar.cpp \
	HipparcosFormat.hpp \
	HipparcosFormat.cpp \
	ConstellationFormat.hpp \
	ConstellationFormat.cpp \
	Planet.hpp \
	Planet.cpp \
	Earth.hpp \
	Earth.cpp \
	Neptune.hpp \
	Neptune.cpp \
	Uranus.hpp \
	Uranus.cpp \
	Saturn.hpp \
	Saturn.cpp \
	Jupiter.hpp \
	Jupiter.cpp \
	Mars.hpp \
	Mars.cpp \
	Venus.hpp \
	Venus.cpp \
	SysInfo.hpp \
	SysInfo.cpp \
	Mercury.hpp \
	Mercury.cpp


if HAVE_PDF
background_SOURCES +=
endif