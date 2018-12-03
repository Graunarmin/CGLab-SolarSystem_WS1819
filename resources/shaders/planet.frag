#version 150

//runs for each pixel, determines the color
//"called" from vertex shader (-> pipeline)
in vec3 pass_Normal; //normale
in vec3 planet_color; //color
in vec3 light_pos; //position of pointlight
in vec3 fragment_pos; //position of the fragment the color gets computed for
in vec3 camera_pos;
in vec2 tex_coords;

//PointLight 
uniform vec3 LightColor;
uniform float LightIntensity;
uniform int ShaderMode;
uniform sampler2D PlanetTexture;
uniform sampler2D NormalTexture;
uniform bool HasNormalMap;

vec4 tex_color = texture(PlanetTexture, tex_coords);

//Shades of light 
vec3 ambient_light = vec3(0.7); //* tex_color.rgb; // * planet_color;
vec3 diffuse_light = vec3(0.8); //* tex_color.rgb; // * planet_color;
vec3 specular_light = vec3(0.5); //* tex_color.xyz; // * planet_color;
vec3 pointlight_pos = vec3(0.0);

//Define reflectivity of Planets, defines how much light it reflects 
float reflectivity = 18.0; //rho (slide 7)
//Shininess = Intensity of the highlight
float shininess = 24.0; //alpha (slide 8)

vec3 specular = specular_light;

//Cel Shading
float shades = 3; //change for more "shading levels"
float outline = 1;

//output
out vec4 out_Color;

// https://learnopengl.com/Lighting/Basic-Lighting
// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting

void main() {

    // Normal 
    vec3 n = normalize(pass_Normal);

    //normal mapping
    if(ShaderMode == 3){
        if(HasNormalMap){
          
            vec3 q0 = dFdx(fragment_pos.xyz); //horizontal tangente (partial derivative)
            vec3 q1 = dFdy(fragment_pos.xyz); //vertical tangente

            //representing the tangent space of the texture:
            vec2 st0 = dFdx(tex_coords.st); //horizontal "line" from pixel to pixel
            vec2 st1 = dFdy(tex_coords.st); //vertical "line" from pixel to pixel

            vec3 S = normalize(q0 * st1.t - q1 * st0.t); //horizontal
            vec3 T = normalize(-q0 * st1.s + q1 * st0.s); //vertical
            vec3 N = normalize(pass_Normal); //normal

            vec3 mapN = texture2D(NormalTexture, tex_coords).xyz * 2.0 -1.0; // "shift" from [0, -1] to [-1, 1]
            //mapN.xy = 0.5 * mapN.xy;
            mat3 tsn = mat3 (S, T, N); //new tangent space 

            //re-define normal
            n = normalize (tsn * mapN); // new tangent space mmultiplied by the "Texture Matrix"
        }
    }

    //Vektor from pixel to pointlight
    vec3 l = pointlight_pos - fragment_pos;
    //Distance from pixel to pointlight (so the Lightintensity gets smaller the further the planet is away)
    float distance_ = length(l);
    //normalize l
    l = normalize(l);

    //Vektor from pixel to camera
    vec3 v = normalize(fragment_pos - camera_pos);

    //Angle bisector (direction of reflection, halfwayvector)
    vec3 h = normalize(l + v);

    //Lambertian BRDF
    //Brightness / Intensity and to determine if it's front or back
    float lambertian = max(dot(l,n), 0.0);
    //Intensity of the light that reaches the processed fragment (Slide 6)
    vec3 beta = (LightColor * LightIntensity) / ((/*4 */ 3.14159265359)* pow(distance_,2));

    //compute diffuse component
    float diff = max(dot(l, n), 0.0) * (reflectivity / 3.14159265359);
    vec3 diffuse = diff * diffuse_light;

    //So the light only hits the front of the planet
    if(lambertian > 0.0){
        float spec = pow(max(dot(h, v),0.0), 4*shininess);
        specular = spec * specular;
    }

    //Celshading: set intensity to one of the levels and draw outline
    if(ShaderMode == 2){
        //no smoothnes, only 3 levels of brightness (defined in shades)
        float shade = floor(lambertian * shades);
        lambertian = shade / shades; 
        
        //Outline
        if(max(dot(n,v),0.0) < 0.35){ //the bigger the threshold the thicker the outline
            //make the pixel black if normal and view are almost perpendicular
            outline = 0;
        }
    }

    //Greyscale
    if(ShaderMode == 4){
        float grey = (tex_color.r + tex_color.g + tex_color.b) / 3;
        tex_color = vec4(grey, grey, grey, 1.0);
    }

    vec3 color = (ambient_light + (beta * lambertian * (diffuse + specular))) * outline;
    out_Color = vec4(color, 1.0) * tex_color; //* vec4(planet_color, 1.0);

}
