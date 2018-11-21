#version 150
//runs for each pixel, determines the color
//from vertex shader
in vec3 pass_Normal; //normale
in vec3 planet_color; //color
in vec3 light_pos; //position of pointlight
in vec3 fragment_pos; //position of the fragment the color gets computed for
in vec3 camera_pos;

//PointLight 
uniform vec3 LightColor;
uniform float LightIntensity;

//Shades of light 
vec3 ambient_light = vec3(0.5, 0.5, 0.5) * planet_color;
vec3 diffuse_light = vec3(0.5, 0.5, 0.5) * planet_color;
vec3 specular_light = vec3(1.0, 1.0, 1.0) * planet_color;
vec3 pointlight_pos = vec3(0.0, 0.0, 0.0);

//Define reflectivity of Planets, defines how much light it reflects 
float reflectivity = 5.0;

float specular = 0.0;

out vec4 out_Color;

void main() {

    // Normal 
    vec3 n = normalize(pass_Normal);

    //Vektor from pixel to pointlight
    vec3 l = pointlight_pos - fragment_pos;

    //Distance from pixel to pointlight, to minimize the LightIntensity
    float distance = length(l);
    //distance = pow(distance,2);

    //l normalized 
    l = normalize(l);
    
    //Lambertian BRDF
    float lambertian = max(dot(l,n), 0.0);

    //So that the light only hits the front of the planet
    if(lambertian > 0.0){

        //Vektor from pixel to camera
        vec3 v = normalize(camera_pos - fragment_pos);

        //Angle bisector 
        vec3 h = normalize(v + l);

        //Angle between normal and halfwayvector (halfwayangle)
        float angle = max(dot(h,n), 0.0);

        specular = pow(angle,reflectivity);

    }

    vec3 colorLinear = ambient_light + lambertian * diffuse_light * ((LightColor * LightIntensity)/(pow(distance,2))) + specular_light * specular * ((LightColor * LightIntensity)/(pow(distance,2)));

    out_Color = vec4(colorLinear, 1.0);

}
