



%.cpp:
	ln -sf ../src/$@

%.hpp:
	ln -sf ../src/$@

SOURCE = JulianDate.cpp JulianDate.hpp \
	GeoPosition.cpp GeoPosition.hpp \
	AzimutAltitude.cpp AzimutAltitude.hpp \
	RaDec.cpp RaDec.hpp \
	RaDecPlanet.cpp RaDecPlanet.hpp \
	Point2D.cpp Point2D.hpp \
	Layout.cpp Layout.hpp \
	Math.cpp Math.hpp \
	Planet.cpp Planet.hpp \
	Earth.cpp Earth.hpp \
	Venus.cpp Venus.hpp

include_source: $(SOURCE)

TESTS: include_source

TESTS = astro_test

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)."\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(GENERICIMG_CFLAGS)

AM_CFLAGS= \
	-Wall -Wpedantic

AM_CXXFLAGS = \
	-Wall -Wpedantic

AM_LDFLAGS =

astro_test_LDADD =  \
	$(GENERICIMG_LIBS)

check_PROGRAMS = astro_test


astro_test_SOURCES = \
    astro_test.cpp \
    $(SOURCE)


