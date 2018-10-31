#include "application_solar.hpp"
#include "window_handler.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include <iostream>

ApplicationSolar::ApplicationSolar(std::string const& resource_path):
    Application{resource_path},
    planet_object{},
    m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 20.0f})}, //Camera
    m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
    {
      initializeGeometry();
      initializeShaderPrograms();
    }

ApplicationSolar::~ApplicationSolar() {
    glDeleteBuffers(1, &planet_object.vertex_BO);
    glDeleteBuffers(1, &planet_object.element_BO);
    glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

void ApplicationSolar::render() const {
  // bind shader to upload uniforms
  glUseProgram(m_shaders.at("planet").handle);
  initializePlanets();

}

void ApplicationSolar::initializePlanets() const{

    model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

    Node root{nullptr, "root", "root", 0};
    auto root_p = std::make_shared<Node>(root);

    GeometryNode sun{root_p, "sun", "root/sun", 1};
    sun.setLocalTransform(glm::mat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    0.0f, 0.0f, 0.0f, 1.0f});
                                  // x     y      z
    sun.setSpeed(0.0f); 
    sun.setDistanceOrigin(0.0f);
    sun.setGeometry(planet_model);
    auto sun_p = std::make_shared<GeometryNode>(sun);

    GeometryNode mercury{sun_p, "mercury", "root/sun/mercury", 2};
    mercury.setLocalTransform(glm::mat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    0.0f, 0.0f, 0.0f,2.5f});
                                  // x     y      z
    mercury.setSpeed(0.7f); 
    mercury.setDistanceOrigin(5.0f);
    mercury.setGeometry(planet_model);

    GeometryNode venus{sun_p, "venus", "root/sun/venus", 2};
    venus.setLocalTransform(glm::mat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    0.0f, 0.0f, 0.0f, 2.4f});
                                  // x     y      z
    venus.setSpeed(0.6f); 
    venus.setDistanceOrigin(8.0f);
    venus.setGeometry(planet_model);

    GeometryNode earth{sun_p, "earth", "root/sun/earth", 2};
    earth.setLocalTransform(glm::mat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    0.0f, 0.0f, 0.0f, 2.3f});
                                  // x     y      z
    earth.setSpeed(0.5f); 
    earth.setDistanceOrigin(11.0f);
    earth.setGeometry(planet_model);
    auto earth_p = std::make_shared<GeometryNode>(earth);

    GeometryNode moon{earth_p, "moon", "root/sun/earth/moon", 3};
    moon.setLocalTransform(glm::mat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    11.0f, 6.0f, 0.0f, 3.5f});
                                  // x     y      z
    moon.setSpeed(0.5f); 
    moon.setDistanceOrigin(11.0f);
    moon.setGeometry(planet_model);

    GeometryNode mars{sun_p, "mars", "root/sun/mars", 2};
    mars.setLocalTransform(glm::mat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    0.0f, 0.0f, 0.0f, 2.5f});
                                  // x     y      z
    mars.setSpeed(0.65f); 
    mars.setDistanceOrigin(15.0f);
    mars.setGeometry(planet_model);

    GeometryNode jupiter{sun_p, "jupiter", "root/sun/jupiter", 2};
    jupiter.setLocalTransform(glm::mat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    0.0f, 0.0f, 0.0f, 1.8f});
                                  // x     y      z
    jupiter.setSpeed(0.7f); 
    jupiter.setDistanceOrigin(14.0f);
    jupiter.setGeometry(planet_model);

    GeometryNode saturn{sun_p, "saturn", "root/sun/saturn", 2};
    saturn.setLocalTransform(glm::mat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    0.0f, 0.0f, 0.0f, 1.9f});
                                  // x     y      z
    saturn.setSpeed(0.75f); 
    saturn.setDistanceOrigin(17.0f);
    saturn.setGeometry(planet_model);

    GeometryNode uranus{sun_p, "uranus", "root/sun/uranus", 2};
    uranus.setLocalTransform(glm::mat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    0.0f, 0.0f, 0.0f, 2.0f});
                                  // x     y      z
    uranus.setSpeed(0.8f); 
    uranus.setDistanceOrigin(21.0f);
    uranus.setGeometry(planet_model);

    GeometryNode neptun{sun_p, "neptun", "root/sun/neptun", 2};
    neptun.setLocalTransform(glm::mat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    0.0f, 0.0f, 0.0f, 2.1f});
                                  // x     y      z
    neptun.setSpeed(0.5f); 
    neptun.setDistanceOrigin(25.0f);
    neptun.setGeometry(planet_model);

    root.addChildren(sun_p);
    (*sun_p).addChildren(std::make_shared<GeometryNode>(mercury));
    (*sun_p).addChildren(std::make_shared<GeometryNode>(venus));
    (*sun_p).addChildren(earth_p);
    (*sun_p).addChildren(std::make_shared<GeometryNode>(mars));
    (*sun_p).addChildren(std::make_shared<GeometryNode>(jupiter));
    (*sun_p).addChildren(std::make_shared<GeometryNode>(saturn));
    (*sun_p).addChildren(std::make_shared<GeometryNode>(uranus));
    (*sun_p).addChildren(std::make_shared<GeometryNode>(neptun));
    (*earth_p).addChildren(std::make_shared<GeometryNode>(moon));
    
    SceneGraph sceneGraph{"solarsystem", root};
    auto solarSystem = sceneGraph.getRoot().getChildrenList();
    planetTransformations(solarSystem);
  }


void ApplicationSolar::planetTransformations(std::list<std::shared_ptr<Node>> const& childrenList) const{

    for(auto const& planet: childrenList){
        auto childPlanets = (*planet).getChildrenList();
        //recursive traversal through SceneGraph 
        if(childPlanets.size() > 0){   
            planetTransformations(childPlanets);
        }

        //compute tranformations
        // glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime())*((*planet).getSpeed()), glm::fvec3{0.0f, 1.0f, 0.0f});
        // model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f+((*planet).getDistanceOrigin()), 0.0f, 1.0f});

        glm::fmat4 model_matrix = glm::rotate((*planet).getLocalTransform(), float(glfwGetTime())*((*planet).getSpeed()), glm::fvec3{0.0f, 1.0f, 0.0f});
        model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f+((*planet).getDistanceOrigin()), 0.0f, 1.0f});

        glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform)* model_matrix);


        //give matrices to shaders
        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                     1, GL_FALSE, glm::value_ptr(model_matrix));


        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                     1, GL_FALSE, glm::value_ptr(normal_matrix));

        // bind the VAO to draw
        glBindVertexArray(planet_object.vertex_AO);

        // draw bound vertex array using bound shader
        glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

        //std::cout << *planet;
    }
}

void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection() {
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 
  // bind shader to which to upload unforms
  glUseProgram(m_shaders.at("planet").handle);
  // upload uniform values to new locations
  uploadView();
  uploadProjection();
}

///////////////////////////// intialisation functions /////////////////////////
// load shader sources
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container m_shader
  m_shaders.emplace("planet", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/simple.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple.frag"}}});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  // generate vertex array object
  glGenVertexArrays(1, &planet_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(planet_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &planet_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
  // configure currently bound array buffer
  //Buffer mit Vertexdaten werden "Verknüpft" so dass der Renderer weiß, wo sie liegen
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

   // generate generic buffer
  glGenBuffers(1, &planet_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  planet_object.num_elements = GLsizei(planet_model.indices.size());
}

///////////////////////////// callback functions for window events ////////////
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {

    //zoom in
    if (key == GLFW_KEY_I  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.3f});
        uploadView();
    }
    //zoom out
    else if (key == GLFW_KEY_O  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.3f});
        uploadView();
    }
    //move to the left
    else if(key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.3f, 0.0f, 0.0f});
        uploadView();
    }
    //move to the right
    else if(key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.3f, 0.0f, 0.0f});
        uploadView();
    }
    //move up
    else if(key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -0.3f, 0.0f});
        uploadView();
    }
    //move down
    else if(key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.3f, 0.0f});
        uploadView();
    }
  
  
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
    // mouse handling

    //std::cout << pos_x << ", "<< pos_y << std::endl;

    //shifting left, right, up and down by moving the mouse in the respective direction
    if (pos_x > 0){
        m_view_transform = glm::rotate(m_view_transform, 0.005f,glm::fvec3{0.0f, 1.0f, 0.0f});
    }
    else if(pos_x < 0){
        m_view_transform = glm::rotate(m_view_transform, 0.005f,glm::fvec3{0.0f, -1.0f, 0.0f});
    }
    if(pos_y > 0){
        m_view_transform = glm::rotate(m_view_transform, 0.005f,glm::fvec3{1.0f, 0.0f, 0.0f});
    } 
    else if(pos_y < 0){
        m_view_transform = glm::rotate(m_view_transform, 0.005f,glm::fvec3{-1.0f, 0.0f, 0.0f});
    }
    uploadView();

    // //zooming in by movin up
    // if ((pos_x > 0 && pos_y < 0) || (pos_x < 0 && pos_y < 0)) {
    //     m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.3f});
    // }
    //  // zooming out by moving down
    // else if ((pos_x  < 0 && pos_y > 0) || (pos_x > 0 && pos_y > 0)){
    //     m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.3f});
    // }
    // uploadView();

}

//handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height) {
  // recalculate projection matrix for new aspect ration
  m_view_projection = utils::calculate_projection_matrix(float(width) / float(height));
  // upload new projection matrix
  uploadProjection();
}


// exe entry point
int main(int argc, char* argv[]) {
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}