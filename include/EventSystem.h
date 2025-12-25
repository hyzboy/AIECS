#pragma once

#include "EntitySystem.h"
#include <functional>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <any>

/// Event data base class
class EventData {
public:
    explicit EventData(const std::string& eventType)
        : eventType(eventType) {}
    virtual ~EventData() = default;

    const std::string& getEventType() const { return eventType; }

private:
    std::string eventType;
};

/// Event callback type
using EventCallback = std::function<void(const std::shared_ptr<EventData>&)>;

/// EventSystem manages event dispatching and listening
class EventSystem : public EntitySystem {
public:
    EventSystem();
    ~EventSystem() override = default;

    /// Subscribe to an event type
    void subscribe(const std::string& eventType, const EventCallback& callback);

    /// Unsubscribe from an event type
    void unsubscribe(const std::string& eventType, const EventCallback& callback);

    /// Dispatch an event immediately
    void dispatch(const std::shared_ptr<EventData>& event);

    /// Queue event for later dispatch
    void queue(const std::shared_ptr<EventData>& event);

    /// Process all queued events
    void processQueue();

    void initialize() override;
    void shutdown() override;
    void update(float deltaTime) override;

private:
    std::map<std::string, std::vector<EventCallback>> subscribers;
    std::vector<std::shared_ptr<EventData>> eventQueue;
};
