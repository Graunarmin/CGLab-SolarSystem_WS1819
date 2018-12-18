#version 150

in vec2 tex_coords;

uniform sampler2D FBTexture;
uniform bool ShaderMode_grey;
uniform bool ShaderMode_verticalMirror;
uniform bool ShaderMode_horizontalMirror;
uniform bool ShaderMode_blur;

out vec4 out_color;

//7 = Greyscale
//8 = Horizontal Mirroring
//9 = Vertical Mirroring
//0 = Blur

//function to compute gaussian blur:
vec4 gaussian_blur(in vec2 texCoords){
    // Pixel Size:
    // pass_TexCoord = pixel_size * gl_FragCoord
    // That means: pixel_size = pass_TexCoord / gl_FragCoord
    vec2 pixel_size = texCoords / gl_FragCoord.xy;
    vec4 pixel_color[9];
    vec4 sum = vec4(0.0);

    // 3x3 matrix
    // |(-x, +y)/16| |(+0, +y)/8| |(+x, +y)/16|
    // |(-x, +0)/8 | |(+0, +0)/4| |(+x, +0)/8 |
    // |(-x, -y)/16| |(+0, -y)/8| |(+x, -y)/16|
    pixel_color[0] = texture(FBTexture, (vec2(texCoords.x - pixel_size.x, texCoords.y + pixel_size.y))) * 0.0625;
    pixel_color[1] = texture(FBTexture, (vec2(texCoords.x, texCoords.y + pixel_size.y))) * 0.125;
    pixel_color[2] = texture(FBTexture, (vec2(texCoords.x + pixel_size.x, texCoords.y + pixel_size.y))) * 0.0625;
    pixel_color[3] = texture(FBTexture, (vec2(texCoords.x - pixel_size.x, texCoords.y))) * 0.125;
    pixel_color[4] = texture(FBTexture, (vec2(texCoords.x, texCoords.y))) * 0.25;
    pixel_color[5] = texture(FBTexture, (vec2(texCoords.x + pixel_size.x, texCoords.y))) * 0.125;
    pixel_color[6] = texture(FBTexture, (vec2(texCoords.x - pixel_size.x, texCoords.y - pixel_size.y))) * 0.0625;
    pixel_color[7] = texture(FBTexture, (vec2(texCoords.x , texCoords.y - pixel_size.y))) * 0.125;
    pixel_color[8] = texture(FBTexture, (vec2(texCoords.x + pixel_size.x, texCoords.y - pixel_size.y))) * 0.0625;

    for(int i = 0; i <= 8; i++){
        sum += pixel_color[i];
    }

    return(sum);
}

void main(){
     
    vec2 texCoords = tex_coords;

    //Horizontal mirroring, flipped around x-axis 
	if(ShaderMode_horizontalMirror){
		texCoords.y = 1.0 - tex_coords.y;
	}

	//Vertical mirroring, flipped around y-axis
    if(ShaderMode_verticalMirror){
        texCoords.x = 1.0 - tex_coords.x;
    }
    
    //Default outcolor, if nothing else is applied
    out_color = texture(FBTexture, texCoords);

    //Gaussian Blur
    if(ShaderMode_blur){
        out_color = gaussian_blur(texCoords);
    }

     //Greyscale
    if(ShaderMode_grey){
        float luminance = (0.2126 * out_color.r + 0.7152* out_color.g + 0.0722* out_color.b);
        out_color = vec4(vec3(luminance), 1.0);
    }
}