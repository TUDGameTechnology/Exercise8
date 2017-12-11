#pragma once

#include "pch.h"

#include <Kore/Graphics4/Graphics.h>
#include <Kore/Graphics4/PipelineState.h>

using namespace Kore;

struct SceneParameters {
	// The view projection matrix aka the camera
	mat4 PV;
	mat4 V;
	
	// Add particle controller here
	
};

class ShaderProgram {
	public:
	ShaderProgram(const char* vsFile, const char* fsFile, Graphics4::VertexStructure& structure, bool depthWrite)
	{
		// Load and link the shaders
		FileReader vs("shader.vert");
		FileReader fs("shader.frag");
		vertexShader = new Graphics4::Shader(vs.readAll(), vs.size(), Graphics4::VertexShader);
		fragmentShader = new Graphics4::Shader(fs.readAll(), fs.size(), Graphics4::FragmentShader);
		
		pipeline = new Graphics4::PipelineState;
		pipeline->inputLayout[0] = &structure;
		pipeline->inputLayout[1] = nullptr;
		pipeline->vertexShader = vertexShader;
		pipeline->fragmentShader = fragmentShader;
		pipeline->depthMode = Graphics4::ZCompareLess;
		pipeline->depthWrite = depthWrite;
		pipeline->blendSource = Graphics4::BlendingOperation::SourceAlpha;
		pipeline->blendDestination = Graphics4::BlendingOperation::InverseSourceAlpha;
		pipeline->compile();
		
		pvLocation = pipeline->getConstantLocation("PV");
		mLocation = pipeline->getConstantLocation("M");
		
		// Add additional locations
	}
	
	// Update this program from the scene parameters
	virtual void Set(const SceneParameters& parameters, const mat4& M, Graphics4::Texture* image)
	{
		// Important: We need to set the program before we set a uniform
		Graphics4::setPipeline(pipeline);
		
		Graphics4::setTexture(tex, image);
		Graphics4::setMatrix(mLocation, M);
		Graphics4::setMatrix(pvLocation, parameters.PV);
		
		// Set Matrix or Vector for additional parameters
		
		Graphics4::setTextureAddressing(tex, Graphics4::U, Graphics4::TextureAddressing::Repeat);
		Graphics4::setTextureAddressing(tex, Graphics4::V, Graphics4::TextureAddressing::Repeat);
	}
	
protected:
	Graphics4::Shader* vertexShader;
	Graphics4::Shader* fragmentShader;
	Graphics4::PipelineState* pipeline;
	
	// Uniform locations - add more as you see fit
	Graphics4::TextureUnit tex;
	Graphics4::ConstantLocation pvLocation;
	Graphics4::ConstantLocation mLocation;
};
