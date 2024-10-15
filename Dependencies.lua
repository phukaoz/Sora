IncludeDir = {}
IncludeDir["glfw"] = "%{wks.location}/Sora/vendor/glfw/include"
IncludeDir["Glad"] = "%{wks.location}/Sora/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Sora/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Sora/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Sora/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Sora/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Sora/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/Sora/vendor/ImGuizmo"
IncludeDir["shaderc"] = "%{wks.location}/Sora/vendor/shaderc/include"
IncludeDir["VulkanSDK"] = "%{wks.location}/Sora/vendor/VulkanSDK/include"
IncludeDir["box2d"] = "%{wks.location}/Sora/vendor/box2d/include"
IncludeDir["mono"] = "%{wks.location}/Sora/vendor/mono/include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{wks.location}/Sora/vendor/VulkanSDK/Lib"
LibraryDir["mono"] = "%{wks.location}/Sora/vendor/mono/lib/%{cfg.buildcfg}"

Library = {}

Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMultiMedia"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"