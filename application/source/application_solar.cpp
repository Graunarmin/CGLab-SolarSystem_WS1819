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
#include <glm/glm.hpp>

#include <iostream>

// Constructor
ApplicationSolar::ApplicationSolar(std::string const& resource_path):
    Application{resource_path},
    planet_object{},
    star_object{},
    orbit_object{},
    sceneGraph_{},
    stars_{},
    orbits_{},
    m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 50.0f})}, //Camera
    m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
    {
        initializeGeometry();
        initializePlanets();
        initializeStars();
        initializeOrbits();
        initializeShaderPrograms();
       
    }

// Destructor
ApplicationSolar::~ApplicationSolar() {
    glDeleteBuffers(1, &planet_object.vertex_BO);
    glDeleteBuffers(1, &planet_object.element_BO);
    glDeleteVertexArrays(1, &planet_object.vertex_AO);

    glDeleteBuffers(1, &orbit_object.vertex_BO);
    glDeleteVertexArrays(1, &orbit_object.vertex_AO);

    glDeleteBuffers(1, &star_object.vertex_BO);
    glDeleteVertexArrays(1, &star_object.vertex_AO);
}

// ---------------------- MAIN ------------------------
void ApplicationSolar::render() const {

    // ------ render planets and orbits ------
    // get the Scenegraph and start recursive traversal
    auto solarSystem = sceneGraph_.getRoot().getChildrenList();
    planetTransformations(solarSystem);

    // ------ render stars ------
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("star").handle);
    // bind the VAO to draw
    glBindVertexArray(star_object.vertex_AO);
    // draw bound vertex array using bound shader
    glDrawArrays(star_object.draw_mode, 0, star_object.num_elements);

}

// ------------------ intialisation functions -------------------

// create planets
void ApplicationSolar::initializePlanets(){

    //Load model
    model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

    //Initialize root: parent=nullptr, name=root, path=root, depth=0
    Node root{nullptr, "root", "root", 0};
    auto root_p = std::make_shared<Node>(root);

    //Initialize sun: parent=pointer to root, name=sun, path=root/sun, depth=1
    GeometryNode sun{root_p, "sun", "root/sun", 1};
    //Set speed of movement
    sun.setSpeed(0.0f);
    // set speed for selfrotation
    sun.setSelfRotation(0.5f);
    //Set distance to the origin
    sun.setDistanceOrigin(glm::fvec3{0.0f, 0.0f, 0.0f});
    // set radius
    sun.setRadius(2.0f);
    //Set geometry
    sun.setGeometry(planet_model);
    //Transformationmatrix 
    sun.setLocalTransform(glm::fmat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    0.0f, 0.0f, 0.0f, 1.0f});
                                // x(l/r), y(u/d), z(b/f), size
    auto sun_p = std::make_shared<GeometryNode>(sun);


    //---------------------------------- Initialize Planets ---------------------------------
    //Set size (in LocalTransform Matrix), speed and distance to origin

    GeometryNode mercury{sun_p, "mercury", "root/sun/mercury", 2};
    mercury.setSpeed(0.2f);
    mercury.setSelfRotation(0.6f); 
    mercury.setDistanceOrigin(glm::fvec3{8.0f, 0.0f, 0.0f});
    mercury.setRadius(0.5f);
    mercury.setGeometry(planet_model);
    mercury.setLocalTransform(glm::fmat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                        0.0f, 1.0f, 0.0f, 0.0f,
                                        0.0f, 0.0f, 1.0f, 0.0f, 
                                        0.0f, 0.0f, 0.0f, 1.0f});
                                      // x     y      z

    GeometryNode venus{sun_p, "venus", "root/sun/venus", 2};
    venus.setSpeed(0.15f);
    venus.setSelfRotation(0.5f); 
    venus.setDistanceOrigin(glm::fvec3{11.0f, 0.0f, 0.0f});
    venus.setRadius(0.6f);
    venus.setGeometry(planet_model);
    venus.setLocalTransform(glm::fmat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                      0.0f, 1.0f, 0.0f, 0.0f,
                                      0.0f, 0.0f, 1.0f, 0.0f, 
                                      0.0f, 0.0f, 0.0f, 1.0f});

    GeometryNode earth{sun_p, "earth", "root/sun/earth", 2};
    earth.setSpeed(0.1f);
    earth.setSelfRotation(0.7f);
    earth.setDistanceOrigin(glm::fvec3{14.0f, 0.0f, 0.0f});
    earth.setRadius(0.6f);
    earth.setGeometry(planet_model);
    earth.setLocalTransform(glm::fmat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                      0.0f, 1.0f, 0.0f, 0.0f,
                                      0.0f, 0.0f, 1.0f, 0.0f, 
                                      0.0f, 0.0f, 0.0f, 1.0f});
    auto earth_p = std::make_shared<GeometryNode>(earth);

    //moon is a child from earth, so it's parent is a pointer to earth and the depth is 3
    GeometryNode moon{earth_p, "moon", "root/sun/earth/moon", 3};
    moon.setSpeed(0.5f); 
    moon.setSelfRotation(0.7f);
    moon.setDistanceOrigin(glm::fvec3{1.5f, 0.0f, 0.0f}); //in this case: Distance to earth!
    moon.setRadius(0.2f);
    moon.setGeometry(planet_model);
    moon.setLocalTransform(glm::fmat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    0.0f, 0.0f, 0.0f, 1.0f});
                                // x(l/r), y(u/d), z(b/f), size

    GeometryNode mars{sun_p, "mars", "root/sun/mars", 2};
    mars.setSpeed(0.2f); 
    mars.setSelfRotation(0.5f);
    mars.setDistanceOrigin(glm::fvec3{20.0f, 0.0f, 0.0f});
    mars.setRadius(0.7f);
    mars.setGeometry(planet_model);
    mars.setLocalTransform(glm::fmat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    0.0f, 0.0f, 0.0f, 1.0f});

    GeometryNode jupiter{sun_p, "jupiter", "root/sun/jupiter", 2};
    jupiter.setSpeed(0.15f); 
    jupiter.setSelfRotation(0.6f);
    jupiter.setDistanceOrigin(glm::fvec3{25.0f, 0.0f, 0.0f});
    jupiter.setRadius(1.2f);
    jupiter.setGeometry(planet_model);
    jupiter.setLocalTransform(glm::fmat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                        0.0f, 1.0f, 0.0f, 0.0f,
                                        0.0f, 0.0f, 1.0f, 0.0f, 
                                        0.0f, 0.0f, 0.0f, 1.0f});
    auto jupiter_p = std::make_shared<GeometryNode>(jupiter);

    GeometryNode jupiter_moon1{jupiter_p, "jupiterMoon1", "root/sun/earth/jupiterMoon1", 3};
    jupiter_moon1.setSpeed(0.5f); 
    jupiter_moon1.setSelfRotation(0.4f); 
    jupiter_moon1.setDistanceOrigin(glm::fvec3{2.0f, 0.0f, 0.0f}); //in this case: Distance to jupiter!
    jupiter_moon1.setRadius(0.1f); 
    jupiter_moon1.setGeometry(planet_model);
    jupiter_moon1.setLocalTransform(glm::fmat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    0.0f, 0.0f, 0.0f, 1.0f});

    GeometryNode jupiter_moon2{jupiter_p, "jupiterMoon2", "root/sun/earth/jupiterMoon2", 3};
    jupiter_moon2.setSpeed(0.4f);
    jupiter_moon2.setSelfRotation(0.4f);
    jupiter_moon2.setDistanceOrigin(glm::fvec3{2.6f, 0.0f, 0.0f}); //in this case: Distance to jupiter!
    jupiter_moon2.setRadius(0.2f);
    jupiter_moon2.setGeometry(planet_model);
    jupiter_moon2.setLocalTransform(glm::fmat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 
                                    0.0f, 0.0f, 0.0f, 1.0f});


    GeometryNode saturn{sun_p, "saturn", "root/sun/saturn", 2};
    saturn.setSpeed(0.15f); 
    saturn.setSelfRotation(0.6f);
    saturn.setDistanceOrigin(glm::fvec3{33.0f, 0.0f, 0.0f});
    saturn.setRadius(1.0f);
    saturn.setGeometry(planet_model);
    saturn.setLocalTransform(glm::fmat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                       0.0f, 1.0f, 0.0f, 0.0f,
                                       0.0f, 0.0f, 1.0f, 0.0f, 
                                       0.0f, 0.0f, 0.0f, 1.0f});

    GeometryNode uranus{sun_p, "uranus", "root/sun/uranus", 2};
    uranus.setSpeed(0.25f); 
    uranus.setSelfRotation(0.8f);
    uranus.setDistanceOrigin(glm::fvec3{36.0f, 0.0f, 0.0f});
    uranus.setRadius(1.0f);
    uranus.setGeometry(planet_model);
    uranus.setLocalTransform(glm::fmat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                       0.0f, 1.0f, 0.0f, 0.0f,
                                       0.0f, 0.0f, 1.0f, 0.0f, 
                                       0.0f, 0.0f, 0.0f, 1.0f});

    GeometryNode neptun{sun_p, "neptun", "root/sun/neptun", 2};
    neptun.setSpeed(0.2f);
    neptun.setSelfRotation(0.5f);
    neptun.setDistanceOrigin(glm::fvec3{39.0f, 0.0f, 0.0f});
    neptun.setRadius(0.7f);
    neptun.setGeometry(planet_model);
    neptun.setLocalTransform(glm::fmat4{1.0f, 0.0f, 0.0f, 0.0f, 
                                       0.0f, 1.0f, 0.0f, 0.0f,
                                       0.0f, 0.0f, 1.0f, 0.0f, 
                                       0.0f, 0.0f, 0.0f, 1.0f});

    //---------------------- Add planets to their parents ---------------------

    root.addChildren(sun_p);
    (*sun_p).addChildren(std::make_shared<GeometryNode>(mercury));
    (*sun_p).addChildren(std::make_shared<GeometryNode>(venus));
    (*sun_p).addChildren(earth_p);
    (*sun_p).addChildren(std::make_shared<GeometryNode>(mars));
    (*sun_p).addChildren(jupiter_p);
    (*sun_p).addChildren(std::make_shared<GeometryNode>(saturn));
    (*sun_p).addChildren(std::make_shared<GeometryNode>(uranus));
    (*sun_p).addChildren(std::make_shared<GeometryNode>(neptun));
    (*earth_p).addChildren(std::make_shared<GeometryNode>(moon));
    (*jupiter_p).addChildren(std::make_shared<GeometryNode>(jupiter_moon1));
    (*jupiter_p).addChildren(std::make_shared<GeometryNode>(jupiter_moon2));

    //------------------------ Initialize SceneGraph -------------------------
    
    sceneGraph_.setName("solarsystem");
    sceneGraph_.setRoot(root);
}

// transform planets
void ApplicationSolar::planetTransformations(std::list<std::shared_ptr<Node>> const& childrenList) const{

    // recursive traversal through SceneGraph
    // visit each child in children_ and check whether they themselves have children which have to be visited
    for(auto const& planet: childrenList){

        auto childPlanets = planet->getChildrenList();

        if(childPlanets.size() > 0){   
            planetTransformations(childPlanets); //recursive call
        }

        //draw orbit for each planet
        drawOrbit(planet);

        //compute tranformations for each planet
        glm::fmat4 model_matrix = glm::fmat4{};
    
        if(planet->getDepth() == 3){
            //moons need extra rotation around their parent planet so we need to shift them to the parent first before
            //adding it's own rotation
            glm::fmat4 parent_matrix = planet->getParent()->getLocalTransform();
            model_matrix = glm::rotate(parent_matrix, float(glfwGetTime())*(planet->getParent()->getSpeed()), glm::fvec3{0.0f, 1.0f, 0.0f});
            model_matrix = glm::translate(model_matrix, -1.0 * planet->getParent()->getDistanceOrigin());
        }
        if(planet->getDepth() == 1){
            //selfrotation works for sun
            model_matrix = glm::rotate(model_matrix * planet->getLocalTransform(), float(glfwGetTime())* (planet->getSelfRotation()),glm::fvec3{0.0f, 1.0f, 0.0f});
            model_matrix = glm::translate(model_matrix, {0.0f, 0.0f, -1.0f});
        }
        // selfrotation (does not work in combination with the other rotation yet)
        // model_matrix = glm::rotate(model_matrix * planet->getLocalTransform(), float(glfwGetTime())* (planet->getSelfRotation()),glm::fvec3{0.0f, 1.0f, 0.0f});
        // model_matrix = glm::translate(model_matrix, {0.0f, 0.0f, -1.0f});
    
        //rotation around parent
        model_matrix = glm::rotate(model_matrix* planet->getLocalTransform(),float(glfwGetTime())*(planet->getSpeed()),glm::fvec3{0.0f, 1.0f, 0.0f});
        model_matrix = glm::translate(model_matrix, -1.0f * planet->getDistanceOrigin());

        //scale planet
        float radius = planet->getRadius();
        model_matrix = glm::scale(model_matrix, glm::fvec3{radius, radius, radius});
        
        //store LocalTransform in Object gives a REALLY bad result^^
        //planet->setLocalTransform(model_matrix);

        //extra matrix for normal transformation to keep them orthogonal to surface
        glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform)* model_matrix);

        // bind shader to upload uniforms
        glUseProgram(m_shaders.at("planet").handle);
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

//create stars
void ApplicationSolar::initializeStars() {

    // 3000 random stars
    for(int i = 0; i < 3000; ++i){ //6 floats for each star (x,y,z (Position) and r,g,b (Color))
        // %100 so they stay between 0 and 99,
        // the more you subtract the more stars appear, sth. between 40 and 60 looks good
        GLfloat x = (rand() % 100) - 40; 
        stars_.push_back(x);
        GLfloat y = (rand() % 100) - 40; 
        stars_.push_back(y);
        GLfloat z = (rand() % 100) - 40; 
        stars_.push_back(z);

        //(rand() % (max-min + 1) + min) / (max-min + 1)  for the Colors so the stay between 0.9 an 1.0
        GLfloat r = ((rand() % 11) + 90) / 11; 
        stars_.push_back(r);
        GLfloat g = ((rand() % 11) + 90) / 11;
        stars_.push_back(g);
        GLfloat b = ((rand() % 11) + 90) / 11; 
        stars_.push_back(b);
    }

    // generate vertex array object
    glGenVertexArrays(1, &star_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(star_object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &star_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    //https://wiki.delphigl.com/index.php/glBindBuffer
    glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
    // configure currently bound array buffer
    //Buffer mit Vertexdaten werden "Verknüpft" so dass der Renderer weiß, wo sie liegen
    //https://wiki.delphigl.com/index.php/glBufferData
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(sizeof(float) * stars_.size()), stars_.data(), GL_STATIC_DRAW);

    // https://wiki.delphigl.com/index.php/glVertexAttribPointer
    // activate first attribute on gpu (Position, 3 floats)
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with offset to 0 (start of Position)
    // each star consists of 6 floats, offset (pointer to first component) is 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
    // activate second attribute on gpu (Normale, 3 floats)
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with offset to float at Index 3 (start of Normale), type is void pointer
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) (sizeof(float) * 3));

    // store type of primitive to draw
    star_object.draw_mode = GL_POINTS;
    // transfer number of indices to model object 
    star_object.num_elements = GLsizei(stars_.size() / 6); //divide by 6 because each star consists of 6 floats
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

void ApplicationSolar::initializeOrbits(){
    
    // create circle of 360 points
    for(int i = 0; i < 360; ++i){
        GLfloat x = cos(i * M_PI / 180);
        orbits_.push_back(x);
        GLfloat y = 0;
        orbits_.push_back(y);
        GLfloat z = sin(i * M_PI / 180);
        orbits_.push_back(z);
    }

    // generate vertex array object
    glGenVertexArrays(1, &orbit_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(orbit_object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &orbit_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    //https://wiki.delphigl.com/index.php/glBindBuffer
    glBindBuffer(GL_ARRAY_BUFFER, orbit_object.vertex_BO);
    // configure currently bound array buffer
    //https://wiki.delphigl.com/index.php/glBufferData
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(sizeof(float) * orbits_.size()), orbits_.data(), GL_STATIC_DRAW);

    // https://wiki.delphigl.com/index.php/glVertexAttribPointer
    // activate first attribute on gpu (Position, 3 floats)
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with offset to 0 (start of Position)
    // each orbit consists of 3 floats, offset (pointer to first component) is 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    
    // There is only one attribute to activate (position), we don't need a second one here

    // store type of primitive to draw
    // https://en.wikibooks.org/wiki/OpenGL_Programming/GLStart/Tut3
    orbit_object.draw_mode = GL_LINES;
    // transfer number of indices to model object 
    orbit_object.num_elements = GLsizei(orbits_.size() / 3); //divide by 3 because each orbit consists of 3 floats (shoul be 360!)
}

void ApplicationSolar::drawOrbit(std::shared_ptr<Node> const& planet) const{

    float distance = planet->getDistanceOrigin().x;
    glm::fmat4 orbit_matrix = glm::fmat4{};
    
    if(planet->getDepth() == 3){
        // all moons, orbiting around a planet that is movig itself
        // first shift the orbit to where the parent is
        glm::fmat4 parent_matrix = planet->getParent()->getLocalTransform();
        //then rotate and translate like in planetTransformations()
        orbit_matrix = glm::rotate(parent_matrix, float(glfwGetTime()) * (planet->getParent()->getSpeed()), glm::fvec3{0.0f, 1.0f, 0.0f});
        orbit_matrix = glm::translate(orbit_matrix, -1.0f * planet->getParent()->getDistanceOrigin());
        //then scale the orbit so it has the right size (distance to origin has to go into every direction!)
        orbit_matrix = glm::scale(orbit_matrix * planet->getLocalTransform(), glm::fvec3{distance, distance, distance});
    }else{
        // everyting orbiting around the sun (fixed origin)
        // orbit only needs to be scaled so its going right through the middle of its planet (distance to origin)
        orbit_matrix = glm::scale(orbit_matrix * planet->getLocalTransform(), glm::fvec3{distance, distance, distance});
    }

    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("orbit").handle);
    //give matrices to shaders
    glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("OrbitMatrix"), 
                       1, GL_FALSE, glm::value_ptr(orbit_matrix));

    // bind the VAO to draw
    glBindVertexArray(orbit_object.vertex_AO);
    // draw bound vertex array using bound shader
    glDrawArrays(orbit_object.draw_mode, 0, planet_object.num_elements);
}


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

    m_shaders.emplace("orbit", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/orbits.vert"},
                                            {GL_FRAGMENT_SHADER, m_resource_path + "shaders/orbits.frag"}}});
    m_shaders.at("orbit").u_locs["OrbitMatrix"] = -1;
    m_shaders.at("orbit").u_locs["ViewMatrix"] = -1;
    m_shaders.at("orbit").u_locs["ProjectionMatrix"] = -1;

    m_shaders.emplace("star", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/stars.vert"},
                                            {GL_FRAGMENT_SHADER, m_resource_path + "shaders/stars.frag"}}});
    m_shaders.at("star").u_locs["ViewMatrix"] = -1;
    m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
}


// ----------------------- Upload and Update -----------------------

void ApplicationSolar::uploadView() {
    // vertices are transformed in camera space, so camera transform must be inverted
    glm::fmat4 view_matrix = glm::inverse(m_view_transform);

    // upload matrix to gpu
    glUseProgram(m_shaders.at("planet").handle);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                        1, GL_FALSE, glm::value_ptr(view_matrix));

    // do the same for orbits
    glUseProgram(m_shaders.at("orbit").handle);
    glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));                    
    
    // do the same for stars
    glUseProgram(m_shaders.at("star").handle);
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection() {
    // upload matrix to gpu
    glUseProgram(m_shaders.at("planet").handle);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                        1, GL_FALSE, glm::value_ptr(m_view_projection));
    
    // do the same for orbits
    glUseProgram(m_shaders.at("orbit").handle);
    glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ProjectionMatrix"),
                        1, GL_FALSE, glm::value_ptr(m_view_projection));

    // do the same for stars
    glUseProgram(m_shaders.at("star").handle);
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                        1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 

    // those two can't be used here, cause then I get GL_INVALID_OPERATION Error
    // (wird generiert wenn glUseProgram zwischen einem glBegin und dem zugehörigen glEnd aufgerufen wird.) 
    // (it's used between glBegin and glEnd)
    // https://wiki.delphigl.com/index.php/glUseProgram
    // instead use them in each function (uploadView and uploadProjection)
    // bind shader to which to upload unforms
    // glUseProgram(m_shaders.at("planet").handle);
    // glUseProgram(m_shaders.at("star").handle);

    // upload uniform values to new locations
    uploadView();
    uploadProjection();
}

// --------------- callback functions for window events --------------
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {

    //Zoom in: I
    //Zoom out: O
    //Move up: W
    //Move down: S
    //Move to the left: A
    //Move to the right: D
  
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
    //move camera right
    else if(key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.3f, 0.0f, 0.0f});
        uploadView();
    }
    //move camera left
    else if(key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.3f, 0.0f, 0.0f});
        uploadView();
    }
    //move camera down
    else if(key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -0.3f, 0.0f});
        uploadView();
    }
    //move camera up
    else if(key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT )){
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
        m_view_transform = glm::rotate(m_view_transform, -0.005f,glm::fvec3{0.0f, 1.0f, 0.0f});
    }
    if(pos_y > 0){
        m_view_transform = glm::rotate(m_view_transform, 0.005f,glm::fvec3{1.0f, 0.0f, 0.0f});
    } 
    else if(pos_y < 0){
        m_view_transform = glm::rotate(m_view_transform, -0.005f,glm::fvec3{1.0f, 0.0f, 0.0f});
    }
    uploadView();

    // //works, but it' very confusing combined with the stuff above - its this or the other 
    // //(or do this while a mouse button is pressed maybe?)
    //
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