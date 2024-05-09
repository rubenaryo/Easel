workspace "Easel"
    architecture "x64"
    startproject "IsoDungeon"

    configurations
    {
        "Debug",
        "Release"
    }
    
    flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}x64"

project "Easel"
    location "Easel"
    kind "SharedLib"
    language "C++"

    targetdir ("_bin/" .. outputdir .. "/%{prj.name}")
    objdir ("_int/" .. outputdir .. "/%{prj.name}")
    shaderobjectfileoutput ("%{!wks.location}/_bin/Shaders/%%(Filename).cso")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "external/**/include/"
    }

    libdirs
    {
        "external/assimp/",
        "external/dxtk/%{cfg.buildcfg}/"
    }

    links
    {
        "assimp",
        "DirectXTK",
        "DirectXTKAudioWin7"
    }

    filter "system:Windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "ESL_PLATFORM_WINDOWS",
            "ESL_BUILD_DLL"
        }

        prebuildcommands
        {
            ("{MKDIR} %{!wks.location}/_bin/" .. outputdir .. "/IsoDungeon")
        }

        postbuildcommands
        {
            ("{COPYFILE} %{!cfg.buildtarget.abspath} %{!wks.location}_bin/".. outputdir .. "/IsoDungeon/%{prj.name}.dll"),
            ("{COPYFILE} %{!wks.location}/external/assimp/Assimp64.dll %{!wks.location}_bin/".. outputdir .. "/IsoDungeon/Assimp64.dll")
        }

    filter "configurations:Debug"
        defines "ESL_DEBUG"
        symbols "On"
        staticruntime "Off"
        shadermodel "5.0"

    filter "configurations:Release"
        defines "ESL_RELEASE"
        optimize "On"
        staticruntime "Off"
        shadermodel "5.0"

project "IsoDungeon"
    location "IsoDungeon"
    kind "WindowedApp"
    language "C++"

    targetdir ("_bin/" .. outputdir .. "/%{prj.name}")
    objdir ("_int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Easel/src"
    }

    links
    {
        "Easel"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "ESL_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "ESL_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "ESL_RELEASE"
        optimize "On"

project "Shaders"
    location "Assets/Shaders"
    kind "ConsoleApp"
    shadermodel "5.0"

    shaderobjectfileoutput ("%{!wks.location}/_bin/Shaders/%%(Filename).cso")
    objdir ("_int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{!wks.location}/Assets/Shaders/**.hlsl",
        "%{!wks.location}/Assets/Shaders/**.hlsli"
    }

    filter { "files:**PS.hlsl" }
        shadertype "Pixel"

    filter { "files:**VS.hlsl" }
        shadertype "Vertex"
