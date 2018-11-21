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
  void planetTransformations(std::list<std::shared_ptr<Node>> const& childrenList) const;
  void drawOrbit(std::shared_ptr<Node> const& planet) const;

  //void setSun();
  //PointLightNode getSun() const;
  

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void initializePlanets();
  void initializeStars();
  void initializeOrbits();
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

  bool celShading_;
};

#endif