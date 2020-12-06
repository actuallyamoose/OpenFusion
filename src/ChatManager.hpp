#pragma once

#include "CNShardServer.hpp"

extern "C" {
#include "cvalue.h"
}

#define CMD_PREFIX '/'

typedef void (*CommandHandler)(std::string fullString, std::vector<std::string>& args, CNSocket* sock);

struct ChatCommand {
    bool isCosmo; // if this command is ran through the cosmo VM
    int requiredAccLevel;
    std::string help;

    union {
        CommandHandler handlr;
        CValue function;
    };

    // C++ command
    ChatCommand(int r, CommandHandler h): isCosmo(false), requiredAccLevel(r), handlr(h) {}
    ChatCommand(int r, CommandHandler h, std::string str): isCosmo(false), requiredAccLevel(r), help(str), handlr(h) {}
    ChatCommand(): ChatCommand(0, nullptr) {}

    // Cosmo command
    ChatCommand(int r, CValue func): isCosmo(true), requiredAccLevel(r), function(func) {}
    ChatCommand(int r, CValue func, std::string str): isCosmo(true), requiredAccLevel(r), help(str), function(func) {}
};

namespace ChatManager {
    extern std::map<std::string, ChatCommand> commands;
    void init();

    void registerCommand(std::string cmd, int requiredLevel, CommandHandler handlr, std::string help = "");

    void chatHandler(CNSocket* sock, CNPacketData* data);
    void emoteHandler(CNSocket* sock, CNPacketData* data);
    void menuChatHandler(CNSocket* sock, CNPacketData* data);
    void sendServerMessage(CNSocket* sock, std::string msg); // uses MOTD
    void announcementHandler(CNSocket* sock, CNPacketData* data);

    std::string sanitizeText(std::string text, bool allowNewlines=false);
}
