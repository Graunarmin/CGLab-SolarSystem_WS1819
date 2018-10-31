#ifndef GEOMETRYNODE_HPP
#define GEOMETRYNODE_HPP

#include "model.hpp"
#include "Node.hpp"

class GeometryNode : public Node{
    public:
        GeometryNode();
        GeometryNode(std::shared_ptr<Node> const& parent, std::string const& name, 
                            std::string const& path, int depth);
        model getGeometry() const;
        void setGeometry(model const& model);

        //std::ostream& print(std::ostream& os) const override;

    private:
        model geometry_;
};

#endif