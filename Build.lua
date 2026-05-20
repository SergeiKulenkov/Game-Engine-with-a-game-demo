workspace "Engine and Game Demo"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Game"

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"
IncludeDir = {}

VULKAN_SDK = os.getenv("VULKAN_SDK")
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["glm"] = "../Vendor/glm"
IncludeDir["glfw"] = "../Vendor/glfw/include"
IncludeDir["ImGUI"] = "../Vendor/ImGUI"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

include "Engine/Build-Engine.lua"
include "GameDemo/Build-Game.lua"