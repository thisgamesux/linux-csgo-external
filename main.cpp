#include <iostream>
#include "log.hpp"
#include "remote.hpp"
#include "hack.hpp"

using namespace std;

int main() {
    if(getuid() != 0) {
        cout << "You should run this as root." << endl;
        return 0;
    }

    cout << "s0beit linux hack version 1.0" << endl;

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
            if (region.filename.compare("client_client.so") == 0/* && region.executable*/) {
                cout << "client_client.so: [" << std::hex << region.start << "][" << std::hex << region.end << "][" << region.pathname << "]" << endl;
                client = region;
                break;
            }
        }

        usleep(500);
    }

    cout << "Found client_client.so [" << std::hex << client.start << "]" << endl;

    while(csgo.IsRunning()) {
        hack::Glow(&csgo, &client);

        //sleep(2);
        usleep(10000);
    }

    cout << "Game ended." << endl;

    return 0;
}