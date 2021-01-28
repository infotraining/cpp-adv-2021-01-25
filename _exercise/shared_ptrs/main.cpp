#include <cassert>
#include <cstdlib>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>

class Observer
{
public:
    virtual void update(const std::string& event_args) = 0;
    virtual ~Observer() {}
};

class TempMonitor
{
    int temp_;
    std::set<std::weak_ptr<Observer>, std::owner_less<std::weak_ptr<Observer>>> observers_;

public:
    TempMonitor() : temp_(0)
    {
    }

    void register_observer(std::weak_ptr<Observer> observer)
    {
        observers_.insert(observer);
    }

    void unregister_observer(std::weak_ptr<Observer> observer)
    {
        observers_.erase(observer);
    }

    void set_temp(int new_temp)
    {
        if (temp_ != new_temp)
        {
            temp_ = new_temp;
            notify("Temp changed on: " + std::to_string(temp_));
        }
    }

protected:
    void notify(const std::string& event_args)
    {
        for (std::weak_ptr<Observer> observer : observers_)
        {
            if (std::shared_ptr<Observer> living_observer = observer.lock(); living_observer)
                living_observer->update(event_args);
        }
    }
};

class Fan : public Observer
{
public:
    virtual void update(const std::string& event)
    {
        std::cout << "Fan is notified: " << event << "\n";
    }
};

class Display : public Observer, public std::enable_shared_from_this<Display>
{
public:
    virtual void update(const std::string& event)
    {
        std::cout << "Update of display: " << event << std::endl;
    }

    void register_me_as_observer(TempMonitor& tm)
    {
        tm.register_observer(shared_from_this());
    }
};

int main(int argc, char const* argv[])
{
    using namespace std;

    TempMonitor s;

    std::shared_ptr<Fan> fan = std::make_shared<Fan>();
    s.register_observer(fan);

    {
        std::shared_ptr<Observer> o = std::make_shared<Display>();

        auto dp = std::dynamic_pointer_cast<Display>(o);

        dp->register_me_as_observer(s);

        s.set_temp(1);

        dp.reset();

        cout << "End of scope." << endl;
    }

    s.set_temp(2);
}
