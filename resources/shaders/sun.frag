#version 150
//runs for each pixel, determines the color
//"called" from vertex shader (-> pipeline)
in vec3 pass_Normal; //normale
in vec3 fragment_pos; //position of the fragment the color gets computed for
in vec3 camera_pos;
in vec2 tex_coords;

uniform bool ShaderMode_cell;
uniform sampler2D SunTexture;

//Cel Shading (There is no shade on the sun, so here we only need outlines)
float outline = 1;

vec2 texCoords = tex_coords;

//output
out vec4 out_Color;

//1 = Default (Reset)
//2 = Cell Shading

void main() {

    vec4 tex_color = texture(SunTexture, texCoords);

    //Celshading: draw outline
    if(ShaderMode_cell){
        // Normal 
        vec3 n = normalize(pass_Normal);
        //Vektor from pixel to camera
        vec3 v = normalize(fragment_pos - camera_pos);
        
        //Outline
        if(max(dot(n,v),0.0) < 0.35){ //the bigger the threshold the thicker the outline
            //make the pixel black if normal and view are almost perpendicular
            outline = 0;
        }
    }

    out_Color = vec4(tex_color.rgb * outline, 1.0);    
}
