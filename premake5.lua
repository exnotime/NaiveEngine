solution "NaiveEngine"
    configurations { "Debug", "Release" }
        flags{ "Unicode", "NoPCH" }
        libdirs { "lib" }
        includedirs { "include"}
        platforms{"x64"}

        local location_path = "solution"

        if _ACTION then
	        defines { "_CRT_SECURE_NO_WARNINGS", "NOMINMAX" }

            location_path = location_path .. "/" .. _ACTION
            location ( location_path )
            location_path = location_path .. "/projects"
        end

    configuration { "Debug" }
        defines { "DEBUG" }
        flags { "Symbols" }

    configuration { "Release" }
        defines { "NDEBUG", "RELEASE" }
        flags { "Optimize", "FloatFast" }

    configuration { "Debug" }
        targetdir ( "bin/" .. "/debug" )

    configuration { "Release" }
        targetdir ( "bin/" .. "/release" )

	project "CORE"
		targetname "CORE"
		debugdir ""
		location ( location_path )
		language "C++"
		kind "ConsoleApp"
		defines { "GLM_FORCE_RADIANS" }
		files { "source/core/**", "source/imgui/*" }
		includedirs { "include", "source" }
		links { "glfw3", "glfw3dll", "gfx", "opengl32", "glew32" }

	project "GFX"
		targetname "GFX"
		debugdir ""
		location ( location_path )
		language "C++"
		kind "SharedLib"
		defines { "GFX_DLL_EXPORT","GLM_FORCE_RADIANS" }
		files {"source/gfx/*.cpp", "source/gfx/*.h"}
		includedirs { "include", "source"}
		links { "Soil2", "assimp", "opengl32", "glew32" }
