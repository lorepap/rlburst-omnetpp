#ifndef DTNODE_H
#define DTNODE_H

#include <memory>
#include <vector>
#include <string>
#include "inet/common/packet/Packet.h"
#include "inet/linklayer/ethernet/EtherFrame_m.h"

using namespace inet;

// Forward declaration
class DTAction;

/**
 * Base class for Decision Tree nodes
 */
class DTNode {
public:
    virtual ~DTNode() {}
    virtual int evaluate(Packet *packet, cModule *switchModule) = 0;
    virtual void print(int depth = 0) const = 0;
};

/**
 * Decision node that compares a field against a threshold
 */
class DTConditionNode : public DTNode {
private:
    std::string fieldName;
    double threshold;
    std::shared_ptr<DTNode> leftChild;  // Taken when field < threshold
    std::shared_ptr<DTNode> rightChild; // Taken when field >= threshold

public:
    DTConditionNode(const std::string &fieldName, double threshold,
                   std::shared_ptr<DTNode> leftChild,
                   std::shared_ptr<DTNode> rightChild)
        : fieldName(fieldName), threshold(threshold),
          leftChild(leftChild), rightChild(rightChild) {}

    int evaluate(Packet *packet, cModule *switchModule) override;
    void print(int depth = 0) const override;
};

/**
 * Leaf node that performs an action
 */
class DTLeafNode : public DTNode {
private:
    int actionCode;  // 0=forward, 1=deflect, 2=drop
    int deflectPort; // Only used for deflect action

public:
    DTLeafNode(int actionCode, int deflectPort = -1)
        : actionCode(actionCode), deflectPort(deflectPort) {}

    int evaluate(Packet *packet, cModule *switchModule) override;
    void print(int depth = 0) const override;
};

#endif // DTNODE_H