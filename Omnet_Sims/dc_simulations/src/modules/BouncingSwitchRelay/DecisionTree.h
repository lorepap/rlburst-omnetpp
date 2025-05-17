#ifndef DECISIONTREE_H
#define DECISIONTREE_H

#include "DTNode.h"
#include <memory>
#include <string>
#include <fstream>

/**
 * Decision Tree for packet routing decisions
 */
class DecisionTree {
private:
    std::shared_ptr<DTNode> root;
    
public:
    DecisionTree() : root(nullptr) {}
    
    // Load tree from configuration file
    bool loadFromFile(const std::string &filename);
    
    // Create a sample tree for testing
    void createSampleTree();
    
    // Evaluate a packet through the decision tree
    int evaluate(Packet *packet, cModule *switchModule);
    
    // Print the tree structure
    void print() const;
};

#endif // DECISIONTREE_H