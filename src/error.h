#ifndef __ERROR_H__
#define __ERROR_H__

namespace smpl {

    class Error {

        private:
            std::string _m;

        public:
            std::string msg() const {
                return _m;
            };
            Error(const std::string &new_msg) {
                _m = new_msg;
            };

    };

}

#endif
