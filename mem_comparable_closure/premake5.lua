GOOGLE_BENCHMARK_LIBDIRS = {}

workspace "mem_comparable_closure"
    configurations { "Test"}
    targetdir "bin"


project "mem_comparable_closure"
    kind "ConsoleApp"
    toolset "clang"
    language "C++"
    cppdialect "C++17"
    files { "src/*.cpp"  }
    includedirs {"include", "src/include"}
    removefiles { "libs/allocators/*.t.cpp"}
    buildoptions { "-Wall", "-Wno-unused-local-typedef"}
    optimize "Full"
    filter "Test"
        files {"test/*.cpp"}
	includedirs "libs/doctest/doctest"
	targetdir "bin/Test"
