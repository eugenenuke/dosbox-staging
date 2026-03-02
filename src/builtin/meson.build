libbuiltin_sources = files(
    'glide2x.cpp',
)

libbuiltin = static_library(
    'builtin',
    libbuiltin_sources,
    include_directories: incdir,
    dependencies: [
        sdl2_dep,
        libloguru_dep,
    ],
    cpp_args: warnings,
)

libbuiltin_dep = declare_dependency(link_with: libbuiltin)

internal_deps += libbuiltin_dep
