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
    speed_(0.5f),
    distanceOrigin_(0.0f),
    radius_(1.0f),
    selfRotation_(0.5f),
    origin_(nullptr){}

Node::Node(std::shared_ptr<Node> const& parent, std::string const& name, 
           std::string const& path, int depth, std::shared_ptr<Node> const& origin):
    parent_(parent),
    children_(),
    name_(name),
    path_(path),
    depth_(depth),
    localTransform_(1.0f),
    worldTransform_(1.0f),
    speed_(0.5f),
    distanceOrigin_(0.0f),
    radius_(1.0f),
    selfRotation_(0.5f), 
    origin_(origin){}


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
    return Node{}; //If nothing was found: return empty node
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

float Node::getRadius() const{
    return radius_;
}

float Node::getSelfRotation() const{
    return selfRotation_;
}

std::shared_ptr<Node> Node::getOrigin() const{
    return origin_;
}

void Node::setColor(glm::fvec3 const& color){
    color_ = color;
}

glm::fvec3 Node::getColor() const{
    return color_;
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

void Node::setWorldTransform(glm::fmat4 const& localTransform){
    //mit Parametern?
    worldTransform_ = getParent()->getWorldTransform() * localTransform;
}

void Node::setSpeed(float speed){
    speed_ = speed;
}

void Node::setRadius(float radius){
    radius_ = radius;
}

void Node::setDistanceOrigin(glm::fvec3 const& distanceOrigin){
    distanceOrigin_ = distanceOrigin;
}

void Node::setSelfRotation(float selfRotation){
    selfRotation_ = selfRotation;
}

void Node::setOrigin(std::shared_ptr<Node> const& origin){
    origin_ = origin;
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
    << "depth: " << depth_ << "\n"
    << "origin: " << origin_->getName() <<"\n";
    // << "Local Transform: " << glm::to_string(localTransform_) << "\n"
    // << "World Transform: " << glm::to_string(worldTransform_) << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, Node const& n){
    return n.print(os);
}



