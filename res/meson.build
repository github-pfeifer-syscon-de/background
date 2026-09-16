
#
# see https://mesonbuild.com/Gnome-module.html
gnome = import('gnome')

app_resources = gnome.compile_resources(
    'resources', meson.project_name() + '.gresources.xml'
    , source_dir: meson.current_source_dir()
)

# icons + desktop
dataDir = join_paths(get_option('prefix'), get_option('datadir'))
iconDir = join_paths(dataDir, 'icons/hicolor')
install_data(
      meson.project_name() + '.png'
    , install_dir: join_paths(iconDir, '64x64/apps'))
install_data(
      meson.project_name() + '.svg'
    , install_dir: join_paths(iconDir, 'scalable/apps'))
install_data(
      meson.project_name() + '.desktop'
    , install_dir: join_paths(dataDir, 'applications'))

curl_exec = find_program('curl', required: true)
texture_files = [
    '2k_earth_daymap.jpg'
]
foreach texture_file : texture_files
    url = 'https://www.solarsystemscope.com/textures/download/' + texture_file
    custom_target(
          command          : [curl_exec, url, '-o', '@OUTPUT@']
        , output           : texture_file
#        , build_by_default : true      # this may result in always rebuild with install ???
        , install          : true
        , install_dir      : pkgdatadir)
endforeach


# Data-files
install_data(
    ['hipparcos.json'
    , 'mw.json'
    , 'SnT_constellation.txt'
    , 'messier.json'
    , 'clock.py'
    , 'info.py'
    , 'cal.py'
    , 'DSEG14Modern-Regular.ttf'
    ]
    , install_dir: pkgdatadir)

win_res_file = []
if target_machine.system() == 'windows'     # expects resource compiler, disable if you don't have one (part of msys2 but not claimed by any package... )
    windows = import('windows')
    win_res_file = windows.compile_resources('win.rc'
                            ,args: []
                            ,depend_files: [meson.project_name() + '.ico']
                           )

endif