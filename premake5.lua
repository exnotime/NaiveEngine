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
		files { "source/core/**.h","source/core/**.cpp", "source/imgui/*" }
		includedirs { "include", "source" }
		links { "glfw3", "glfw3dll", "GFX_GL", "GFX_DX", "physics", "utility", "opengl32", "glew32"}
		configuration {"Release"}
			links {"BulletDynamics", "BulletCollision", "BulletLinearMath" }
		configuration {"Debug"}
			links {"BulletDynamics_Debug", "BulletCollision_Debug", "BulletLinearMath_Debug" }

	project "GFX_GL"
		targetname "GFX_GL"
		debugdir ""
		location ( location_path )
		language "C++"
		kind "SharedLib"
		defines { "GFX_DLL_EXPORT","GLM_FORCE_RADIANS" }
		files {"source/gfx/*.cpp", "source/gfx/*.h"}
		includedirs { "include", "source"}
		links { "Soil2", "assimp", "opengl32", "glew32" , "utility"}

	project "GFX_DX"
		targetname "GFX_DX"
		debugdir ""
		location ( location_path )
		language "C++"
		kind "SharedLib"
		defines { "GFX_DX_DLL_EXPORT","GLM_FORCE_RADIANS" }
		files {"source/gfx_dx/*.cpp", "source/gfx_dx/*.h"}
		includedirs { "include", "source"}
		links { "Soil2", "assimp", "d3d12", "dxgi", "d3dcompiler" ,"utility" ,"opengl32"}

	project "PHYSICS"
		targetname "PHYSICS"
		debugdir ""
		location ( location_path )
		language "C++"
		kind "SharedLib"
		defines { "PHYSICS_DLL_EXPORT", "GLM_FORCE_RADIANS" }
		files { "source/physics/*.cpp", "source/physics/*.h"}
		includedirs { "include", "source" }
		configuration {"Release"}
			links {"BulletDynamics", "BulletCollision", "BulletLinearMath", "utility"}
		configuration {"Debug"}
			links {"BulletDynamics_Debug", "BulletCollision_Debug", "BulletLinearMath_Debug" , "utility"}

	project "UTILITY"
		targetname "UTILITY"
		debugdir ""
		location ( location_path )
		language "C++"
		kind "SharedLib"
		defines "UTILITY_DLL_EXPORT"
		files { "source/utility/*.cpp", "source/utility/*.h"}
		includedirs { "include", "source" }

