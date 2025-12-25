#include "EventSystem.h"

EventSystem::EventSystem()
    : EntitySystem("EventSystem") {
}

void EventSystem::subscribe(const std::string& eventType, const EventCallback& callback) {
    subscribers[eventType].push_back(callback);
}

void EventSystem::unsubscribe(const std::string& eventType, const EventCallback& callback) {
    auto it = subscribers.find(eventType);
    if (it != subscribers.end()) {
        auto& callbacks = it->second;
        // Note: Simple implementation - in production, use proper callback comparison
        // This is a simplified version
    }
}

void EventSystem::dispatch(const std::shared_ptr<EventData>& event) {
    if (!event) return;
    
    auto it = subscribers.find(event->getEventType());
    if (it != subscribers.end()) {
        for (const auto& callback : it->second) {
            callback(event);
        }
    }
}

void EventSystem::queue(const std::shared_ptr<EventData>& event) {
    eventQueue.push_back(event);
}

void EventSystem::processQueue() {
    for (const auto& event : eventQueue) {
        dispatch(event);
    }
    eventQueue.clear();
}

void EventSystem::initialize() {
    Object::onCreate();
    initialized = true;
}

void EventSystem::shutdown() {
    eventQueue.clear();
    subscribers.clear();
    initialized = false;
}

void EventSystem::update(float deltaTime) {
    processQueue();
}
