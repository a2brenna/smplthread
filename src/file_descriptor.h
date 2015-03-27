#ifndef __FILE_DESCRIPTOR_H__
#define __FILE_DESCRIPTOR_H__

#include "smpl.h"

class File_Descriptor : public smpl::Channel {

    private:
        int _fd;

    public:

        virtual ~File_Descriptor();
        File_Descriptor(const int &fd);

        virtual void send(const std::string &msg);
        virtual std::string recv();

};

#endif
