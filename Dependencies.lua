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

LibraryDir = {}
LibraryDir = "%{wks.location}/Sora/vendor/VulkanSDK/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir}/vulkan-1.lib"

Library["ShaderC_Debug"] = "%{LibraryDir}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir}/spirv-cross-glsl.lib"