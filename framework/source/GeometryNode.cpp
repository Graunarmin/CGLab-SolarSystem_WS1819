#include "GeometryNode.hpp"

GeometryNode::GeometryNode():
    Node{},
    geometry_{}{}

GeometryNode::GeometryNode(std::shared_ptr<Node> const& parent, std::string const& name, 
                           std::string const& path, int depth):
    Node{parent, name, path, depth},
    geometry_{}{}



model GeometryNode::getGeometry() const{
    return geometry_;
}

void GeometryNode::setGeometry(model const& model){
    geometry_ = model;
}

// std::ostream& GeometryNode::print(std::ostream& os) const{
//     Node::print(os);
//     os << "Geometry: " << geometry_ << "\n";    //For some reason it has an error here?
//     return(os);
// }