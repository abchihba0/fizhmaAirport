#pragma once

class VPP {
    private:
        int lenght;
        bool status;
    public:
        VPP(int lenght_);
    
        void set_lenght(int lenght_) { lenght = lenght_; }
        void set_status(int status_) { status = status_; }
    
        int get_lenght()const { return lenght; }
        bool get_status()const { return status; }
    };