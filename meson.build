project('background', 'c', 'cpp'
    , default_options: ['c_std=c11'
                       , 'cpp_std=c++20'
                       , 'buildtype=debugoptimized'
                       , 'warning_level=3']
    , license : 'GPL-3.0'
    , meson_version: '>=1.3.0'
    , version: '0.1')

fs = import('fs')

pkgdatadir = join_paths(get_option('prefix'), get_option('datadir'), meson.project_name())
pkglocaledir = join_paths(get_option('prefix'), get_option('datadir'), 'locale')
pkgsrcdir = fs.relative_to( meson.global_source_root(), meson.current_build_dir()) + '/src'
add_project_arguments('-DPACKAGE_DATA_DIR="' + pkgdatadir + '"', language: ['cpp','c'])
add_project_arguments('-DPACKAGE="' + meson.project_name() + '"', language: ['cpp','c'])
# this is to recreate the conventions from autotools
add_project_arguments('-DPACKAGE_SRC_DIR="' + pkgsrcdir + '"', language: ['cpp','c'])

compiler_add_flags = [  # limit the compiler noisiness
      '-Wno-unused-parameter'
    ]
cc = meson.get_compiler('c')
foreach ccflag : compiler_add_flags
    if cc.has_argument(ccflag)
        add_project_arguments(ccflag, language: ['c', 'cpp'])
    endif
endforeach

libharu_deps = declare_dependency(dependencies : cc.find_library('hpdf', required: false))


conf = configuration_data()
conf.set_quoted('VERSION', meson.project_version())     # Surround the version in quotes to make it a C string
conf.set       ('USE_PYTHON', get_option('python'))
conf.set       ('USE_PDF', libharu_deps.found())
configure_file(output : meson.project_name() + '_config.h'
               , configuration : conf)

genericimg_deps = dependency('genericimg', version :'>= 0.4.8')

# gtkmm3_deps     = dependency('gtkmm-3.0')
# glibmm2_deps    = dependency('glibmm-2.4 giomm-2.4')
python_deps     = declare_dependency()
thread_deps     = dependency('threads')
if get_option('python')
   python_deps    = dependency('python3-embed py3cairo pygobject-3.0')
endif
deps = [
      thread_deps
    , genericimg_deps
    , python_deps
    ]
subdir('src')
subdir('test')
subdir('res')
sources += app_resources

project_target = executable(meson.project_name()
    , sources
    , dependencies: deps
    , install: true
    , win_subsystem: 'windows'  # disable this to see console output for windows
    )
