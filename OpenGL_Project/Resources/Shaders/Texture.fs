
	#version 460 core
	
	// Vertex Shader Inputs
	in vec3 FragColor;
	in vec2 FragTexCoords;

	// Uniform Inputs
	uniform sampler2D ImageTexture;

	// Output
	out vec4 FinalColor;

	void main()
	{
		FinalColor = texture(ImageTexture, FragTexCoords);	// * vec4(FragColor, 1.0f);
	}


