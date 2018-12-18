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
    skybox_object{},
    SkyBox_{"Skybox", m_resource_path + "textures/skybox_up.png", m_resource_path + "textures/skybox_down.png", m_resource_path + "textures/skybox_right.png",
            m_resource_path + "textures/skybox_left.png", m_resource_path + "textures/skybox_front.png", m_resource_path + "textures/skybox_back.png" },
    sceneGraph_{},
    stars_{},
    orbits_{},
    sun_l{500.0, glm::fvec3{1.0,1.0,1.0}, nullptr, "sun_l", "root/sun_l", nullptr, 1},
    shaderMode_default{false}, //Default (Reset)
    shaderMode_normal{false}, //Normal Mapping
    shaderMode_grey{false}, //Greyscale
    shaderMode_verticalMirror{false}, //Vertical Mirroring
    shaderMode_horizontalMirror{false}, //Horizontial Mirroring
    shaderMode_blur{false}, //Blur
    shaderMode_cell{false}, //Cell-shading
    m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 50.0f})}, //Camera
    m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)},
    FBTexture_{},
    FBRenderbuffer_{},
    framebuffer_{},
    screenquad_object{}
    {
        initializeGeometry();
        initializeSkybox();
        initializePlanets();
        initializeTextures();
        initializeScreenQuad();
        initializeStars();
        initializeOrbits();
        initializeFramebuffer();
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



// ---------------------- RENDER ------------------------
void ApplicationSolar::render() const {

    // ---- Bind Framebuffer Object to render the scene to it ----
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_.handle);
    //clear Framebuffer Attachments before drawing them
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ----- upload and render skybox -----
    renderSkybox();

    // ------ render planets and orbits ------
    // get the Scenegraph and start recursive traversal
    auto solarSystem = sceneGraph_.getRoot().getChildrenList();
    renderPlanets(solarSystem);

    // ------ render stars ------
    renderStars();

    // ---- Apply Framebuffer Color Texture to Screen Quad and render it to screen ----
    renderScreenQuad();

}

void ApplicationSolar::renderSkybox() const{

    //disable writing to the depth buffers (to draw transparent objects like skybox)
    glDepthMask(GL_FALSE);

    glUseProgram(m_shaders.at("skybox").handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, SkyBox_.getTextureObject().handle);
    glUniform1i(glGetUniformLocation(m_shaders.at("skybox").handle, "SkyTexture"), 0);

    //scale skybox
    glm::fmat4 model_matrix = glm::fmat4{1.0};
    model_matrix = glm::scale(model_matrix, glm::fvec3{40});
    //give matrices to shaders
    glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ModelMatrix"),
                1, GL_FALSE, glm::value_ptr(model_matrix));
    
    glBindVertexArray(skybox_object.vertex_AO);
    glDrawElements(skybox_object.draw_mode, skybox_object.num_elements, model::INDEX.type, NULL);

    //enable writing to depth buffer again so the non-transparent objects (planets) can be rendered
    glDepthMask(GL_TRUE);
}

void ApplicationSolar::renderPlanets(std::list<std::shared_ptr<Node>> const& childrenList) const{

    // recursive traversal through SceneGraph
    // visit each child in children_ and check whether they themselves have children which have to be visited
    for(auto const& planet: childrenList){

        auto childPlanets = planet->getChildrenList();

        if(childPlanets.size() > 0){   
            renderPlanets(childPlanets); //recursive call
        }

        //ignoring the holding Nodes
        if(planet->getDepth() == 2 || planet->getDepth() == 4){

            //compute tranformations for each planet
            glm::fmat4 model_matrix = transformPlanet(planet);

            //extra matrix for normal transformation to keep them orthogonal to surface
            glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform)* model_matrix);

            if(planet->getName() == "sun"){
                // bind shader to upload uniforms
                glUseProgram(m_shaders.at("sun").handle);
                //give matrices to shaders
                glUniformMatrix4fv(m_shaders.at("sun").u_locs.at("ModelMatrix"),
                            1, GL_FALSE, glm::value_ptr(model_matrix));

                glUniformMatrix4fv(m_shaders.at("sun").u_locs.at("NormalMatrix"),
                            1, GL_FALSE, glm::value_ptr(normal_matrix));

            }else{
                 // bind shader to upload uniforms
                glUseProgram(m_shaders.at("planet").handle);
                //give matrices to shaders
                glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                            1, GL_FALSE, glm::value_ptr(model_matrix));

                glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                            1, GL_FALSE, glm::value_ptr(normal_matrix));

                // glUniform3f(m_shaders.at("planet").u_locs.at("PlanetColor"), planet->getColor().x, planet->getColor().y, planet->getColor().z);
            }

            //upload textures to shader:
            //activate texture unit
            glActiveTexture(GL_TEXTURE0);
            //bind for accessing
            glBindTexture(GL_TEXTURE_2D, planet->getTextureObject().handle);

            if(planet->getName() == "sun"){
                //upload texture unit data to shader
                glUniform1i(m_shaders.at("sun").u_locs.at("SunTexture"), 0); //0 because 0 is the texture slot we defined in glActiveTexture for this
            }else{
                //upload texture unit data to shader
                glUniform1i(m_shaders.at("planet").u_locs.at("PlanetTexture"), 0); //0 because 0 is the texture slot we defined in glActiveTexture for this

                //activate normal-mapping texture unit
                glActiveTexture(GL_TEXTURE1);
                //bind for accessing
                glBindTexture(GL_TEXTURE_2D, planet->getNormalTextureObject().handle);
                //upload normal map texture unit data to shader
                glUniform1i(m_shaders.at("planet").u_locs.at("NormalTexture"), 1); //1 because 1 is the texture slot we defined in glActiveTexture for this
                //upload if planet has a normal map or not
                glUniform1i(m_shaders.at("planet").u_locs.at("HasNormalMap"), planet->hasNormapMapping()); 

                //upload light units to shader (TO DO:create separate function for this!)
                glUniform3f(m_shaders.at("planet").u_locs.at("LightColor"), sun_l.getLightColor().x, sun_l.getLightColor().y, sun_l.getLightColor().z);
                glUniform1f(m_shaders.at("planet").u_locs.at("LightIntensity"), sun_l.getLightIntensity());
            }

            // bind the VAO to draw
            glBindVertexArray(planet_object.vertex_AO);
            // draw bound vertex array using bound shader
            glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
        }
        //std::cout << *planet;
    }
}

// transform planets
glm::fmat4 ApplicationSolar::transformPlanet(std::shared_ptr<Node> const& planet) const{

    //render the orbit for each planet
    renderOrbit(planet);
    
    //compute tranformations for each planet
    glm::fmat4 model_matrix = planet->getWorldTransform();

    if(planet->getDepth() == 4){
        //moons need extra rotation around their parent planet so we need to shift them to the parent first before
        //adding it's own rotation
        glm::fmat4 parent_matrix = planet->getOrigin()->getLocalTransform();
        model_matrix = glm::rotate(parent_matrix, float(glfwGetTime())*(planet->getOrigin()->getSpeed()), glm::fvec3{0.0f, 1.0f, 0.0f});
        model_matrix = glm::translate(model_matrix, -1.0 * planet->getOrigin()->getDistanceOrigin());
    }

    //rotation around parent
    model_matrix = glm::rotate(model_matrix* planet->getLocalTransform(),float(glfwGetTime())*(planet->getSpeed()),glm::fvec3{0.0f, 1.0f, 0.0f});
    model_matrix = glm::translate(model_matrix, -1.0f * planet->getDistanceOrigin());
    
    //selfrotation
    model_matrix = glm::rotate(model_matrix, float(glfwGetTime() * planet->getSelfRotation()),glm::fvec3{0.0f, 1.0f, 0.0f});
    
    //scale planet
    float radius = planet->getRadius();
    model_matrix = glm::scale(model_matrix, glm::fvec3{radius, radius, radius});

    return model_matrix;

}

void ApplicationSolar::renderOrbit(std::shared_ptr<Node> const& planet) const{

    float distance = planet->getDistanceOrigin().x;
    glm::fmat4 orbit_matrix = glm::fmat4{};
    
    if(planet->getDepth() == 4){
        // all moons, orbiting around a planet that is movig itself
        // first shift the orbit to where the parent is
        glm::fmat4 parent_matrix = planet->getOrigin()->getLocalTransform();
        //then rotate and translate like in planetTransformations()
        orbit_matrix = glm::rotate(parent_matrix, float(glfwGetTime()) * (planet->getOrigin()->getSpeed()), glm::fvec3{0.0f, 1.0f, 0.0f});
        orbit_matrix = glm::translate(orbit_matrix, -1.0f * planet->getOrigin()->getDistanceOrigin());
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

void ApplicationSolar::renderStars() const{
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("star").handle);
    // bind the VAO to draw
    glBindVertexArray(star_object.vertex_AO);
    // draw bound vertex array using bound shader
    glDrawArrays(star_object.draw_mode, 0, star_object.num_elements);
}

void ApplicationSolar::renderScreenQuad() const{
    // bind to default framebuffer at 0
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glUseProgram(m_shaders.at("screenquad").handle);
    glActiveTexture(GL_TEXTURE2); //texture from framebuffer is in slot 2
    glBindTexture(GL_TEXTURE_2D, FBTexture_.handle);
    //upload texture from framebuffer object to shader 
    glUniform1i(m_shaders.at("screenquad").u_locs.at("FBTexture"), 2);

    glBindVertexArray(screenquad_object.vertex_AO);
    glDrawArrays(screenquad_object.draw_mode, 0, screenquad_object.num_elements);
}


// ------------------ INITIALIZE-------------------

//init framebuffer object to render the scene to
void ApplicationSolar::initializeFramebuffer(unsigned int width, unsigned int height){
    // default width: 960u, default height: 840u (see applicatio.cpp -> resolution)

    // -------- First init the Texture (Color Attachment) --------
    glActiveTexture(GL_TEXTURE2); //0 is for textures, 1 for normalmapping
    glGenTextures(1, &FBTexture_.handle);
    glBindTexture(GL_TEXTURE_2D, FBTexture_.handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale down
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale up (render texture on area bigger than the texture)

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // MIRRORED_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // MIRRORED_REPEAT

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);


    // -------- then init the Renderbuffer (Depth Attachment) --------
    glGenRenderbuffers(1, &FBRenderbuffer_.handle);
    glBindRenderbuffer(GL_RENDERBUFFER, FBRenderbuffer_.handle);

    //Parameters are: GL_RENDERBUFFER, internalformat (here depth format?), width, height
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);


    // -------- then use both to create the Framebuffer --------
    //Define Framebuffer
    glGenFramebuffers(1, &framebuffer_.handle);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_.handle);
    //Define Attachments
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, FBTexture_.handle, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, FBRenderbuffer_.handle);
    //Define which Buffers to write
    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);
    //check that the framebuffer can be written
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::cout << "ERROR, Framebuffer can't be written " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    }
}

// create planets
void ApplicationSolar::initializePlanets(){

    //Load model
    model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

    //Initialize root: parent=nullptr, name=root, path=root, depth=0
    Node root{nullptr, "root", "root", 0, nullptr};
    auto root_p = std::make_shared<Node>(root);

    sun_l.setParent(root_p);
    auto sun_lp = std::make_shared<Node>(sun_l);
    //Initialize sun: parent=pointer to root, name=sun, path=root/sun, depth=1
    GeometryNode sun{sun_lp, "sun", "root/sun_l/sun", 2, nullptr, m_resource_path + "textures/sunmap.png"};
    //Set speed of movement
    sun.setSpeed(0.0f);
    // set speed for selfrotation
    sun.setSelfRotation(0.5f);
    sun.setDistanceOrigin(glm::fvec3{0.0f, 0.0f, 0.0f});
    sun.setRadius(2.0f);
    sun.setGeometry(planet_model);
    //convert RGB between 0 and 255: color/255 
    sun.setColor(glm::fvec3{1.0, 1.0, 0.0});

    auto sun_p = std::make_shared<GeometryNode>(sun);

    //---------------------------------- Initialize Planets ---------------------------------
    //Set size (in LocalTransform Matrix), speed and distance to origin
    
    Node mercury_h{root_p, "mercury_h", "root/mercury_h", 1, sun_p};
    auto mercury_hp = std::make_shared<Node>(mercury_h);
    GeometryNode mercury{mercury_hp, "mercury", "root/mercury_h/mercury", 2, sun_p, m_resource_path + "textures/mercurymap.png"/*, m_resource_path + "textures/mercurynormal.png"*/};
    mercury.setSpeed(0.2f);
    mercury.setDistanceOrigin(glm::fvec3{8.0f, 0.0f, 0.0f});
    mercury.setRadius(0.5f);
    mercury.setSelfRotation(0.6f); 
    mercury.setGeometry(planet_model);
    mercury.setColor(glm::fvec3{1.0, 0.698, 0.4});

    Node venus_h{root_p, "venus_h", "root/venus_h", 1, sun_p};
    auto venus_hp = std::make_shared<Node>(venus_h);
    GeometryNode venus{venus_hp, "venus", "root/venus_h/venus", 2, sun_p, m_resource_path + "textures/venusmap.png"/*, m_resource_path + "textures/venusnormal.png"*/};
    venus.setSpeed(0.15f);
    venus.setDistanceOrigin(glm::fvec3{11.0f, 0.0f, 0.0f});
    venus.setRadius(0.6f);
    venus.setSelfRotation(0.5f); 
    venus.setGeometry(planet_model);
    venus.setColor(glm::fvec3{0.8, 1.0, 1.0});

    Node earth_h{root_p, "earth_h", "root/earth_h", 1, sun_p};
    auto earth_hp = std::make_shared<Node>(earth_h);
    GeometryNode earth{earth_hp, "earth", "root/earth_h/earth", 2, sun_p, m_resource_path + "textures/earthmap.png", m_resource_path + "textures/earthnormal.png"};
    earth.setSpeed(0.1f);
    earth.setDistanceOrigin(glm::fvec3{14.0f, 0.0f, 0.0f});
    earth.setRadius(0.6f);
    earth.setSelfRotation(0.7f);
    earth.setGeometry(planet_model);
    earth.setColor(glm::fvec3{0.2, 0.6, 1.0});
    auto earth_p = std::make_shared<GeometryNode>(earth);

    Node moon_h{earth_hp, "moon_h", "root/earth_h/moon_h", 3, earth_p};
    auto moon_hp = std::make_shared<Node>(moon_h);
    GeometryNode moon{moon_hp, "moon", "root/earth_h/moon_h/moon", 4, earth_p, m_resource_path + "textures/moonmap.png"/*, m_resource_path + "textures/moonnormal.png"*/};
    moon.setSpeed(0.5f);
    moon.setDistanceOrigin(glm::fvec3{1.5f, 0.0f, 0.0f}); //in this case: Distance to earth!
    moon.setRadius(0.2f);
    moon.setSelfRotation(0.7f);
    moon.setGeometry(planet_model);
    moon.setColor(glm::fvec3{0.956, 0.956, 0.956});

    Node mars_h{root_p, "mars_h", "root/ears_h", 1, sun_p};
    auto mars_hp = std::make_shared<Node>(mars_h);
    GeometryNode mars{mars_hp, "mars", "root/mars_h/mars", 2, sun_p, m_resource_path + "textures/marsmap.png", m_resource_path + "textures/marsnormal.png"};
    mars.setSpeed(0.2f);
    mars.setDistanceOrigin(glm::fvec3{20.0f, 0.0f, 0.0f});
    mars.setRadius(0.7f);
    mars.setSelfRotation(0.5f);
    mars.setGeometry(planet_model);
    mars.setColor(glm::fvec3{1.0, 0.4, 0.4});

    Node jupiter_h{root_p, "jupiter_h", "root/jupiter_h", 1, sun_p};
    auto jupiter_hp = std::make_shared<Node>(jupiter_h);
    GeometryNode jupiter{jupiter_hp, "jupiter", "root/jupiter_h/jupiter", 2, sun_p, m_resource_path + "textures/jupitermap.png"};
    jupiter.setSpeed(0.15f);
    jupiter.setDistanceOrigin(glm::fvec3{25.0f, 0.0f, 0.0f});
    jupiter.setRadius(1.2f);
    jupiter.setSelfRotation(0.6f);
    jupiter.setGeometry(planet_model);
    jupiter.setColor(glm::fvec3{0.8, 0.6, 1.0});
    auto jupiter_p = std::make_shared<GeometryNode>(jupiter);

    Node jupiter_moon1_h{jupiter_hp, "jupiter_moon1_h", "root/jupiter_h/jupiter_moon1_h", 3, jupiter_p};
    auto jupiter_moon1_hp = std::make_shared<Node>(jupiter_moon1_h);
    GeometryNode jupiter_moon1{jupiter_moon1_hp, "jupiter_moon1", "root/jupiter_h/jupiter_moon1_h/jupiter_moon1", 4, jupiter_p, m_resource_path + "textures/moonmap.png"/*, m_resource_path + "textures/moonnormal.png"*/};
    jupiter_moon1.setSpeed(0.5f); 
    jupiter_moon1.setDistanceOrigin(glm::fvec3{2.0f, 0.0f, 0.0f}); //in this case: Distance to jupiter!
    jupiter_moon1.setRadius(0.1f); 
    jupiter_moon1.setSelfRotation(0.4f); 
    jupiter_moon1.setGeometry(planet_model);
    jupiter_moon1.setColor(glm::fvec3{1.0, 0.8, 1.0});

    Node jupiter_moon2_h{jupiter_hp, "jupiter_moon2_h", "root/jupiter_h/jupiter_moon2_h", 3, jupiter_p};
    auto jupiter_moon2_hp = std::make_shared<Node>(jupiter_moon2_h);
    GeometryNode jupiter_moon2{jupiter_moon2_hp, "jupiter_moon2", "root/jupiter_h/jupiter_moon2_h/jupiter_moon2", 4, jupiter_p, m_resource_path + "textures/moonmap.png"/*, m_resource_path + "textures/moonnormal.png"*/};
    jupiter_moon2.setSpeed(0.4f);
    jupiter_moon2.setDistanceOrigin(glm::fvec3{2.6f, 0.0f, 0.0f}); //in this case: Distance to jupiter!
    jupiter_moon2.setRadius(0.2f);
    jupiter_moon2.setSelfRotation(0.4f);
    jupiter_moon2.setGeometry(planet_model);
    jupiter_moon2.setColor(glm::fvec3{0.8, 1.0, 0.8});

    Node saturn_h{root_p, "saturn_h", "root/saturn_h", 1, sun_p};
    auto saturn_hp = std::make_shared<Node>(saturn_h);
    GeometryNode saturn{saturn_hp, "saturn", "root/saturn_h/saturn", 2, sun_p, m_resource_path + "textures/saturnmap.png"};
    saturn.setSpeed(0.15f); 
    saturn.setDistanceOrigin(glm::fvec3{33.0f, 0.0f, 0.0f});
    saturn.setRadius(1.0f);
    saturn.setSelfRotation(0.6f);
    saturn.setGeometry(planet_model);
    saturn.setColor(glm::fvec3{0.6, 0.4, 0.2});

    Node uranus_h{root_p, "uranus_h", "root/uranus_h", 1, sun_p};
    auto uranus_hp = std::make_shared<Node>(uranus_h);
    GeometryNode uranus{uranus_hp, "uranus", "root/sun/uranus", 2, sun_p, m_resource_path + "textures/uranusmap.png"};
    uranus.setSpeed(0.25f); 
    uranus.setDistanceOrigin(glm::fvec3{36.0f, 0.0f, 0.0f});
    uranus.setRadius(1.0f);
    uranus.setSelfRotation(0.8f);
    uranus.setGeometry(planet_model);
    uranus.setColor(glm::fvec3{0.6, 1.0, 1.0});

    Node neptun_h{root_p, "neptun_h", "root/neptun_h", 1, sun_p};
    auto neptun_hp = std::make_shared<Node>(neptun_h);
    GeometryNode neptun{neptun_hp, "neptun", "root/neptun_h/neptun", 2, sun_p, m_resource_path + "textures/neptunmap.png"};
    neptun.setSpeed(0.2f);
    neptun.setDistanceOrigin(glm::fvec3{39.0f, 0.0f, 0.0f});
    neptun.setRadius(0.7f);
    neptun.setSelfRotation(0.5f);
    neptun.setGeometry(planet_model);
    neptun.setColor(glm::fvec3{0.4, 0.4, 1.0});

    //---------------------- Add planets to their parents ---------------------

    root.addChildren(sun_lp);
    (*sun_lp).addChildren(std::make_shared<GeometryNode>(sun));
    root.addChildren(mercury_hp);
    (*mercury_hp).addChildren(std::make_shared<GeometryNode>(mercury));
    root.addChildren(venus_hp);
    (*venus_hp).addChildren(std::make_shared<GeometryNode>(venus));
    root.addChildren(earth_hp);
    (*earth_hp).addChildren(std::make_shared<GeometryNode>(earth));
    (*earth_hp).addChildren(moon_hp);
    (*moon_hp).addChildren(std::make_shared<GeometryNode>(moon));
    root.addChildren(mars_hp);
    (*mars_hp).addChildren(std::make_shared<GeometryNode>(mars));
    root.addChildren(jupiter_hp);
    (*jupiter_hp).addChildren(std::make_shared<GeometryNode>(jupiter));
    (*jupiter_hp).addChildren(jupiter_moon1_hp);
    (*jupiter_moon1_hp).addChildren(std::make_shared<GeometryNode>(jupiter_moon1));
    (*jupiter_hp).addChildren(jupiter_moon2_hp);
    (*jupiter_moon2_hp).addChildren(std::make_shared<GeometryNode>(jupiter_moon2));
    root.addChildren(saturn_hp);
    (*saturn_hp).addChildren(std::make_shared<GeometryNode>(saturn));
    root.addChildren(uranus_hp);
    (*uranus_hp).addChildren(std::make_shared<GeometryNode>(uranus));
    root.addChildren(neptun_hp);
    (*neptun_hp).addChildren(std::make_shared<GeometryNode>(neptun));

    //------------------------ Initialize SceneGraph -------------------------
    
    sceneGraph_.setName("solarsystem");
    sceneGraph_.setRoot(root);
}

// load models
void ApplicationSolar::initializeGeometry() {
    model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL | model::TEXCOORD);

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

    // activate third attribute on gpu
    glEnableVertexAttribArray(2);
    // second attribute is 2 floats with no offset & stride
    glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);

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

//create stars
void ApplicationSolar::initializeStars() {

    // 3000 random stars
    for(int i = 0; i < 2000; ++i){ //6 floats for each star (x,y,z (Position) and r,g,b (Color))
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

void ApplicationSolar::initializeSkybox() {

    model skybox_model = model_loader::obj(m_resource_path + "models/skybox.obj", model::NORMAL);

    // generate vertex array object
    glGenVertexArrays(1, &skybox_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(skybox_object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &skybox_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, skybox_object.vertex_BO);
    // configure currently bound array buffer
    //Buffer mit Vertexdaten werden "Verknüpft" so dass der Renderer weiß, wo sie liegen
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * skybox_model.data.size(), skybox_model.data.data(), GL_STATIC_DRAW);

    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, skybox_model.vertex_bytes, skybox_model.offsets[model::POSITION]);

    // generate generic buffer
    glGenBuffers(1, &skybox_object.element_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_object.element_BO);
    // configure currently bound array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * skybox_model.indices.size(), skybox_model.indices.data(), GL_STATIC_DRAW);

    // store type of primitive to draw
    skybox_object.draw_mode = GL_TRIANGLES;
    // transfer number of indices to model object 
    skybox_object.num_elements = GLsizei(skybox_model.indices.size());

    SkyBox_.setGeometry(skybox_model);

}

void ApplicationSolar::initializeScreenQuad() {

    model screenquad_model = model_loader::obj(m_resource_path + "models/quad.obj", model::TEXCOORD);

    // generate vertex array object
    glGenVertexArrays(1, &screenquad_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(screenquad_object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &screenquad_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, screenquad_object.vertex_BO);
    // configure currently bound array buffer
    //Buffer mit Vertexdaten werden "Verknüpft" so dass der Renderer weiß, wo sie liegen
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * screenquad_model.data.size(), screenquad_model.data.data(), GL_STATIC_DRAW);

    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, screenquad_model.vertex_bytes, screenquad_model.offsets[model::POSITION]);

    // activate second attribute on gpu
    glEnableVertexAttribArray(1);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(1, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, screenquad_model.vertex_bytes, screenquad_model.offsets[model::TEXCOORD]);

    // store type of primitive to draw
    screenquad_object.draw_mode = GL_TRIANGLE_STRIP;
    // transfer number of indices to model object 
    screenquad_object.num_elements = GLsizei(screenquad_model.indices.size());

    //glBindVertexArray(0);

}

void ApplicationSolar::initializeTextures(){

    // ----- init skybox texture -----
    auto textures = SkyBox_.getTextures();
    texture_object tex_object{};

    auto front = textures["skybox_front"];
    auto back = textures["skybox_back"];
    auto up = textures["skybox_up"];
    auto down = textures["skybox_down"];
    auto left = textures["skybox_left"];
    auto right = textures["skybox_right"];

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex_object.handle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_object.handle);

    //texture minifying and magnifying
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale down
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale up (render texture on area bigger than the texture)

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, right.channels, right.width, right.height, 0, right.channels, 
                 right.channel_type, right.ptr());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, left.channels, left.width, left.height, 0, left.channels, 
                 left.channel_type, left.ptr());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, up.channels, up.width, up.height, 0, up.channels, 
                 up.channel_type, up.ptr());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, down.channels, down.width, down.height, 0, down.channels, 
                 down.channel_type, down.ptr());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, front.channels, front.width, front.height, 0, front.channels, 
                 front.channel_type, front.ptr());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, back.channels, back.width, back.height, 0, back.channels, 
                 back.channel_type, back.ptr());

    SkyBox_.setTextureObject(tex_object);


    // ----- init planet textures -----
    auto childrenList = sceneGraph_.getRoot().getChildrenList();
    loadPlanetTextures(childrenList);

}

void ApplicationSolar::loadPlanetTextures(std::list<std::shared_ptr<Node>> const& childrenList){
    
    for(auto const& planet: childrenList){

        auto childPlanets = planet->getChildrenList();

        if(childPlanets.size() > 0){   
           loadPlanetTextures(childPlanets); //recursive call
        }

        if(planet->getDepth() == 2 || planet->getDepth() == 4){
            //this way the holding nodes are being ignored

            if(planet->hasNormapMapping()){
                //init normal Textures
                loadNormalTextures(planet);
            }

            auto texture = planet -> getTexture();
            auto text_object = planet -> getTextureObject();

            glActiveTexture(GL_TEXTURE0);
            glGenTextures(1, &text_object.handle);
            glBindTexture(GL_TEXTURE_2D, text_object.handle);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale down
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale up (render texture on area bigger than the texture)

            glTexImage2D(GL_TEXTURE_2D, 0, texture.channels, texture.width, texture.height, 0, texture.channels, 
                        texture.channel_type, texture.ptr());
            
            // store the loaded Texture in planet
            planet->setTextureObject(text_object);

        }
    }
}

void ApplicationSolar::loadNormalTextures(std::shared_ptr<Node> const& planet){
    
    auto texture = planet->getNormalTexture();
    auto text_object = planet -> getNormalTextureObject();

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &text_object.handle);
    glBindTexture(GL_TEXTURE_2D, text_object.handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale down
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale up (render texture on area bigger than the texture)

    glTexImage2D(GL_TEXTURE_2D, 0, texture.channels, texture.width, texture.height, 0, texture.channels, 
                texture.channel_type, texture.ptr());
    
    // store the loaded Texture in planet
    planet->setNormalTextureObject(text_object);

    
}

// load shader sources
void ApplicationSolar::initializeShaderPrograms() {
    // store shader program objects in container m_shader
    m_shaders.emplace("planet", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/planet.vert"},
                                            {GL_FRAGMENT_SHADER, m_resource_path + "shaders/planet.frag"}}});
    // request uniform locations for shader program
    m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
    m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
    m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
    m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
    //m_shaders.at("planet").u_locs["PlanetColor"] = -1;
    m_shaders.at("planet").u_locs["LightColor"] = -1;
    m_shaders.at("planet").u_locs["LightIntensity"] = -1;
    m_shaders.at("planet").u_locs["PlanetTexture"] = -1;
    m_shaders.at("planet").u_locs["NormalTexture"] = -1;
    m_shaders.at("planet").u_locs["HasNormalMap"] = 0;
    m_shaders.at("planet").u_locs["ShaderMode_normal"] = 0; //normal mapping
    m_shaders.at("planet").u_locs["ShaderMode_cell"] = 0; //cell-shading


    m_shaders.emplace("sun", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/sun.vert"},
                                            {GL_FRAGMENT_SHADER, m_resource_path + "shaders/sun.frag"}}});
    // request uniform locations for shader program
    m_shaders.at("sun").u_locs["NormalMatrix"] = -1;
    m_shaders.at("sun").u_locs["ModelMatrix"] = -1;
    m_shaders.at("sun").u_locs["ViewMatrix"] = -1;
    m_shaders.at("sun").u_locs["ProjectionMatrix"] = -1;
    m_shaders.at("sun").u_locs["SunTexture"] = -1;
    m_shaders.at("sun").u_locs["ShaderMode_cell"] = 0; //cell-shading


    m_shaders.emplace("orbit", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/orbits.vert"},
                                            {GL_FRAGMENT_SHADER, m_resource_path + "shaders/orbits.frag"}}});
    m_shaders.at("orbit").u_locs["OrbitMatrix"] = -1;
    m_shaders.at("orbit").u_locs["ViewMatrix"] = -1;
    m_shaders.at("orbit").u_locs["ProjectionMatrix"] = -1;


    m_shaders.emplace("star", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/stars.vert"},
                                            {GL_FRAGMENT_SHADER, m_resource_path + "shaders/stars.frag"}}});
    m_shaders.at("star").u_locs["ViewMatrix"] = -1;
    m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;


    m_shaders.emplace("skybox", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/skybox.vert"},
                                            {GL_FRAGMENT_SHADER, m_resource_path + "shaders/skybox.frag"}}});
    m_shaders.at("skybox").u_locs["ViewMatrix"] = -1;
    m_shaders.at("skybox").u_locs["ProjectionMatrix"] = -1;
    m_shaders.at("skybox").u_locs["ModelMatrix"] = -1;
    m_shaders.at("skybox").u_locs["SkyTexture"] = -1;


    m_shaders.emplace("screenquad", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/screenquad.vert"},
                                            {GL_FRAGMENT_SHADER, m_resource_path + "shaders/screenquad.frag"}}});

    m_shaders.at("screenquad").u_locs["FBTexture"] = -1;
    m_shaders.at("screenquad").u_locs["ShaderMode_blur"] = 0; //blur
    m_shaders.at("screenquad").u_locs["ShaderMode_grey"] = 0; //greyscale
    m_shaders.at("screenquad").u_locs["ShaderMode_verticalMirror"] = 0; //vertical mirror
    m_shaders.at("screenquad").u_locs["ShaderMode_horizontalMirror"] = 0; //horizontal mirror
   
}


// ----------------------- UPLOAD and UPDATE -----------------------

void ApplicationSolar::uploadView() {
    // vertices are transformed in camera space, so camera transform must be inverted
    glm::fmat4 view_matrix = glm::inverse(m_view_transform);

    // upload matrix to gpu
    glUseProgram(m_shaders.at("planet").handle);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                        1, GL_FALSE, glm::value_ptr(view_matrix));
    
    // upload matrix to gpu
    glUseProgram(m_shaders.at("sun").handle);
    glUniformMatrix4fv(m_shaders.at("sun").u_locs.at("ViewMatrix"),
                        1, GL_FALSE, glm::value_ptr(view_matrix));

    // do the same for orbits
    glUseProgram(m_shaders.at("orbit").handle);
    glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));                    
    
    // do the same for stars
    glUseProgram(m_shaders.at("star").handle);
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
    
    glUseProgram(m_shaders.at("skybox").handle);
    glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection() {
    // upload matrix to gpu
    glUseProgram(m_shaders.at("planet").handle);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                        1, GL_FALSE, glm::value_ptr(m_view_projection));
    
    glUseProgram(m_shaders.at("sun").handle);
    glUniformMatrix4fv(m_shaders.at("sun").u_locs.at("ProjectionMatrix"),
                        1, GL_FALSE, glm::value_ptr(m_view_projection));

    // do the same for orbits
    glUseProgram(m_shaders.at("orbit").handle);
    glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ProjectionMatrix"),
                        1, GL_FALSE, glm::value_ptr(m_view_projection));

    // do the same for stars
    glUseProgram(m_shaders.at("star").handle);
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                        1, GL_FALSE, glm::value_ptr(m_view_projection));

    glUseProgram(m_shaders.at("skybox").handle);
    glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ProjectionMatrix"),
                        1, GL_FALSE, glm::value_ptr(m_view_projection));
}

void ApplicationSolar::uploadAppearance() {
    // upload matrix to gpu
    glUseProgram(m_shaders.at("planet").handle);
    glUniform1i(m_shaders.at("planet").u_locs.at("ShaderMode_normal"), shaderMode_normal);
    glUniform1i(m_shaders.at("planet").u_locs.at("ShaderMode_cell"), shaderMode_cell);

    glUseProgram(m_shaders.at("sun").handle);
    glUniform1i(m_shaders.at("sun").u_locs.at("ShaderMode_cell"), shaderMode_cell);

    glUseProgram(m_shaders.at("screenquad").handle);
    glUniform1i(m_shaders.at("screenquad").u_locs.at("ShaderMode_blur"), shaderMode_blur);
    glUniform1i(m_shaders.at("screenquad").u_locs.at("ShaderMode_grey"), shaderMode_grey);
    glUniform1i(m_shaders.at("screenquad").u_locs.at("ShaderMode_verticalMirror"), shaderMode_verticalMirror);
    glUniform1i(m_shaders.at("screenquad").u_locs.at("ShaderMode_horizontalMirror"), shaderMode_horizontalMirror);
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 
    // upload uniform values to new locations
    uploadView();
    uploadProjection();
    uploadAppearance();
}


// ------------------------- CALLBACKS -------------------------
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {
    
    // w = up
    // s = down
    // a = camera to left
    // d = camera to right
    // 0 = gaussian blur
    // 1 = default shader / reset shaders
    // 2 = cell-shading
    // 3 = normal mapping
    // 7 = grey-scales
    // 8 = horizontal mirroring
    // 9 = vertical mirroring

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
    //Blur
    else if(key == GLFW_KEY_0 && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        shaderMode_blur ? shaderMode_blur = false : shaderMode_blur = true;
        uploadAppearance();
    }
    //Blinn-Phong Shading - Default - reset everyting
    else if(key == GLFW_KEY_1 && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        shaderMode_default = true; 
        shaderMode_blur = false; shaderMode_cell = false; shaderMode_grey = false; 
        shaderMode_normal = false; shaderMode_horizontalMirror = false; shaderMode_verticalMirror = false;
        uploadAppearance();
    }
    //Cell-Shading
    else if(key == GLFW_KEY_2 && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        shaderMode_cell ? shaderMode_cell = false : shaderMode_cell = true;
        uploadAppearance();
    }
    //Normal Mapping
    else if(key == GLFW_KEY_3 && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        shaderMode_normal ? shaderMode_normal = false : shaderMode_normal = true;
        uploadAppearance();
    }
    //Grey-Scales
    else if(key == GLFW_KEY_7 && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        shaderMode_grey ? shaderMode_grey = false : shaderMode_grey = true;
        uploadAppearance();
    }
    //horizontal mirroring
    else if(key == GLFW_KEY_8 && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        shaderMode_horizontalMirror ? shaderMode_horizontalMirror = false : shaderMode_horizontalMirror = true;
        uploadAppearance();
    }
    //vertical mirroring
    else if(key == GLFW_KEY_9 && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        shaderMode_verticalMirror ? shaderMode_verticalMirror = false : shaderMode_verticalMirror = true;
        uploadAppearance();
    }
  
  
}

//handle delta mouse movement input //see window_handler.cpp
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
  
  //resize everything with window
  // -> to see the cursor: change line 135 in window_handler.cpp from disabled to normal:
  // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
  initializeFramebuffer(width, height);

  // upload new projection matrix
  uploadProjection();
}


// exe entry point
int main(int argc, char* argv[]) {
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}