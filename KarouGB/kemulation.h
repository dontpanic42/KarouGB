//
//  kemulation.h
//  KarouGB
//
//  Created by Daniel on 02.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#ifndef __KarouGB__kemulation__
#define __KarouGB__kemulation__

#include <iostream>
#include <queue>
#include <thread>
#include <unordered_map>

class KEmulation
{
private:
    enum event_id_t
    {
        PLAY_EVENT = 0,
        PAUSE_EVENT = 1,
        SAVE_GAME_EVENT = 2,
        LOAD_GAME_EVENT = 3,
        LAST_EVENT = 4
    };
    
    struct event_t
    {
        event_id_t type;
        std::string filename;
        
        event_t()
        : type(PAUSE_EVENT)
        {
        }
        
        event_t(event_id_t type)
        : type(type)
        {
        }
        
    };
    
    typedef std::function<void(const event_t &)> event_callback_t;
    typedef std::queue<event_t> queue_t;
    typedef std::mutex queue_mutex_t;
    typedef std::mutex callback_mutex_t;
    typedef std::lock_guard<queue_mutex_t> lock_guard_t;
    typedef std::unordered_map<std::size_t, std::vector<event_callback_t>> callback_map_t;
    
    queue_t queue;
    queue_mutex_t queueMutex;
    
    callback_map_t callbacks;
    callback_mutex_t callbackMutex;
    
    /* Threadsafe, kann extern aufgerufen werden */
    void trigger(const event_t & event);
    
    void addEventListener(event_id_t type, const event_callback_t & callback);
    
    /* Vom verabeitenden thread aufzurufen, ruft die zu den aktuell
     vorhandenen events passenden event-handler auf */
    void processPending();
    
    std::atomic_bool paused;
    std::atomic_bool quit;
    std::string cartName;
    std::unique_ptr<std::thread> emuthread;
    
    void mainloop();
protected:
    const std::string & getCartridgeFile();
    
    virtual void onPause() = 0;
    virtual void onResume() = 0;
    virtual void onLoadGame(const std::string & filename) = 0;
    virtual void onSaveGame(const std::string & filename) = 0;
    virtual void onInitialize() = 0;
    virtual void onTeardown() = 0;
    /* Wenn onEmulationTick() true zurück gibt, beende die emulation. */
    virtual bool onEmulationTick(bool paused) = 0;
public:
    KEmulation(const std::string & filename);
    virtual ~KEmulation() { }
    
    void start();
    
    void setRunning(bool value);
    bool isRunning();
    bool hasExited();
    
    void saveGame(const std::string & filename);
    void loadGame(const std::string & filename);
    
    void quitEmulation();
};

#endif /* defined(__KarouGB__kemulation__) */
