#version 150
//runs for each pixel, determines the color
//"called" from vertex shader (-> pipeline)
in vec3 pass_Normal; //normale
in vec3 planet_color; //color
in vec3 light_pos; //position of pointlight
in vec3 fragment_pos; //position of the fragment the color gets computed for
in vec3 camera_pos;

//PointLight 
uniform vec3 LightColor;
uniform float LightIntensity;
uniform bool CelShading;

//Shades of light 
vec3 ambient_light = vec3(0.5, 0.5, 0.5) * planet_color;
vec3 diffuse_light = vec3(0.5, 0.5, 0.5) * planet_color;
vec3 specular_light = vec3(0.5, 0.5, 0.5) * planet_color;
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
out vec4 out_Color;s

void main() {

    // Normal 
    vec3 n = normalize(pass_Normal);

    //Vektor from pixel to pointlight
    vec3 l = pointlight_pos - fragment_pos;
    //Distance from pixel to pointlight (so the Lightintensity gets smaller the further the planet is away)
    float distance_ = length(l);
    //normalize l
    l = normalize(l);

    //Vektor from pixel to camera
    vec3 v = normalize(camera_pos-fragment_pos);

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
        float spec = pow(max(dot(h, n),0.0), 4*shininess);
        specular = spec * specular;
    }

    //Celshading: set intensity to one of the levels and draw outline
    if(CelShading){
        //no smoothnes, only 3 levels of brightness (defined in shades)
        float shade = floor(lambertian * shades);
        lambertian = shade / shades; 
        
        //Outline
        if(dot(n,v) < 0.35){ //the bigger the threshold the thicker the outline
            //make the pixel black if normal and view are almost perpendicular
            outline = 0;
        }
    }

    vec3 color = (ambient_light + (beta * lambertian * (diffuse + specular))) * outline;
    out_Color = vec4(color, 1.0);

}
