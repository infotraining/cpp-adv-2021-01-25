#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

class Observer
{
public:
    virtual void update(const std::string& event_args) = 0;
    virtual ~Observer() { }
};

class TempMonitor
{
    int temp_;
    std::set<std::weak_ptr<Observer>, std::owner_less<std::weak_ptr<Observer>>> observers_;

public:
    TempMonitor()
        : temp_(0)
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

    TempMonitor temp_monitor;

    std::shared_ptr<Fan> fan = std::make_shared<Fan>();
    temp_monitor.register_observer(fan);

    {
        std::shared_ptr<Observer> o = std::make_shared<Display>();

        auto dp = std::dynamic_pointer_cast<Display>(o);

        dp->register_me_as_observer(temp_monitor);

        temp_monitor.set_temp(1);

        dp.reset();

        cout << "End of scope." << endl;
    }

    temp_monitor.set_temp(2);
}

namespace explain_shared_from_this
{
    class BadSharingThis
    {
    public:
        void use()
        {
        }

        std::shared_ptr<BadSharingThis> get_this()
        {
            return std::shared_ptr<BadSharingThis>(this);
        }
    };

    class GoodSharingThis : std::enable_shared_from_this<GoodSharingThis>
    {
    public:
        void use()
        {
        }

        std::shared_ptr<GoodSharingThis> get_this()
        {
            return shared_from_this();
        }
    };
}

void using_shared_from_this()
{
    using namespace explain_shared_from_this;

    auto ptr1 = std::make_shared<GoodSharingThis>();

    auto ptr2 = ptr1->get_this();

    ptr1.reset();

    ptr2->use();
}
