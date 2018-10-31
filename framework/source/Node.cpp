#include "Node.hpp"

//Constructors
Node::Node():
    parent_(nullptr),
    children_(),
    name_("Node"), 
    path_(),
    depth_(0),
    localTransform_(1.0f), //Identity Matrix‚
    worldTransform_(1.0f),
    speed_(1.0f),
    distanceOrigin_(0.0f){}

Node::Node(std::shared_ptr<Node> const& parent, std::string const& name, 
           std::string const& path, int depth):
    parent_(parent),
    children_(),
    name_(name),
    path_(path),
    depth_(depth),
    localTransform_(1.0f),
    worldTransform_(1.0f),
    speed_(1.0f),
    distanceOrigin_(0.0f){}


//Getter
std::shared_ptr<Node> Node::getParent() const{
    return parent_;
}

Node Node::getChildren(std::string const& childName) const{
    for(auto const& child: children_){
        if(child -> name_ == childName){
            return *child;
        }
    }
    return Node{}; //If nothing was found: return empty node‚
}

std::list<std::shared_ptr<Node>> Node::getChildrenList() const{
    return children_;
}

std::string Node::getName() const{
    return name_;
}

std::string Node::getPath() const{
    return path_;
}

int Node::getDepth() const{
    return depth_;
}

glm::fmat4 Node::getLocalTransform() const{
    return localTransform_;
}

glm::fmat4 Node::getWorldTransform() const{
    return worldTransform_;
}

float Node::getSpeed() const{
    return speed_;
}

glm::fvec3 Node::getDistanceOrigin() const{
    return distanceOrigin_;
}

void Node::addChildren(std::shared_ptr<Node> const& child){
    children_.push_back(child);
}

Node Node::removeChildren(std::string const& childName){
    for(std::list<std::shared_ptr<Node>>::iterator it = children_.begin(); it != children_.end(); ++it){
        if((*it)-> name_ == childName){
            auto tmp = *(*it);
            children_.erase(it);
            return tmp;
        }
    }
    std::cout << "ERROR - " << childName << " does not exist in this List.";
}

//Setter‚
void Node::setParent(std::shared_ptr<Node> const& parent){
    parent_ = parent;
}

void Node::setLocalTransform(glm::fmat4 const& localTransform){
    localTransform_ = localTransform;
}

void Node::setWorldTransform(glm::fmat4 const& worldTransform){
    worldTransform_ = worldTransform;
}

void Node::setSpeed(float speed){
    speed_ = speed;
}

void Node::setDistanceOrigin(glm::fvec3 const& distanceOrigin){
    distanceOrigin_ = distanceOrigin;
}

//Just for now and us, maybe that's helpful
std::ostream& Node::print(std::ostream& os) const{
    os << "name: " << name_ << "\n"
    << "path: " << path_ << "\n"
    << "Parent: " << parent_ -> getName() << "\n" 
    << "children: ";
    for(auto const& i: children_){
        os << i -> name_ <<", ";
    } 
    os << "\n"
    << "depth: " << depth_ << "\n";
    // << "Local Transform: " << glm::to_string(localTransform_) << "\n"
    // << "World Transform: " << glm::to_string(worldTransform_) << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, Node const& n){
    return n.print(os);
}



