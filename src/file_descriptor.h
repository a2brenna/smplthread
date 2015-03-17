#ifndef __FILE_DESCRIPTOR_H__
#define __FILE_DESCRIPTOR_H__

#include "channel.h"

class File_Descriptor : smpl::Channel {

    private:

    public:

        virtual ~File_Descriptor();

        virtual void send(const std::string &msg);
        virtual std::string recv();

};

#endif
