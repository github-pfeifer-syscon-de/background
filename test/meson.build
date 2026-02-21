

# also make config available here
configure_file (output : meson.project_name() + '_config.h'
    , configuration : conf)
incl_dir = include_directories('../src')

astro_test = executable('astro_test'
    , 'astro_test.cpp'
	, '../src/JulianDate.cpp'
	, '../src/GeoPosition.cpp'
	, '../src/AzimutAltitude.cpp'
	, '../src/RaDec.cpp'
	, '../src/RaDecPlanet.cpp'
	, '../src/Point2D.cpp'
	, '../src/Layout.cpp'
	, '../src/Math.cpp'
	, '../src/Planet.cpp'
	, '../src/Planets.cpp'
	, '../src/Messier.cpp'
	, '../src/MessierLoader.cpp'
	, '../src/FileLoader.cpp'
	, '../src/Phase.cpp'
	, '../src/Renderer.cpp'
	, '../src/HaruRenderer.cpp'
	, '../src/Moon.cpp'
    , dependencies        : deps
    , include_directories : incl_dir
    )
test('astro_test', astro_test)

