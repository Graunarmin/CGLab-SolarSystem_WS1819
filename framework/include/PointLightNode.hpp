#ifndef POINTLIGHTNODE_HPP
#define POINTLIGHTNODE_HPP

#include "Node.hpp"

class PointLightNode : public Node{

    public:

        PointLightNode();
        PointLightNode(float lightIntensity, glm::fvec3 const& lightColor, std::shared_ptr<Node> const& parent, std::string const& name, std::string const& path, std::shared_ptr<Node> const& origin, int depth);

        void setLightIntensity(float lightIntensity);
        float getLightIntensity() const;

        void setLightColor(glm::fvec3 const& lightColor);
        glm::fvec3 getLightColor() const;

    private:

        float lightIntensity_;
        glm::fvec3 lightColor_;
};

#endif