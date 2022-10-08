	#include "TextLabel.h"

	TextLabel::TextLabel(std::string Text, std::string Font, glm::ivec2 PixelSize, glm::vec2 Pos, glm::vec3 Color, glm::vec2 Scale)
	{
		SetText(Text);
		SetColor(Color);
		SetScale(Scale);
		SetPosition(Pos);
	
		// Edit to work with your code
		ProjectionMat = glm::ortho(0.0f, (GLfloat)Utils::WindowWidth, 0.0f, (GLfloat)Utils::WindowHeight, 0.0f, 100.0f);
		Program_Text = ShaderLoader::CreateProgram("Resources/Shaders/Text.vs", "Resources/Shaders/Text.fs");
		// End Edit
	
		FT_Library FontLibrary;	// Font Library Object to load the font
		FT_Face FontFace;	// Each character is called a glyph and Face is the collection of glyphs

		// Initiate the font library object
		if (FT_Init_FreeType(&FontLibrary) != 0)
		{
			std::cout << "FreeType Error: Could not init FreeType Library" << std::endl;
			return; // Initialized variable stays false;
		}

		// Load the font as face
		if (FT_New_Face(FontLibrary, Font.c_str(), 0, &FontFace) != 0)
		{
			std::cout << "Freetype Error: Failed to Load font: " << Font << std::endl;
			return;	// Initialized variable stays false;
		}

		FT_Set_Pixel_Sizes(FontFace, PixelSize.x, PixelSize.y);		// Set size to load glyph as	
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);						// Disable byte-alignment restriction

		// Load the characters of the font up to the FontCharacterLimit
		for (GLubyte Glyph = 0; Glyph < FontCharacterLimit; Glyph++)
		{
			// Load the character glyph into face
			if (FT_Load_Char(FontFace, Glyph, FT_LOAD_RENDER))
			{
				std::cout << "FreeType Error: Failed to load Glyph: " << (unsigned char)Glyph << std::endl;
				continue;
			}

			// Now to store character for later use
			FontChar FontCharacter = { 
				GenerateTexture(FontFace), // Generate a texture for each char (glyph)
				glm::ivec2(FontFace->glyph->bitmap.width, FontFace->glyph->bitmap.rows),
				glm::ivec2(FontFace->glyph->bitmap_left, FontFace->glyph->bitmap_top),
				(GLuint)FontFace->glyph->advance.x / 64 // Advance is no. of 1/64 pixels -> Convert to pixels
			};
			CharacterMap.insert(std::pair<GLchar, FontChar>(Glyph, FontCharacter));
		}

		// Destroy FreeType objects once we are finished
		FT_Done_Face(FontFace);
		FT_Done_FreeType(FontLibrary);

		//Configure VAO/VBO for texture quads
		glGenVertexArrays(1, &VAO_Text);
		glBindVertexArray(VAO_Text);

		glGenBuffers(1, &VBO_DynamicQuad);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_DynamicQuad);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Set the state of Initialized to allow future calls
		Initialized = true;
	}

	void TextLabel::Render()
	{
		if (Initialized == false)
		{
			return;	// Prevent calls to the TextLabel if it failed to initialize
		}

		// Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Activate corresponding render state
		glUseProgram(Program_Text);
		glUniform3fv(glGetUniformLocation(Program_Text, "TextColor"), 1, glm::value_ptr(Color));
		glUniformMatrix4fv(glGetUniformLocation(Program_Text, "ProjectionMat"), 1, GL_FALSE, glm::value_ptr(ProjectionMat));
		glBindVertexArray(VAO_Text);

		// Store Character Origin that can change as we write each character. Keep the starting text position intact.
		glm::vec2 CharacterOrigin = Position;

		// Iterate through the texts characters
		for (std::string::const_iterator TextCharacter = TextStr.begin(); TextCharacter != TextStr.end(); TextCharacter++)
		{
			FontChar FontCharacter = CharacterMap[*TextCharacter];
			GLfloat PosX = CharacterOrigin.x + FontCharacter.Bearing.x * Scale.x;
			GLfloat PosY = CharacterOrigin.y - (FontCharacter.Size.y - FontCharacter.Bearing.y) * Scale.y;
			GLfloat Width = FontCharacter.Size.x * Scale.x;
			GLfloat Height = FontCharacter.Size.y * Scale.y;

			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{ PosX, PosY + Height, 0.0, 0.0 }, { PosX, PosY, 0.0, 1.0 }, { PosX + Width, PosY, 1.0, 1.0 },
				{ PosX, PosY + Height, 0.0, 0.0 }, { PosX + Width, PosY, 1.0, 1.0 },{ PosX + Width, PosY + Height, 1.0, 0.0 },
			};
			// Reload the vertex array to the VBO
			glBindBuffer(GL_ARRAY_BUFFER, VBO_DynamicQuad);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

			// Render the glyph texture over the quad
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FontCharacter.TextureID);		
			glUniform1i(glGetUniformLocation(Program_Text, "TextTexture"), 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			// Move the position of the origin point for the next glyph by the advance 
			CharacterOrigin.x += FontCharacter.Advance * Scale.x; 
		}

		glUseProgram(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisable(GL_BLEND);
	}

	GLuint TextLabel::GenerateTexture(FT_Face FontFace)
	{
		GLuint TextureID;
		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D, TextureID);

		glTexImage2D(	GL_TEXTURE_2D, 
						0, 
						GL_RED, 
						FontFace->glyph->bitmap.width, 
						FontFace->glyph->bitmap.rows, 
						0, 
						GL_RED, 
						GL_UNSIGNED_BYTE, 
						FontFace->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		return TextureID;
	}

