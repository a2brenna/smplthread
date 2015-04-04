#ifndef __TRIPWIRE_H__
#define __TRIPWIRE_H__

#include <functional>

class Tripwire {

    private:

        std::function<void()> _payload;

    public:

        Tripwire(const std::function<void()> &p){
            _payload = p;
        }
        ~Tripwire(){
            _payload();
        }

};

#endif
