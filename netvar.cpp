#include "netvar.hpp"

using namespace std;

vector<netvar::class_t> g_classes;

netvar::class_t read_class(remote::Handle game, remote::MapModuleMemoryRegion client, netvar::ClientClass cls) {
    netvar::class_t ret;

    netvar::RecvTable table;

    if(game.Read(cls.m_pRecvTable, &table, sizeof(table))) {
        netvar::RecvProp* props = new netvar::RecvProp[table.m_nProps];

        if(game.Read(table.m_pProps, props, sizeof(netvar::RecvProp) * table.m_nProps)) {
            for(size_t i = 0; i < table.m_nProps; i++) {
                char propName[256];

                if(game.Read(props[i].m_pVarName, propName, 256)) {
                    netvar::prop_t p;

                    p.name = propName;
                    p.offset = (size_t) props[i].m_Offset;

                    ret.props.push_back(p);
                }
            }
        }

        delete[] props;
    }

    char networkName[256];

    if(game.Read((void*) cls.m_pNetworkName, networkName, 256)) {
        ret.name = networkName;
    }

    return ret;
}

bool netvar::Cache(remote::Handle game, remote::MapModuleMemoryRegion client) {
    g_classes.clear();

    cout << "Address of thing: " << std::hex << client.start + 0x01492B30 << endl;

    //09 84 8E D4 F5 01 00
    //or      [esi+ecx*4+1F5D4h], eax

    //A1 30 2B 49 01
    //mov     eax, ds:g_pClientClassHead

    //8B 4B 0C
    //mov     ecx, [ebx+0Ch]

    //85 C0
    //test    eax, eax

    void *codeOfClientClassHead = client.find(game,
            "\x09\x84\x8E\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x8B\x4B\x0C\x85\xC0",
            "xxx????x????xxxxx");

    if(codeOfClientClassHead == 0) {
        cout << "Unable to find netvar pattern" << endl;
        return false;
    }

    cout << "codeOfClientClassHead: " << std::hex << codeOfClientClassHead << endl;

    void* addressOfClientClassHead = NULL;

    if(!game.Read(((char*) codeOfClientClassHead + 8), &addressOfClientClassHead, sizeof(void*))) {
        cout << "Unable to read code address" << endl;

        return false;
    }

    cout << "addressOfClientClassHead: " << std::hex << addressOfClientClassHead << endl;

    if(addressOfClientClassHead == 0) {
        cout << "Error reading ClientClassHead address" << endl;
        return false;
    }

    if(!game.Read(addressOfClientClassHead, &addressOfClientClassHead, sizeof(void*))) {
        cout << "Error reading ClientClassHead x2" << endl;
        return false;
    }

    ClientClass currentClass;

    if(!game.Read(addressOfClientClassHead, &currentClass, sizeof(ClientClass))) {
        cout << "Error reading ClientClassHead" << endl;
        return false;
    }

    //cout << "currentClass.m_pCreateFn: " << std::hex << (void*) currentClass.m_pCreateFn << endl;
    //cout << "currentClass.m_pCreateEventFn: " << std::hex << (void*) currentClass.m_pCreateEventFn << endl;
    //cout << "currentClass.m_pNetworkName: " << std::hex << (void*) currentClass.m_pNetworkName << endl;
    //cout << "currentClass.m_pRecvTable: " << std::hex << currentClass.m_pRecvTable << endl;
    //cout << "currentClass.m_pNext: " << std::hex << currentClass.m_pNext << endl;
    //cout << "currentClass.m_ClassID: " << std::hex << currentClass.m_ClassID << endl;

    while(currentClass.m_pNext) {
        if(!currentClass.m_pNetworkName)
            break;

        netvar::class_t cls = read_class(game, client, currentClass);

        if(cls.name.length() && cls.props.size() > 0) {
            g_classes.push_back(cls);
        }

        if(!game.Read(currentClass.m_pNext, &currentClass, sizeof(ClientClass))) {
            break;
        }
    }

    cout << "Netvars done" << endl;

    return (g_classes.size() > 0);
}

std::vector<netvar::class_t> netvar::GetAllClasses() {
    return g_classes;
}

int netvar::GetOffset(std::string classname, std::string varname) {
    for(size_t i = 0; i < g_classes.size(); i++) {
        if(g_classes[i].name.compare(classname) != 0)
            continue;

        for(size_t p = 0; p < g_classes[i].props.size(); p++) {
            if(g_classes[i].props[p].name.compare(varname) == 0) {
                return g_classes[i].props[p].offset;
            }
        }
    }

    return -1;
}