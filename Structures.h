#pragma once


namespace Structures {

    class Part {

    protected:
        const unsigned int ID;

    public:

        Part();

        Part(unsigned int i);

        unsigned int getID() { return ID; }
    };
}