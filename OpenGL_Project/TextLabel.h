
	#pragma once

	// Dependency Includes
	#include <glm.hpp>
	#include <gtc\matrix_transform.hpp>
	#include <gtc\type_ptr.hpp>
	#include <ft2build.h>
	#include FT_FREETYPE_H  

	// Library Includes
	#include <map>
	#include <string>
	#include <iostream>

	// Local Includes
	#include "ShaderLoader.h"
	#include "Utility.h"

	

	class TextLabel 
	{
	private:
		struct FontChar
		{
			GLuint		TextureID = 0;	// Texture ID 
			glm::ivec2	Size;			// Size of the glyph
			glm::ivec2  Bearing;		// Offset of the glyph (top left) from the baseline
			GLuint		Advance = 0;	// How far to move for the next character
		};

	public:
		TextLabel(
			std::string Text, 
			std::string Font, 
			glm::ivec2 PixelSize,
			glm::vec2 Pos, 
			glm::vec3 Color		= glm::vec3(1.0f, 1.0f, 1.0f), 
			glm::vec2 Scale		= glm::vec2(1.0f, 1.0f));

		~TextLabel() {};

		void Render();
		void SetText(std::string Text) { this->TextStr = Text; };
		void SetColor(glm::vec3 Color) { this->Color = Color; };
		void SetScale(glm::vec2 Scale) { this->Scale = Scale; };
		void SetPosition(glm::vec2 Position) { this->Position = Position; };

	private:
		GLuint GenerateTexture(FT_Face face);

		static const int FontCharacterLimit = 128;
		bool Initialized = false;

		std::string TextStr;
		glm::vec2 Scale;
		glm::vec3 Color;
		glm::vec2 Position;
	
		GLuint VAO_Text;
		GLuint VBO_DynamicQuad;
		GLuint Program_Text;
		glm::mat4 ProjectionMat;
		std::map<GLchar, FontChar> CharacterMap;
	};

