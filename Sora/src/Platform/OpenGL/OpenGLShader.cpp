#include "sorapch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <filesystem>

#include <glad/glad.h>
                                                                                                                                                                                                                                                                                                                               
#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>

#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "Sora/Core/Timer.h"

namespace Sora {

	namespace Utils {

		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;

			SORA_CORE_ASSERT(false, "Unknown shader type '{0}'", type);
			return 0;
		}

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:		return shaderc_glsl_vertex_shader;
				case GL_FRAGMENT_SHADER:	return shaderc_glsl_fragment_shader;
			}

			SORA_CORE_ASSERT(false, "Unknown shader stage");
			return (shaderc_shader_kind)0;
		}

		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:		return "GL_VERTEX_SHADER";
				case GL_FRAGMENT_SHADER:	return "GL_FRAGMENT_SHADER";
			}

			SORA_CORE_ASSERT(false, "Unknown shader stage");
			return nullptr;
		}

		static const char* GetCacheDirectory()
		{
			return "assets/cache/shader/opengl";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cache_directory = GetCacheDirectory();
			if (!std::filesystem::exists(cache_directory))
				std::filesystem::create_directories(cache_directory);
		}

		static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:		return ".cached_opengl.vert";
				case GL_FRAGMENT_SHADER:	return ".cached_opengl.frag";
			}

			SORA_CORE_ASSERT(false, "Unknown shader stage");
			return "";
		}

		static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:		return ".cached_vulkan.vert";
				case GL_FRAGMENT_SHADER:	return ".cached_vulkan.frag";
			}

			SORA_CORE_ASSERT(false, "Unknown shader stage");
			return "";
		}
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: mFilepath(filepath)
	{
		SORA_PROFILE_FUNCTION();

		Utils::CreateCacheDirectoryIfNeeded();

		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);

		{
			Timer timer;
			CompileOrGetVulkanBinaries(shaderSources);
			CompileOrGetOpenGLBinaries();
			CreateProgram();
			SORA_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		}

		auto last_slash = filepath.find_last_of("/\\");
		last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;
		auto last_dot = filepath.rfind('.');
		auto count = last_dot == std::string::npos ? filepath.size() - last_slash : last_dot - last_slash;
		mName = filepath.substr(last_slash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertex_src, const std::string& fragment_src)
		: mName(name)
	{
		SORA_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertex_src;
		sources[GL_FRAGMENT_SHADER] = fragment_src;
		
		CompileOrGetVulkanBinaries(sources);
		CompileOrGetOpenGLBinaries();
		CreateProgram();
	}

	OpenGLShader::~OpenGLShader()
	{
		SORA_PROFILE_FUNCTION();

		glDeleteProgram(mRendererID);
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		SORA_PROFILE_FUNCTION();

		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			SORA_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		SORA_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shader_sources;

		const char* type_token = "#type";
		size_t type_token_length = strlen(type_token);
		size_t pos = source.find(type_token, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			SORA_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + type_token_length + 1;
			std::string type = source.substr(begin, eol - begin);
			SORA_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specified '{0}'", type);

			size_t next_line_pos = source.find_first_of("\r\n", eol);
			pos = source.find(type_token, next_line_pos);
			shader_sources[Utils::ShaderTypeFromString(type)] = source.substr(next_line_pos, pos - (next_line_pos == std::string::npos ? source.size() - 1 : next_line_pos));
		}

		return shader_sources;
	}

	void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shader_sources)
	{
		GLuint program = glCreateProgram();
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cache_directory = Utils::GetCacheDirectory();

		auto& shader_data = mVulkanSPIRV;
		shader_data.clear();
		for (auto&& [stage, source] : shader_sources)
		{
			std::filesystem::path shader_file_path = mFilepath;
			std::filesystem::path cache_path = cache_directory / (shader_file_path.filename().string() +
				Utils::GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cache_path, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shader_data[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage),
					mFilepath.c_str(), options);
				SORA_CORE_ASSERT(module.GetCompilationStatus() == shaderc_compilation_status_success, module.GetErrorMessage());
				
				shader_data[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cache_path, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shader_data[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : shader_data)
			Reflect(stage, data);
	}

	void OpenGLShader::CompileOrGetOpenGLBinaries()
	{
		auto& shader_data = mOpenGLSPIRV;
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cache_directory = Utils::GetCacheDirectory();
		
		shader_data.clear();
		mOpenGLSourceCode.clear();
		for (auto&& [stage, spirv] : mVulkanSPIRV)
		{
			std::filesystem::path shader_file_path = mFilepath;
			std::filesystem::path cache_path = cache_directory / (shader_file_path.filename().string() +
				Utils::GLShaderStageCachedOpenGLFileExtension(stage));

			std::ifstream in(cache_path, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shader_data[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glsl_compiler(spirv);
				mOpenGLSourceCode[stage] = glsl_compiler.compile();
				auto& source = mOpenGLSourceCode[stage];

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage),
					mFilepath.c_str(), options);
				SORA_CORE_ASSERT(module.GetCompilationStatus() == shaderc_compilation_status_success, module.GetErrorMessage());

				shader_data[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cache_path, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shader_data[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram();
		std::vector<GLuint> shader_ids;
		for (auto&& [stage, spirv] : mOpenGLSPIRV)
		{
			GLuint id = shader_ids.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &id, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), static_cast<int>(spirv.size() * sizeof(uint32_t)));
			glSpecializeShader(id, "main", 0, nullptr, nullptr);
			glAttachShader(program, id);
		}

		glLinkProgram(program);

		GLint is_linked;
		glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint max_length = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

			std::vector<GLchar> info_log(max_length);
			glGetProgramInfoLog(program, max_length, &max_length, info_log.data());
			SORA_CORE_ERROR("Shader linking failed ({0}):\n{1}", mFilepath, info_log.data());

			glDeleteProgram(program);

			for (auto id : shader_ids)
				glDeleteShader(id);
		}

		for (auto id : shader_ids)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		mRendererID = program;
	}

	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shader_data)
	{
		spirv_cross::Compiler compiler(shader_data);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		SORA_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), mFilepath);
		SORA_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		SORA_CORE_TRACE("    {0} resources", resources.sampled_images.size());

		SORA_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& buffer_type = compiler.get_type(resource.base_type_id);
			size_t buffer_size = compiler.get_declared_struct_size(buffer_type);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			size_t member_count = buffer_type.member_types.size();

			SORA_CORE_TRACE("    {0}", resource.name);
			SORA_CORE_TRACE("    Size = {0}", buffer_size);
			SORA_CORE_TRACE("    Binding = {0}", binding);
			SORA_CORE_TRACE("    Members = {0}", member_count);
		}
	}

	void OpenGLShader::Bind() const
	{
		SORA_PROFILE_FUNCTION();

		glUseProgram(mRendererID);
	}

	void OpenGLShader::Unbind() const
	{
		SORA_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		SORA_PROFILE_FUNCTION();

		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		SORA_PROFILE_FUNCTION();

		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		SORA_PROFILE_FUNCTION();

		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		SORA_PROFILE_FUNCTION();

		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		SORA_PROFILE_FUNCTION();

		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value) 
	{
		SORA_PROFILE_FUNCTION();

		UploadUniformMat4(name, value);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value) const
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count) const
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value) const
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value) const
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value) const
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value) const
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& value) const
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& value) const
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

}
