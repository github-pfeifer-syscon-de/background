
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
	, 'Sun.cpp'
	, 'Star.cpp'
	, 'Polyline.cpp'
	, 'Constellation.cpp'
	, 'HipparcosStar.cpp'
	, 'HipparcosFormat.cpp'
	, 'ConstellationFormat.cpp'
	, 'SysInfo.cpp'
	, 'Milkyway.cpp'
	, 'Poly.cpp'
	, 'ParamDlg.cpp'
	, 'TimeDlg.cpp'
	, 'BackPaint.cpp'
	, 'GeoPaint.cpp'
	, 'GeoBitmap.cpp'
	, 'GeoConversion.cpp'
	, 'BackConfig.cpp'
	, 'ConfigGeoJsonGrid.cpp'
	, 'ConfigGridFlights.cpp'
	, 'ConfigStarGrid.cpp'
	, 'ModulePaint.cpp'
	, 'FlightsDlg.cpp'
    )

astro_srcs = files(		# used for test
	  'JulianDate.cpp'
	, 'GeoPosition.cpp'
	, 'AzimutAltitude.cpp'
	, 'RaDec.cpp'
	, 'RaDecPlanet.cpp'
	, 'Point2D.cpp'
	, 'Layout.cpp'
	, 'Math.cpp'
	, 'Planet.cpp'
	, 'Planets.cpp'
	, 'Messier.cpp'
	, 'MessierLoader.cpp'
	, 'FileLoader.cpp'
	, 'Phase.cpp'
	, 'Renderer.cpp'
	, 'Moon.cpp'
)
if libharu_deps.found()
	astro_srcs += files(
		'HaruRenderer.cpp'
	)
endif
sources += astro_srcs

if get_option('python')
sources += files(
	  'PyWrapper.cpp'
    )
endif


if target_machine.system() == 'windows'
  sources += files('SysInfoWindows.cpp')
else
  sources += files('SysInfoLinux.cpp') # may need adaptions if != linux
endif