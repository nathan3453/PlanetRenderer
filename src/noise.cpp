#include "noise.h"

Noise::Noise(int cubemapResolution, GLuint* cubemapNoiseTexture, GLuint* cubemapNormalTexture) {
	this->sampleOffsetSize = 0.05f;
	this->needToDispatch   = false;
	this->cubemapResolution = cubemapResolution;
	this->cubemapNoiseTexture = cubemapNoiseTexture;
	this->cubemapNormalTexture = cubemapNormalTexture;

	cubemapNoiseShaderProgram = CompileComputeShader("shaders/cubemapNoise.comp");
	cubemapNormalShaderProgram = CompileComputeShader("shaders/cubemapNormal.comp");

	noise_seedLocation = GetUniformLocation(cubemapNoiseShaderProgram, "u_Seed");
	normal_NoiseSamplerLocation = GetUniformLocation(cubemapNormalShaderProgram, "u_TerrainCubemap");	
	normal_SampleOffset = GetUniformLocation(cubemapNormalShaderProgram, "u_Offset");

	glUniform1f(normal_SampleOffset, sampleOffsetSize);
	CreateTextures();
	CreateFramebuffers();
}

void Noise::Dispatch(int seed) {
	// Cubemap noise generation
	glUseProgram(cubemapNoiseShaderProgram);
	glUniform1i(noise_seedLocation, seed);
	glBindImageTexture(0, *cubemapNoiseTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute(32, 32, 6);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// Cubemap normal generation
	glUseProgram(cubemapNormalShaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *cubemapNoiseTexture);
	glUniform1i(normal_NoiseSamplerLocation, 0);

	glBindImageTexture(0, *cubemapNormalTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glUniform1i(normal_NoiseSamplerLocation, 0);

	glDispatchCompute(32, 32, 6);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// Ending
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Noise::CreateTextures() {
	// Noise texture
	glGenTextures(1, cubemapNoiseTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *cubemapNoiseTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, cubemapResolution, cubemapResolution, 0, GL_RGBA, GL_FLOAT, nullptr);
	}

	// Normal map texture
	glGenTextures(1, cubemapNormalTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *cubemapNormalTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, cubemapResolution, cubemapResolution, 0, GL_RGBA, GL_FLOAT, nullptr);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Noise::CreateFramebuffers() {
	// Noise cubemap framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &fboCubemapNoise);
	glBindFramebuffer(GL_FRAMEBUFFER, fboCubemapNoise);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *cubemapNoiseTexture, 0);

	// Normal cubemap framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &fboCubemapNormal);
	glBindFramebuffer(GL_FRAMEBUFFER, fboCubemapNormal);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *cubemapNormalTexture, 0);

	// Bind defualt
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Destroy
Noise::~Noise() {
	glDeleteTextures(1, cubemapNoiseTexture);
	glDeleteTextures(1, cubemapNormalTexture);

	glDeleteFramebuffers(1, &fboCubemapNoise);
	glDeleteFramebuffers(1, &fboCubemapNormal);

	glDeleteProgram(cubemapNoiseShaderProgram);
	glDeleteProgram(cubemapNormalShaderProgram);
}