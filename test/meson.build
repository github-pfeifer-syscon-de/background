

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
