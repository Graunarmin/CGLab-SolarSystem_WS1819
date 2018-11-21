#version 150
//in vec3 pass_orbitColor;
out vec4 out_orbitColor;

void main(){
    //RGB Color convert: x/255 and alpha stays 1.0
    out_orbitColor = vec4(0.0, 0.6, 0.6, 1.0);
}