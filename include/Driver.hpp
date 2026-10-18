#pragma once

class Driver {
public:
    virtual ~Driver() = default;

    virtual void initialize() = 0;
    virtual void ready() = 0;
    virtual void read() = 0;
    virtual void faulted() = 0;
};
