#include "BouncingIeee8021dRelayWithDT.h"

Define_Module(BouncingIeee8021dRelayWithDT);

BouncingIeee8021dRelayWithDT::BouncingIeee8021dRelayWithDT() {
}

BouncingIeee8021dRelayWithDT::~BouncingIeee8021dRelayWithDT() {
}

void BouncingIeee8021dRelayWithDT::initialize(int stage) {
    // Call parent initialize first
    BouncingIeee8021dRelay::initialize(stage);
    
    if (stage == INITSTAGE_LOCAL) {
        // Initialize DT-specific parameters
        use_decision_tree = getAncestorPar("use_decision_tree");
        dt_config_file = getAncestorPar("dt_config_file");
        
        if (use_decision_tree) {
            // Initialize decision tree
            if (!dt_config_file.empty()) {
                if (!decisionTree.loadFromFile(dt_config_file)) {
                    EV_WARN << "Failed to load decision tree from file, using sample tree\n";
                    decisionTree.createSampleTree();
                }
            } else {
                EV_INFO << "No decision tree config file specified, using sample tree\n";
                decisionTree.createSampleTree();
            }
            
            // Print the tree structure
            decisionTree.print();
        }
    }
}

void BouncingIeee8021dRelayWithDT::dispatch(Packet *packet, InterfaceEntry *ie) {
    const auto& frame = packet->peekAtFront<EthernetMacHeader>();
    
    // Skip for broadcast packets and dropped packet headers
    if (frame->getIs_v2_dropped_packet_header() || frame->getDest().isBroadcast()) {
        BouncingIeee8021dRelay::dispatch(packet, ie);
        return;
    }
    
    if (use_decision_tree) {
        // Apply decision tree routing
        InterfaceEntry *dtInterface = applyDecisionTree(packet, ie);
        
        if (dtInterface != nullptr) {
            // Decision tree made a decision, use that
            BouncingIeee8021dRelay::dispatch(packet, dtInterface);
        } else {
            // Decision tree said to drop or couldn't make a decision
            EV_INFO << "Decision tree result: drop packet\n";
            delete packet;
        }
    } else {
        // Use regular dispatch
        BouncingIeee8021dRelay::dispatch(packet, ie);
    }
}

InterfaceEntry* BouncingIeee8021dRelayWithDT::applyDecisionTree(Packet *packet, InterfaceEntry *ie) {
    int decision = decisionTree.evaluate(packet, getParentModule());
    
    switch (decision) {
        case 0: // Forward
            EV_INFO << "DT decision: Forward packet on original interface\n";
            return ie;
            
        case 1: { // Deflect
            EV_INFO << "DT decision: Deflect packet\n";
            
            // Apply power-of-2 strategy for deflection
            if (port_idx_connected_to_switch_neioghbors.size() >= 2) {
                std::string switch_name = getParentModule()->getFullName();
                long min_queue_occupancy = -1;
                InterfaceEntry *chosen_interface = nullptr;
                
                // Select 2 random ports
                std::list<int> port_idx_copy = port_idx_connected_to_switch_neioghbors;
                for (int i = 0; i < 2 && !port_idx_copy.empty(); i++) {
                    int random_idx = rand() % port_idx_copy.size();
                    auto it = port_idx_copy.begin();
                    std::advance(it, random_idx);
                    
                    int interface_idx = *it;
                    port_idx_copy.erase(it);
                    
                    // Check queue occupancy
                    std::string module_path_string = switch_name + ".eth[" + std::to_string(interface_idx) + "].mac";
                    AugmentedEtherMac *mac = check_and_cast<AugmentedEtherMac *>(getModuleByPath(module_path_string.c_str()));
                    std::string queue_full_path = module_path_string + ".queue";
                    long queue_occupancy = mac->get_queue_occupancy(queue_full_path);
                    
                    // Choose the least congested
                    if (min_queue_occupancy == -1 || queue_occupancy < min_queue_occupancy) {
                        min_queue_occupancy = queue_occupancy;
                        chosen_interface = ifTable->getInterface(interface_idx);
                    }
                }
                
                if (chosen_interface)
                    return chosen_interface;
            }
            
            // Fallback to original simple selection
            if (!port_idx_connected_to_switch_neioghbors.empty()) {
                int deflectPortIdx = *port_idx_connected_to_switch_neioghbors.begin();
                return ifTable->getInterface(deflectPortIdx);
            }
            return nullptr;
        }
            
        case 2: // Drop
            EV_INFO << "DT decision: Drop packet\n";
            return nullptr;
            
        default:
            EV_ERROR << "Unknown decision tree result: " << decision << "\n";
            return ie; // Default to forwarding
    }
}