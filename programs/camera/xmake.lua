add_rules("plugin.vsxmake.autoupdate")
add_rules("mode.debug", "mode.release")

add_includedirs("include", "src")

if is_plat("windows") then
	--set_runtimes(is_mode("debug") and "MDd" or "MD")
end

if is_arch("arm.*") then
    add_defines("ARM")
    add_requires("libcamera", "libevent_pthreads")
    add_requires("opencv >=4.10.0", {configs = {gtk = true}})--is_mode("debug")}})
else
    add_requires("opencv >=4.10.0")--is_mode("debug")}})    
end

add_requires("matplotplusplus", "spdlog")
--add_requires("implot")

target("camera")
    set_kind("binary")
    set_languages("cxx20")

    add_files("src/**.cpp")
    add_headerfiles("src/**.hpp", "src/**.h")
    add_headerfiles("include/**.hpp", "include/**.h")

    if not is_arch("arm.*") then
        remove_files("src/camera/event_loop.cpp", "src/camera/mapped_framebuffer.cpp")
        remove_headerfiles("include/camera/event_loop.hpp", "include/camera/mapped_framebuffer.hpp")
    end

    add_packages("opencv")
    add_packages("matplotplusplus", "spdlog") --, "implot")

    if is_arch("arm.*") then
        add_packages("libcamera", "libevent_pthreads")
    end

    after_build(function (target)
        os.mkdir(path.join(target:targetdir(), "assets"))
        os.cp("assets/*", path.join(target:targetdir(), "assets"))
    end)

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

