#ifndef CAMERANODE_HPP
#define CAMERANODE_HPP

#include "Node.hpp"

class CameraNode : public Node{

    public:
        CameraNode();
        CameraNode(bool isPerspective, bool isEnabled, 
                   glm::mat4 const& projectionMatrix);
        
        bool getPerspective() const;
        bool getEnabled() const;
        glm::mat4 getProjectionMatrix() const;

        void setEnabled(bool enabled);
        void setProjectionMatrix(glm::mat4 projectionMatrix);

        std::ostream& print(std::ostream& os) const override;
    
    private:

        bool isPerspective_;
        bool isEnabled_;
        glm::mat4 projectionMatrix_;
};

#endif