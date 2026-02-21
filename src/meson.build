
sources = files(
     'BackgroundApp.cpp'
	, 'StarWin.cpp'
	, 'StarMountOperation.cpp'
	, 'StarDraw.cpp'
	, 'StarPaint.cpp'
	, 'Module.cpp'
	, 'InfoModule.cpp'
	, 'CalendarModule.cpp'
	, 'ClockModule.cpp'
	, 'Math.cpp'
	, 'RaDec.cpp'
	, 'RaDecPlanet.cpp'
	, 'AzimutAltitude.cpp'
	, 'Point2D.cpp'
	, 'Layout.cpp'
	, 'GeoPosition.cpp'
	, 'JulianDate.cpp'
	, 'Phase.cpp'
	, 'Sun.cpp'
	, 'Moon.cpp'
	, 'Star.cpp'
	, 'Polyline.cpp'
	, 'Constellation.cpp'
	, 'HipparcosStar.cpp'
	, 'HipparcosFormat.cpp'
	, 'ConstellationFormat.cpp'
	, 'Planet.cpp'
	, 'Planets.cpp'
	, 'SysInfo.cpp'
	, 'Milkyway.cpp'
	, 'FileLoader.cpp'
	, 'Poly.cpp'
	, 'Messier.cpp'
	, 'MessierLoader.cpp'
	, 'ParamDlg.cpp'
	, 'Renderer.cpp'
	, 'TimeDlg.cpp'
    )

if get_option('python')
sources += files(
	  'PyWrapper.cpp'
    )
endif

if libharu_deps.found()
sources += files(
	  'HaruRenderer.cpp'
	)
endif