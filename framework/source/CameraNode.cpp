#include "CameraNode.hpp"

CameraNode::CameraNode():
    Node{},
    isPerspective_{true},
    isEnabled_{true},
    projectionMatrix_{1.0f}{}

CameraNode::CameraNode(bool isPerspective, bool isEnabled, 
            glm::fmat4 const& projectionMatrix):
    Node{},
    isPerspective_{isPerspective},
    isEnabled_{isEnabled},
    projectionMatrix_{projectionMatrix}{}

bool CameraNode::getPerspective() const{
    return isPerspective_;
}

bool CameraNode::getEnabled() const{
    return isEnabled_;
}

glm::fmat4 CameraNode::getProjectionMatrix() const{
    return projectionMatrix_;
}

void CameraNode::setEnabled(bool enabled){
    isEnabled_ = enabled;
}

void CameraNode::setProjectionMatrix(glm::fmat4 projectionMatrix){
    projectionMatrix_ = projectionMatrix;
}

std::ostream& CameraNode::print(std::ostream& os) const{
    Node::print(os);
    os << "Is Perspective: " << isPerspective_ << "\n"
    << "Is Enabled: " << isEnabled_ << "\n"
    << "Projection Matrix: " << glm::to_string(projectionMatrix_) << "\n";
    return os;
}