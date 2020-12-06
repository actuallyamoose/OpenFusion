#include "CosmoManager.hpp"

#include "../CNShardServer.hpp"

extern "C" {
#include "cbaselib.h"
#include "cvm.h"
#include "cobj.h"
#include "cparse.h"
}

CState *CosmoManager::state = nullptr;

void ctimerHandler(CNServer* serv, time_t time, TimerEvent *event) {
    // push function onto stack
    cosmoV_pushValue(CosmoManager::state, cosmoV_newObj(event->userData));

    // push UNIX time onto stack
    cosmoV_pushNumber(CosmoManager::state, time);

    // call function :)
    cosmoV_call(CosmoManager::state, 1); // the 1 arg is the time
}

CValue registerTimer(CState *state, int nargs, CValue *args) {
    if (nargs != 2) {
        cosmoV_error(state, "timer(): 2 arguments expected! %d given!", nargs);
        return cosmoV_newNil();
    }

    if (!IS_CLOSURE(args[0])) {
        cosmoV_error(state, "timer(): first argument expected to be a function! %s given!", cosmoV_typeStr(args[0]));
        return cosmoV_newNil();
    }

    if (!IS_NUMBER(args[1])) {
        cosmoV_error(state, "timer(): second argument expected to be a number! %s given!", cosmoV_typeStr(args[1]));
        return cosmoV_newNil();
    }

    CObjFunction *func = (CObjFunction*)cosmoV_readObj(args[0]);
    int timer = (int)cosmoV_readNumber(args[1]);

    TimerEvent event = TimerEvent(ctimerHandler, timer);
    event.userData = (void*)func;

    CNShardServer::Timers.push_back(event);

    return cosmoV_newNil();
}

const char* testScript = "function test(time) print(\"called at \" .. time) end setTimer(test, 2000)";

void CosmoManager::init() {
    state = cosmoV_newState();

    // load standard cosmo api
    cosmoB_loadLibrary(state);

    // load our libary
    cosmoV_register(state, "setTimer", cosmoV_newObj(cosmoO_newCFunction(state, registerTimer)));

    // run our test script
    cosmoP_compileString(state, testScript);
    cosmoV_call(state, 0); 
}