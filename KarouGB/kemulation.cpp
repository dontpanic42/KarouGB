//
//  kemulation.cpp
//  KarouGB
//
//  Created by Daniel on 02.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#include "kemulation.h"
KEmulation::KEmulation(const std::string & filename)
: paused(true)
, cartName(filename)
, quit(false)
, emuthread(nullptr)
{
    addEventListener(PLAY_EVENT, [this] (const event_t &) {
        this->onResume();
    });
    
    addEventListener(PAUSE_EVENT, [this] (const event_t &) {
        this->onPause();
    });
    
    addEventListener(SAVE_GAME_EVENT, [this] (const event_t & ev) {
        this->onSaveGame(ev.filename);
    });
    
    addEventListener(LOAD_GAME_EVENT, [this] (const event_t & ev) {
        this->onLoadGame(ev.filename);
    });
}

void KEmulation::start()
{
    if(!emuthread)
    {
        auto callLoop = [this](){ this->mainloop(); };
        emuthread = std::move(std::unique_ptr<std::thread>(new std::thread(callLoop)));
    }
}

void KEmulation::mainloop()
{
    onInitialize();
    
    while(!quit && !onEmulationTick(paused))
    {
        processPending();
    }
    
    quit = true;
    onTeardown();
}

bool KEmulation::hasExited()
{
    return quit;
}

void KEmulation::quitEmulation()
{
    quit = true;
    if(emuthread)
    {
        emuthread->join();
    }
}

void KEmulation::setRunning(bool value)
{
    if(value == !paused)
    {
        return;
    }
    
    paused = !value;
    
    if(value)
    {
        trigger(event_t(PLAY_EVENT));
    }
    else
    {
        trigger(event_t(PAUSE_EVENT));
    }
}

bool KEmulation::isRunning()
{
    return !paused;
}

void KEmulation::saveGame(const std::string & filename)
{
    event_t ev(SAVE_GAME_EVENT);
    ev.filename = filename;
    trigger(ev);
}

void KEmulation::loadGame(const std::string & filename)
{
    event_t ev(LOAD_GAME_EVENT);
    ev.filename = filename;
    trigger(ev);
}

const std::string & KEmulation::getCartridgeFile()
{
    return cartName;
}

void KEmulation::trigger(const event_t & event)
{
    lock_guard_t lk(queueMutex);
    
    queue.push(event);
}

void KEmulation::addEventListener(event_id_t type, const event_callback_t &callback)
{
    lock_guard_t lk(callbackMutex);
    
    std::size_t t(static_cast<std::size_t>(type));
    
    auto it = callbacks.find(type);
    if(it == callbacks.end())
    {
        callbacks.insert(std::make_pair(t, std::vector<event_callback_t>()));
    }
    
    callbacks[t].push_back(callback);
}

void KEmulation::processPending()
{
    std::unique_lock<std::mutex> lkQ(queueMutex, std::defer_lock);
    std::unique_lock<std::mutex> lkC(callbackMutex, std::defer_lock);
    std::lock(lkQ, lkC);
    
    if(queue.empty())
    {
        return;
    }
    
    /* Mache eine lokale Kopie vom queue */
    queue_t localQueue(queue);
    /* Und von den callbacks... */
    callback_map_t localMap(callbacks);
    
    /* Leere das globale queue */
    while(!queue.empty())
    {
        queue.pop();
    }
    
    /* Gib das queue und die callbacks frei, so das die eventhandler
     addEventListener() und trigger() aufrufen können, ohne das es
     zu deadlocks kommt */
    lkQ.unlock();
    lkC.unlock();
    
    while(!localQueue.empty())
    {
        const event_t & event = localQueue.front();
        auto it = localMap.find(event.type);
        if(it != localMap.end())
        {
            for(const event_callback_t & cb : it->second)
            {
                cb(event);
            }
        }
        
        localQueue.pop();
    }
}