#include "DTNode.h"
#include <iostream>
#include "../Augmented_Mac/AugmentedEtherMac.h"

// Get packet field value - extend this for more fields
double getFieldValue(const std::string &fieldName, Packet *packet, cModule *switchModule) {
    // These are examples - add more fields as needed
    if (fieldName == "packet_count") {
        // Read from metadata (would need to be stored per flow)
        return 0; // Placeholder 
    }
    else if (fieldName == "deflect_count") {
        return 0; // Placeholder
    }
    else if (fieldName == "queue_length") {
        // Get queue length
        const auto& frame = packet->peekAtFront<EthernetMacHeader>();
        int interfaceIndex = packet->getTag<InterfaceInd>()->getInterfaceId();
        std::string modulePath = switchModule->getFullName();
        modulePath += ".eth[" + std::to_string(interfaceIndex) + "].mac.queue";
        cModule* queueModule = switchModule->getModuleByPath(modulePath.c_str());
        AugmentedEtherMac *mac = check_and_cast<AugmentedEtherMac *>(
            switchModule->getModuleByPath((modulePath + "mac").c_str()));
        return mac->get_queue_occupancy(modulePath);
    }
    else if (fieldName == "ttl") {
        // Extract TTL from packet
        b packetPosition = packet->getFrontOffset();
        packet->setFrontIteratorPosition(b(0));
        auto phyHeader = packet->peekAtFront<EthernetPhyHeader>();
        auto ethHeader = packet->peekAt<EthernetMacHeader>(phyHeader->getChunkLength());
        auto ipHeader = packet->peekAt<Ipv4Header>(
            phyHeader->getChunkLength() + ethHeader->getChunkLength());
        int ttl = ipHeader->getTimeToLive();
        packet->setFrontIteratorPosition(packetPosition);
        return ttl;
    }
    
    // Add more fields as needed
    return 0.0;
}

int DTConditionNode::evaluate(Packet *packet, cModule *switchModule) {
    double fieldValue = getFieldValue(fieldName, packet, switchModule);
    
    if (fieldValue < threshold) {
        return leftChild->evaluate(packet, switchModule);
    } else {
        return rightChild->evaluate(packet, switchModule);
    }
}

void DTConditionNode::print(int depth) const {
    std::string indent(depth * 2, ' ');
    EV << indent << "Condition: " << fieldName << " < " << threshold << "\n";
    
    EV << indent << "  If true:\n";
    leftChild->print(depth + 2);
    
    EV << indent << "  If false:\n";
    rightChild->print(depth + 2);
}

int DTLeafNode::evaluate(Packet *packet, cModule *switchModule) {
    return actionCode;
}

void DTLeafNode::print(int depth) const {
    std::string indent(depth * 2, ' ');
    if (actionCode == 0) {
        EV << indent << "Action: Forward\n";
    } else if (actionCode == 1) {
        EV << indent << "Action: Deflect to port " << deflectPort << "\n";
    } else if (actionCode == 2) {
        EV << indent << "Action: Drop\n";
    } else {
        EV << indent << "Action: Unknown (" << actionCode << ")\n";
    }
}