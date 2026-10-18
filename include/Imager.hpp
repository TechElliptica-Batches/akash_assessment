#pragma once

class Imager {
public:
    virtual ~Imager() = default;

protected:
    Imager() = default;

    Imager(const Imager&) = default;
    Imager& operator=(const Imager&) = default;
    Imager(Imager&&) = default;
    Imager& operator=(Imager&&) = default;
};
