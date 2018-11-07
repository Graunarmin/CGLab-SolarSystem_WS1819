#version 150

//in vec3 pass_orbitColor;
out vec4 out_orbitColor;

void main(){
    // orbit is just white (maybe lightblue or sth later? But first it has to work)
    //out_orbitColor = vec4(1.0, 1.0, 1.0, 1.0);

    //now it's green, it's not so bright so that's good, but it still doesn't look really good
    out_orbitColor = vec4(0.2, 0.4, 0.3, 0.6);
}