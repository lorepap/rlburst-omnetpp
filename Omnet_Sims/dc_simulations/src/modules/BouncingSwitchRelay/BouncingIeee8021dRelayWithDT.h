#ifndef BOUNCINGIEEE8021DRELAYWITHDT_H
#define BOUNCINGIEEE8021DRELAYWITHDT_H

#include "BouncingIeee8021dRelay.h"
#include "DecisionTree.h"

/**
 * Extended relay module that includes Decision Tree based deflection
 */
class BouncingIeee8021dRelayWithDT : public BouncingIeee8021dRelay {
protected:
    // New parameters
    bool use_decision_tree;
    std::string dt_config_file;
    DecisionTree decisionTree;
    
    // Override dispatch to include DT-based decision making
    virtual void dispatch(Packet *packet, InterfaceEntry *ie) override;
    
    // Method to handle decision tree evaluations
    InterfaceEntry* applyDecisionTree(Packet *packet, InterfaceEntry *ie);
    
    // Override initialize to set up the decision tree
    virtual void initialize(int stage) override;

public:
    BouncingIeee8021dRelayWithDT();
    virtual ~BouncingIeee8021dRelayWithDT();
};

#endif // BOUNCINGIEEE8021DRELAYWITHDT_H