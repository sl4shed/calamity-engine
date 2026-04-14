#pragma once
#include <functional>
#include <algorithm>

// btw, this signal class is genuinely so simple that it doesn't need to have a c++ file, i do have a signal.cpp file but it's there for "futureproofing" lmao

/**
 * # Signal
 * 
 * A signal allows you to connect multiple callbacks to it and fire it with arguments. It's basically a [Godot signal](https://docs.godotengine.org/en/stable/classes/class_signal.html) (because I like Godot), however with some notable differences:
 * 
 * 
 * First off, since Calamity is single-threaded (for now), a signal should generally be used by connecting callbacks:
 * ```cpp
 * Signal<int> mySignal;
 * mySignal.connect([](int value) {
 *    Logger::info("The meaning of life is {}", value);
 * });
 * 
 * mySignal.fire(42);
 * 
 * // Output: "The meaning of life is 42"
 * ```
 * 
 * However, if you don't wish to use callbacks and you don't need arguments, you can check if the signal has been fired. For example, you could use this pattern if you want to wait for an AudioSource to finish playing:
 * ```cpp
 * std::shared_ptr<AudioSource> source = std::make_shared<AudioSource>("path/to/sound.wav");
 * source->play();
 * 
 * // then, in an update function (in a script, per se):
 * if (!source->finishedSignal.fired) return;
 * Logger::info("Sound finished playing!");
 * source->finishedSignal.reset();
 * ```
 * 
 * Since fired is a boolean, to fire a signal again you have to reset it. This is usually done by the code which recieves the signal (like the previous example).
 * ```cpp
 * Signal<int> mySignal;
 * mySignal.connect([](int value) {
 *   Logger::info("The meaning of life is {}", value);
 *   mySignal.reset(); // reset the signal so it can be fired again
 * });
 * 
 * mySignal.fire(42);
 * mySignal.fire(43);
 * 
 * // Output: "The meaning of life is 42"
 * // Output: "The meaning of life is 43"
 * ```
 */
template<typename... Args>
class Signal {
public:
    bool fired = false;

    void fire(Args... args) {
        if(fired) return;
        fired = true;
        for (const auto& callback : callbacks) {
            callback(args...);
        }
    }

    void reset() {
        fired = false;
    }

    void connect(std::function<void(Args...)> cb) {
        callbacks.push_back(cb);
    }

    void disconnect(std::function<void(Args...)> cb) {
        callbacks.erase(std::remove(callbacks.begin(), callbacks.end(), cb), callbacks.end());
    }

private:
    std::vector<std::function<void(Args...)>> callbacks;
};