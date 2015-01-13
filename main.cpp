#include <iostream>
#include "log.hpp"
#include "remote.hpp"
#include "hack.hpp"
#include "netvar.hpp"

using namespace std;

// This isn't called or used because it's for debugging, use it if you want.
void printAllClasses() {
    for(size_t i = 0; i < netvar::GetAllClasses().size(); i++) {
        cout << netvar::GetAllClasses()[i].name << endl;

        for(size_t p = 0; p < netvar::GetAllClasses()[i].props.size(); p++) {
            cout << "\t" << netvar::GetAllClasses()[i].props[p].name << " = " << std::hex << netvar::GetAllClasses()[i].props[p].offset << endl;
        }
    }
}

int main() {
    if(getuid() != 0) {
        cout << "You should run this as root." << endl;
        return 0;
    }

    cout << "s0beit linux hack version 1.3" << endl;

    log::init();
    log::put("Hack loaded...");

    remote::Handle csgo;

    while(true) {
        if(remote::FindProcessByName("csgo_linux", &csgo)) {
            break;
        }

        usleep(500);
    }

    cout << "CSGO Process Located [" << csgo.GetPath() << "][" << csgo.GetPid() << "]" << endl;

    remote::MapModuleMemoryRegion client;

    client.start = 0;

    while(client.start == 0) {
        if(!csgo.IsRunning()) {
            cout << "Exited game before client could be located, terminating" << endl;
            return 0;
        }

        csgo.ParseMaps();

        for(auto region : csgo.regions) {
            if (region.filename.compare("client_client.so") == 0 && region.executable) {
                cout << "client_client.so: [" << std::hex << region.start << "][" << std::hex << region.end << "][" << region.pathname << "]" << endl;
                client = region;
                break;
            }
        }

        usleep(500);
    }

    cout << "Found client_client.so [" << std::hex << client.start << "]" << endl;

    //E8 ? ? ? ? 8B 78 14 6B D6 34
    void* foundGlowPointerCall = client.find(csgo,
            "\xE8\x00\x00\x00\x00\x8B\x78\x14\x6B\xD6\x34",
            "x????xxxxxx");

    if(!foundGlowPointerCall) {
        cout << "Unable to find glow pointer call reference" << endl;
        return 0;
    }

    cout << "Glow function reference: " << std::hex << foundGlowPointerCall << endl;

    unsigned long call = csgo.GetCallAddress(foundGlowPointerCall);

    if(!call) {
        cout << "Unable to read glow pointer call reference address" << endl;
        return 0;
    }

    cout << "Glow function address: " << std::hex << call << endl;

    unsigned long addressOfGlowPointer = 0;

    if(!csgo.Read((void*) (call + 0x11), &addressOfGlowPointer, sizeof(unsigned long))) {
        cout << "Unable to read address of glow pointer" << endl;
        return 0;
    }

    cout << "Glow Array: " << std::hex << addressOfGlowPointer << endl;

    // It's the same!
    // cout << "Glow Address [" << std::hex << addressOfGlowPointer << "] vs [" << (client.start + 0x054612C0) << "]" << endl;

    while(!netvar::Cache(csgo, client)) {
        if(!csgo.IsRunning()) {
            cout << "Exited game before netvars could be cached, terminating" << endl;
            return 0;
        }

        usleep(5000);
    }

    while(csgo.IsRunning()) {
        hack::Glow(&csgo, &client, addressOfGlowPointer);

        usleep(5000);
    }

    cout << "Game ended." << endl;

    return 0;
}