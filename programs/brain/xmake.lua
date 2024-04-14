add_rules("plugin.vsxmake.autoupdate")
add_rules("mode.debug", "mode.release")

add_includedirs("include", "src")

add_requires("sfml", "imgui-sfml")

set_symbols("debug")

set_languages("clatest", "cxxlatest")

set_optimize("fastest")

target("pathfinding")
    set_kind("binary")

    add_files("src/pathfinding/**.cpp")
    add_headerfiles("include/pathfinding/**.hpp")
    add_headerfiles("src/pathfinding/**.hpp")

    add_headerfiles("src/parameters.hpp")

    add_files("src/maths/**.cpp")
    add_headerfiles("include/maths/**.hpp")
    add_headerfiles("src/maths/**.hpp")

    add_packages("sfml", "imgui-sfml")

target("hal")
    set_kind("binary")

    add_files("src/hal/**.cpp")
    add_headerfiles("include/hal/**.hpp")
    add_headerfiles("src/hal/**.hpp")

    add_headerfiles("src/parameters.hpp")

    add_files("src/maths/**.cpp")
    add_headerfiles("include/maths/**.hpp")
    add_headerfiles("src/maths/**.hpp")

    add_packages("sfml", "imgui-sfml")

target("core")
    set_kind("binary")

    add_files("src/core/**.cpp")
    add_headerfiles("include/core/**.hpp")
    add_headerfiles("src/core/**.hpp")

    add_headerfiles("src/parameters.hpp")

    add_files("src/maths/**.cpp")
    add_headerfiles("include/maths/**.hpp")
    add_headerfiles("src/maths/**.hpp")

    add_files("src/world/**.cpp")
    add_headerfiles("include/world/**.hpp")
    add_headerfiles("src/world/**.hpp")

    add_files("src/gui/**.cpp")
    add_headerfiles("include/gui/**.hpp")
    add_headerfiles("src/gui/**.hpp")

    add_files("src/hal/**.cpp")
    add_headerfiles("include/hal/**.hpp")
    add_headerfiles("src/hal/**.hpp")
    remove_files("src/hal/main.cpp")

    add_packages("sfml", "imgui-sfml")

    set_policy("build.sanitizer.thread", true) -- Enable thread sanitizer to detect data races
    add_cxflags("-fsanitize=thread", "-ftrapv")
    add_mxflags("-fsanitize=thread", "-ftrapv")
    add_ldflags("-fsanitize=thread")


--    after_build(function (target)
--        os.mkdir(path.join(target:targetdir(), "assets"))
--        os.cp("assets/*", path.join(target:targetdir(), "assets"))
--    end)

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

