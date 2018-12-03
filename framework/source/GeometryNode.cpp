#include "GeometryNode.hpp"

GeometryNode::GeometryNode():
    Node{},
    geometry_{}{}

GeometryNode::GeometryNode(std::shared_ptr<Node> const& parent, std::string const& name, 
                           std::string const& path, int depth, std::shared_ptr<Node> const& origin, std::string const& texPath /*int id,*/):
    Node{parent, name, path, depth, origin},
    geometry_{},
    textureObject_{}, //texture object (used like geometry model)
    texPath_{texPath},
    texture_{},
    hasNormalMap_{false},
    texPaths_{},
    textures_{}{
        //load texture information and store in planet
        //std::cout<< texPath.substr(33, texPath.size()-37) << std::endl;
        setTexture(texPath_);

    }

//Normal Mapping
GeometryNode::GeometryNode(std::shared_ptr<Node> const& parent, std::string const& name, std::string const& path, 
            int depth, std::shared_ptr<Node> const& origin, std::string const& texPath, std::string const& normalTexPath):
    Node{parent, name, path, depth, origin},
    geometry_{},
    textureObject_{}, //texture object (used like geometry model)
    texPath_{texPath},
    texture_{},
    normalTexPath_{normalTexPath},
    normalTextureObject_{},
    normalTexture_{},
    hasNormalMap_{true},
    texPaths_{},
    textures_{}{
        setTexture(texPath_);
        setNormalTexture(normalTexPath_);

    }

//Skybox
GeometryNode::GeometryNode(std::string const& name, std::string const& texPath, std::string const& texPath2, 
             std::string const& texPath3, std::string const& texPath4, std::string const& texPath5,
             std::string const& texPath6):
    Node{nullptr, name, "path", 0, nullptr},
    geometry_{},
    textureObject_{}, //texture object (used like geometry model)
    texPath_{},
    texture_{},
    hasNormalMap_{false},
    texPaths_{std::vector<std::string>{texPath,texPath2,texPath3,texPath4, texPath5, texPath6}}, 
    textures_{}{
        setTextures(texPaths_);

    }

model GeometryNode::getGeometry() const{
    return geometry_;
}

void GeometryNode::setGeometry(model const& model){
    geometry_ = model;
}

//For all planets
std::string GeometryNode::getTexpath() const{
    return texPath_;
}

void GeometryNode::setTexPath(std::string const& texPath){
    texPath_ = texPath;
}

pixel_data  GeometryNode::getTexture() const{
    return texture_;
}

void  GeometryNode::setTexture(std::string const& texPath){
    texture_ = texture_loader::file(texPath);
}

texture_object GeometryNode::getTextureObject() const{
    return textureObject_;;
}

void GeometryNode::setTextureObject(texture_object const& textureObject){
    textureObject_ = textureObject;
}

//Normalmapping
std::string GeometryNode::getNormalTexpath() const{
    return normalTexPath_;

}
void GeometryNode::setNormalTexPath(std::string const& normalTexPath){
    normalTexPath_ = normalTexPath;
}


pixel_data GeometryNode::getNormalTexture() const{
    return normalTexture_;

}
void GeometryNode::setNormalTexture(std::string const& normalTexPath){
    normalTexture_ = texture_loader::file(normalTexPath);
}

texture_object GeometryNode::getNormalTextureObject() const{
    return normalTextureObject_;

}

void GeometryNode::setNormalTextureObject(texture_object const& normalTextureObject){
    normalTextureObject_ = normalTextureObject;
}

bool GeometryNode::hasNormapMapping() const {
    return hasNormalMap_;
}

void GeometryNode::setHasNormalMapping(bool normalMapping) {
    hasNormalMap_ = normalMapping;

}

//Skybox
std::vector<std::string> GeometryNode::getTexpaths() const{
     return texPaths_;
     
 }
void GeometryNode::setTexPaths(std::vector<std::string> const& texPaths) {
    for(auto const& path : texPaths){
        texPaths_.push_back(path);
    }
}

std::map<std::string, pixel_data> GeometryNode::getTextures() const {
    return textures_;

}
void GeometryNode::setTextures(std::vector<std::string> const& texPaths) {
    for(auto const& p: texPaths){
        pixel_data tex = texture_loader::file(p);
        std::string path = p.substr(33, p.size()-37);
        //std::cout<< path << std::endl;
        textures_.emplace(std::make_pair(path, tex));
    }
}

// int GeometryNode::getPlanetID() const{
//     return planetID_;
// }

// void GeometryNode::setPlanetID(int id){
//    planetID_ = id;
// }



// std::ostream& GeometryNode::print(std::ostream& os) const{
//     Node::print(os);
//     os << "Geometry: " << geometry_ << "\n";    //For some reason it has an error here?
//     return(os);
// }