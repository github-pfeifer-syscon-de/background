


TESTS = astro_test

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)."\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(GENERICIMG_CFLAGS) \
	$(HARU_CFLAGS) \
	-I ../src

AM_CFLAGS= \
	-Wall -Wpedantic

AM_CXXFLAGS = \
	-Wall -Wpedantic

AM_LDFLAGS =

astro_test_LDADD =  \
	../src/background-JulianDate.o \
	../src/background-GeoPosition.o \
	../src/background-AzimutAltitude.o \
	../src/background-RaDec.o \
	../src/background-RaDecPlanet.o \
	../src/background-Point2D.o \
	../src/background-Layout.o \
	../src/background-Math.o \
	../src/background-Planet.o \
	../src/background-Planets.o \
	../src/background-Messier.o \
	../src/background-MessierLoader.o \
	../src/background-FileLoader.o \
	../src/background-Phase.o \
	../src/background-Renderer.o \
	../src/background-HaruRenderer.o \
	../src/background-Moon.o \
	$(HARU_LIBS) \
	$(GENERICIMG_LIBS)

check_PROGRAMS = astro_test


astro_test_SOURCES = \
    astro_test.cpp


