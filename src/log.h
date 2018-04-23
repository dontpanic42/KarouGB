//
//  log.h
//  KarouLog
//
//  Created by Daniel on 26.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __KarouLog__log__
#define __KarouLog__log__

#include <iostream>
#include <queue>
#include <thread>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <atomic>

#define LOG_MAX_MESSAGE_LENGTH 256
namespace lg
{
    enum log_level
    {
        DBG = 0,
        INFO = 1,
        WARN = 2,
        ERROR = 3,
        /* Höchstes Log-Level - sollte nicht für 
           die Ausgabe von Log-Messages verwendet werden
           (also nicht print(LOG_DISABLED,...) */
        LOG_DISABLED = 4
    };
    
    namespace internal
    {
        struct log_entry_t {
            log_level level;
            std::string message;
            
            log_entry_t()
            : level(INFO)
            , message("")
            {
            }
            
            log_entry_t(log_level level, const std::string & message)
            : level(level)
            , message(message)
            {
            }
        };
        
        /* Consumer */
        template<typename T = log_entry_t>
        class Worker
        {
        public:
            typedef T entry_t;
            typedef std::queue<entry_t> queue_t;
        private:
            std::shared_ptr<std::mutex> m;
            std::shared_ptr<queue_t> queue;
            std::atomic_bool stopped;
            std::shared_ptr<std::condition_variable> cv;
            std::atomic_char logLevel;
        public:
            Worker(std::shared_ptr<std::mutex> & m,
                   std::shared_ptr<std::condition_variable> & cv,
                   std::shared_ptr<queue_t> queue)
            : m(m)
            , cv(cv)
            , queue(queue)
            , stopped(false)
            , logLevel(DBG)
            {
            }
            
            virtual ~Worker()
            {
            }
            
            void run()
            {
                while(!stopped)
                {
                    std::unique_lock<std::mutex> lock(*m);
                    
                    cv->wait(lock);
                    
                    while(queue->size())
                    {
                        processEntry(queue->front());
                        queue->pop();
                    }
                }
            }
            
            void stop()
            {
                stopped = true;
                std::unique_lock<std::mutex> lock(*m);
                cv->notify_all();
            }
            
            virtual void processEntry(const entry_t & entry)
            {
                if(entry.level >= getLogLevel())
                {
                    std::cout << entry.message;
                }
            }
            
            void setLogLevel(log_level level)
            {
                logLevel = level;
            }
            
            log_level getLogLevel()
            {
                return static_cast<log_level>(static_cast<char>(logLevel));
            }
        };
        
        /* Producer */
        template<typename T = log_entry_t>
        class Logger
        {
        public:
            typedef T entry_t;
            typedef std::queue<entry_t> queue_t;
        private:
            std::shared_ptr<std::mutex> m;
            std::shared_ptr<queue_t> queue;
            std::shared_ptr<std::condition_variable> cv;
            
        public:
            Logger(std::shared_ptr<std::mutex> & m,
                   std::shared_ptr<std::condition_variable> & cv,
                   std::shared_ptr<queue_t> queue)
            : m(m)
            , cv(cv)
            , queue(queue)
            {
            }
            
            void insert(const entry_t & entry)
            {
                std::unique_lock<std::mutex> lock(*m);
                queue->push(entry);
                cv->notify_one();
            }
            
            void insert(log_level level, const std::string & message)
            {
                insert(entry_t(level, message));
            }
        };
        
        template<typename E = log_entry_t, typename T = Worker<E>, typename L = Logger<E>>
        class LogInstance
        {
        public:
            typedef T worker_t;
            typedef L logger_t;
            typedef E entry_t;
            typedef std::queue<entry_t> queue_t;
        private:
            std::unique_ptr<worker_t> workerptr;
            std::unique_ptr<logger_t> loggerptr;
            std::unique_ptr<std::thread> workerthread;
            
            std::shared_ptr<std::mutex> mut;
            std::shared_ptr<queue_t> queue;
            std::shared_ptr<std::condition_variable> cv;
            std::chrono::time_point<std::chrono::system_clock> timeOfCreation;
        public:
            LogInstance()
            : timeOfCreation(std::chrono::system_clock::now())
            , mut(std::make_shared<std::mutex>())
            , queue(std::make_shared<queue_t>())
            , cv(std::make_shared<std::condition_variable>())
            {
                workerptr = std::move(std::unique_ptr<worker_t>(new worker_t(mut, cv, queue)));
                loggerptr = std::move(std::unique_ptr<logger_t>(new logger_t(mut, cv, queue)));
                
                workerthread = std::move(std::unique_ptr<std::thread>(new std::thread([this](){
                    this->workerptr->run();
                })));
            }
            
            ~LogInstance()
            {
                workerptr->stop();
                workerthread->join();
            }
            
            worker_t & worker()
            {
                return *workerptr;
            }
            
            logger_t & logger()
            {
                return *loggerptr;
            }
            
            void setLogLevel(log_level level)
            {
                workerptr->setLogLevel(level);
            }
            
            std::chrono::microseconds getTimeSinceCreation()
            {
                auto now = std::chrono::system_clock::now();
                return std::chrono::duration_cast<std::chrono::microseconds>(now - timeOfCreation);
            }
        };
        
        template<typename E = log_entry_t, typename T = Worker<E>, typename L = Logger<E>>
        LogInstance<E, T, L> & getDefaultLog()
        {
            static LogInstance<E, T, L> log;
            return log;
        }
    }
    
    template<typename E = internal::log_entry_t,
             typename T = internal::Worker<E>,
             typename L = internal::Logger<E>>
    void setLevel(log_level level)
    {
        internal::getDefaultLog<E, T, L>().setLogLevel(level);
    }
    
    template<typename... Ts>
    void print(log_level level,
               const std::string & levelName,
               const std::string & tag,
               const std::string & message,
               const Ts & ... ts)
    {
        char buffer[LOG_MAX_MESSAGE_LENGTH];
        std::snprintf(&buffer[0], LOG_MAX_MESSAGE_LENGTH, message.c_str(), ts...);
        
        std::string formated_string(&buffer[0]);
        
        std::stringstream ss;
        ss << std::setw(10) << std::to_string(internal::getDefaultLog().getTimeSinceCreation().count());
        std::string timeString(ss.str() + ":");
        
        std::string prefixed_message;
        prefixed_message += timeString;
        prefixed_message += levelName + " ";
        prefixed_message += tag + ":";
        prefixed_message += formated_string;
        
        internal::getDefaultLog().logger().insert(level, prefixed_message);
    }
    
    template<typename... Ts>
    inline void info(const std::string & tag, const std::string & message, const Ts & ... ts)
    {
        print(INFO, "[info]", tag, message, ts...);
    }
    
    template<typename... Ts>
    inline void warn(const std::string & tag, const std::string & message, const Ts & ... ts)
    {
        print(WARN, "[warn]", tag, message, ts...);
    }
    
    template<typename... Ts>
    inline void error(const std::string & tag, const std::string & message, const Ts & ... ts)
    {
        print(ERROR, "[errn]", tag, message, ts...);
    }
    
    template<typename... Ts>
    inline void debug(const std::string & tag, const std::string & message, const Ts & ... ts)
    {
        print(DBG, "[debg]", tag, message, ts...);
    }
}

#endif /* defined(__KarouLog__log__) */
