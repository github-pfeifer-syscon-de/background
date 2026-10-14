

test_incl_dir = [
      include_directories('../src')
    , conf_include
]

astro_test_srcs = files(
	 'astro_test.cpp'
)
astro_test_srcs += astro_srcs

astro_test = executable('astro_test'
    , astro_test_srcs
    , dependencies        : deps
    , include_directories : test_incl_dir
    )
test('astro_test', astro_test)

# test for converting offical german location data
#   in utm to wgs, the optional dependency proj is required to make this work
#   download dbf-data-variant from
#    https://gdz.bkg.bund.de/index.php/default/inspire-wfs-geographical-names-gn250-wfs-gn250-inspire.html
#   the directory that contains GN250_p.dbf should be used as parameter when starting gn250_conf
#   see gn250_conv sources, you may vary the selection to your taste
#     the output gn250.geo.json will be placed in the current directory
if projGeo_lib.found()
geo_test_srcs = files(
    'dbf.c'
  , 'dbf_endian.c'
  , 'Conversion.cpp'
  , 'DbfFile.cpp'
  , 'gn250_conv.cpp'
)
# just as an option the data will also be written as sqlite
if sqlite3_lib.found()
    sqlite = '../src/SqlLite.cpp'
    sql_test_srcs = files(
        'sql_test.cpp'
       , sqlite
    )
    sql_test = executable('sql_test'
                          , sql_test_srcs
                          , dependencies        : deps
                          , include_directories : test_incl_dir
    )
    geo_test_srcs += files(sqlite)
endif
dbf_test = executable('gn250_conv'
    , geo_test_srcs
    , dependencies        : deps
    , include_directories : test_incl_dir
)
endif
