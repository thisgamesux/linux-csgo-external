#include <iomanip>
#include "hack.hpp"

void Radar(remote::Handle* csgo, remote::MapModuleMemoryRegion* client, void* ent) {
    unsigned char spotted = 1;

    csgo->Write((void*)((unsigned long) ent + 0x929), &spotted, sizeof(unsigned char));
}

void hack::Glow(remote::Handle* csgo, remote::MapModuleMemoryRegion* client) {
    if(!csgo || !client)
        return;

    void* glowClassAddress = (void*) (client->start + 0x054612C0);

    hack::CGlowObjectManager manager;

    if(!csgo->Read(glowClassAddress, &manager, sizeof(hack::CGlowObjectManager))) {
        std::cout << "Failed to read glowClassAddress" << std::endl;
        return;
    }

    hack::GlowObjectDefinition_t* glowArray = new hack::GlowObjectDefinition_t[manager.m_GlowObjectDefinitions.Count];

    if(!csgo->Read(manager.m_GlowObjectDefinitions.DataPtr, glowArray, sizeof(hack::GlowObjectDefinition_t) * manager.m_GlowObjectDefinitions.Count)) {
        std::cout << "Failed to read m_GlowObjectDefinitions" << std::endl;
        delete[] glowArray;
        return;
    }

    for(unsigned int i = 0; i < manager.m_GlowObjectDefinitions.Count; i++) {
        if(glowArray[i].m_pEntity == NULL)
            continue;

        hack::Entity ent;

        if(csgo->Read(glowArray[i].m_pEntity, &ent, sizeof(hack::Entity))) {
            if(ent.m_iTeamNum != 2 && ent.m_iTeamNum != 3) {
                glowArray[i].m_bRenderWhenOccluded = 0;
                glowArray[i].m_bRenderWhenUnoccluded = 0;
                continue;
            }

            // Radar Hack
            Radar(csgo, client, glowArray[i].m_pEntity);

            glowArray[i].m_bRenderWhenOccluded = 1;
            glowArray[i].m_bRenderWhenUnoccluded = 0;

            if(ent.m_iTeamNum == 2) {
                glowArray[i].m_flGlowRed = 1.0f;
                glowArray[i].m_flGlowGreen = 0.0f;
                glowArray[i].m_flGlowBlue = 0.0f;
                glowArray[i].m_flGlowAlpha = 0.6f;

            } else if(ent.m_iTeamNum == 3) {
                glowArray[i].m_flGlowRed = 0.0f;
                glowArray[i].m_flGlowGreen = 0.0f;
                glowArray[i].m_flGlowBlue = 1.0f;
                glowArray[i].m_flGlowAlpha = 0.6f;
            }
        } else {
            std::cout << "Unable to read entity..." << std::endl;
        }
    }

    csgo->Write(manager.m_GlowObjectDefinitions.DataPtr, glowArray, sizeof(hack::GlowObjectDefinition_t) * manager.m_GlowObjectDefinitions.Count);

    delete[] glowArray;
}