#pragma once


namespace Structures {

    class Part {

    protected:
        const unsigned int ID;

    public:

        Part();

        explicit Part(unsigned int i);

        unsigned int getID() const { return ID; }
    };
}