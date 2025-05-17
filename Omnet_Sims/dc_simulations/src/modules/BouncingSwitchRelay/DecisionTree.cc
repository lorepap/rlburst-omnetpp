#include "DecisionTree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

bool DecisionTree::loadFromFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        EV_ERROR << "Failed to open decision tree configuration file: " << filename << "\n";
        return false;
    }
    
    // Simple parsing logic for a text-based tree configuration
    // In a real implementation, this would be more robust
    
    // Format example:
    // NODE condition queue_length 50
    //   NODE condition deflect_count 2
    //     LEAF forward
    //     LEAF deflect 3
    //   LEAF drop
    
    // For now, just create a sample tree
    createSampleTree();
    return true;
}

void DecisionTree::createSampleTree() {
    // Create a simple decision tree:
    //                    queue_length < 80?
    //                    /            \
    //                  yes             no
    //                  /                \
    //           forward            deflect_count < 3?
    //                                /          \
    //                              yes           no
    //                              /              \
    //                         deflect            drop
    
    auto forwardNode = std::make_shared<DTLeafNode>(0); // forward
    auto deflectNode = std::make_shared<DTLeafNode>(1, 3); // deflect to port 3
    auto dropNode = std::make_shared<DTLeafNode>(2); // drop
    
    auto deflectCountNode = std::make_shared<DTConditionNode>(
        "deflect_count", 3, deflectNode, dropNode);
    
    root = std::make_shared<DTConditionNode>(
        "queue_length", 80, forwardNode, deflectCountNode);
}

int DecisionTree::evaluate(Packet *packet, cModule *switchModule) {
    if (!root) {
        EV_ERROR << "Decision tree not initialized\n";
        return 0; // Default to forward
    }
    
    return root->evaluate(packet, switchModule);
}

void DecisionTree::print() const {
    if (!root) {
        EV_INFO << "Decision tree not initialized\n";
        return;
    }
    
    EV_INFO << "Decision Tree Structure:\n";
    root->print();
}