#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "SceneGraph.hpp"
#include "Node.hpp"
#include <string>
#include "GeometryNode.hpp"
#include "PointLightNode.hpp"
#include "texture_loader.hpp"

// gpu representation of model
class ApplicationSolar : public Application {
    public:
        // allocate and initialize objects
        ApplicationSolar(std::string const& resource_path);
        // free allocated objects
        ~ApplicationSolar();

        // react to key input
        void keyCallback(int key, int action, int mods);
        //handle delta mouse movement input
        void mouseCallback(double pos_x, double pos_y);
        //handle resizing
        void resizeCallback(unsigned width, unsigned height);

        // draw all objects
        void render() const;
        void renderSkybox() const;
        void renderPlanets(std::list<std::shared_ptr<Node>> const& childrenList) const;
        void renderOrbit(std::shared_ptr<Node> const& planet) const;
        void renderStars() const;
        void renderScreenQuad() const;

    protected:
        void initializeShaderPrograms();
        void initializeGeometry();
        void initializeSkybox();
        void initializePlanets();
        void initializeTextures();
        void initializeFramebuffer(unsigned int width = 960u, unsigned int height = 840u);
        void initializeScreenQuad();
        void initializeStars();
        void initializeOrbits();

        glm::fmat4 transformPlanet(std::shared_ptr<Node> const& planet) const;
        void loadPlanetTextures(std::list<std::shared_ptr<Node>> const& childrenList);
        void loadNormalTextures(std::shared_ptr<Node> const& planet);

        // update uniform values
        void uploadUniforms();
        // upload projection matrix
        void uploadProjection();
        //Upload if celshading or not
        void uploadAppearance();
        // upload view matrix
        void uploadView();

        // cpu representation of model
        model_object planet_object;
        model_object star_object;
        model_object orbit_object;
        model_object skybox_object;
        model_object screenquad_object;

        GeometryNode SkyBox_;

        SceneGraph sceneGraph_;

        // vector to store the stars
        std::vector<GLfloat> stars_;
        // vector to store orbits
        std::vector<GLfloat> orbits_;
        
        // camera transform matrix
        glm::fmat4 m_view_transform;
        // camera projection matrix
        glm::fmat4 m_view_projection;

        PointLightNode sun_l;

        //Default
        bool shaderMode_default;
        //Normal Mapping
        bool shaderMode_normal;
        //Greyscale
        bool shaderMode_grey;
        //Vertical Mirroring
        bool shaderMode_verticalMirror;
        //Horizontial Mirroring
        bool shaderMode_horizontalMirror;
        //Blur
        bool shaderMode_blur;
        //Cell-shading
        bool shaderMode_cell;

        //Framebuffer Object with a texture as Color Attachment and a Renderbuffer as Depth Attachment (slide 5)
        texture_object FBTexture_;
        texture_object FBRenderbuffer_;
        texture_object framebuffer_;

};

#endif