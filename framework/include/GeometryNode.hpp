#ifndef GEOMETRYNODE_HPP
#define GEOMETRYNODE_HPP

#include "model.hpp"
#include "Node.hpp"

class GeometryNode : public Node{
    public:
        GeometryNode();

        //constructor for planets without normal map
        GeometryNode(std::shared_ptr<Node> const& parent, std::string const& name, 
                            std::string const& path, int depth, std::shared_ptr<Node> const& origin, std::string const& texPath);

        //normal mapping constructor
        GeometryNode(std::shared_ptr<Node> const& parent, std::string const& name, 
                     std::string const& path, int depth, std::shared_ptr<Node> const& origin, std::string const& texPath, std::string const& normalTexPath);

        //skybox
        GeometryNode(std::string const& name, std::string const& texPath, std::string const& texPath2, 
                      std::string const& texPath3, std::string const& texPath4, std::string const& texPath5,
                      std::string const& texPath6);
        
        

        model getGeometry() const;
        void setGeometry(model const& model);

        //all planets
        std::string getTexpath() const override;
        void setTexPath(std::string const& texPath) override;

        pixel_data getTexture() const override;
        void setTexture(std::string const& texPath) override;

        texture_object getTextureObject() const override;
        void setTextureObject(texture_object const& textureObject) override;
        
        //Normalmapping
        std::string getNormalTexpath() const override;
        void setNormalTexPath(std::string const& normalTexPath) override;

        pixel_data getNormalTexture() const override;
        void setNormalTexture(std::string const& normalTexPath) override;

        texture_object getNormalTextureObject() const override;
        void setNormalTextureObject(texture_object const& normalTextureObject) override;
        
        //Skybox
        std::vector<std::string> getTexpaths() const override;
        void setTexPaths(std::vector<std::string> const& texPaths) override;

        std::map<std::string, pixel_data> getTextures() const override;
        void setTextures(std::vector<std::string> const& texPaths) override;

        bool hasNormapMapping() const override;
        void setHasNormalMapping(bool normalMapping) override;

        // int getPlanetID() const override;
        // void setPlanetID(int id) override;

        //std::ostream& print(std::ostream& os) const override;

    private:
        model geometry_;
        texture_object textureObject_;
        texture_object normalTextureObject_;
        std::string texPath_;
        std::string normalTexPath_;
        pixel_data texture_;
        pixel_data normalTexture_;

        //list of textures for skybox, to complicated for all planets
        std::vector<std::string> texPaths_;
        std::map<std::string, pixel_data> textures_;
        bool hasNormalMap_;

        //int planetID_;
};

#endif