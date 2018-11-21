#include "PointLightNode.hpp"

PointLightNode::PointLightNode():
    Node{},
    lightIntensity_{},
    lightColor_{}{}

PointLightNode::PointLightNode(float lightIntensity, glm::fvec3 const& lightColor, std::shared_ptr<Node> const& parent, std::string const& name, std::string const& path, std::shared_ptr<Node> const& origin, int depth):
    Node{parent, name, path, depth, origin},
    lightIntensity_{lightIntensity},
    lightColor_{lightColor}{}

void PointLightNode::setLightIntensity(float lightIntensity){
    lightIntensity_ = lightIntensity;
}

float PointLightNode::getLightIntensity() const{
    return lightIntensity_;
}

void PointLightNode::setLightColor(glm::fvec3 const& lightColor){
    lightColor_ = lightColor;
}

glm::fvec3 PointLightNode::getLightColor() const{
    return lightColor_;
}