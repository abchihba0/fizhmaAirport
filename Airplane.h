#pragma once
#include <string>

class Airplane {
    public:
        virtual ~Airplane() = default;
        virtual std::string getType() const = 0;
        virtual int getMaxCircle() const = 0;
        virtual int getVppLength() const = 0;
        virtual int getTime() const = 0;
    protected:
        int circle = 0;
};


class CargoPlane : public Airplane {
    public:
        std::string getType() const override { return "CargoPlane"; }
        int getMaxCircle() const override {return MAX_CIRCLES;}
        int getVppLength() const override {return NEED_VPP_LENGTH;}
        int getTime() const override {return REQUIRED_TIME;}
        static const int MAX_CIRCLES = 10;  
        static const int NEED_VPP_LENGTH = 4000;  // meters
        static const int REQUIRED_TIME = 6;  //minutes
};


class PassengerPlane : public Airplane {
public:
    std::string getType() const override { return "PassengerPlane"; }
    int getMaxCircle() const override {return MAX_CIRCLES;}
    int getVppLength() const override {return NEED_VPP_LENGTH;}
    int getTime() const override {return REQUIRED_TIME;}
    static const int MAX_CIRCLES = 15;
    static const int NEED_VPP_LENGTH = 2000;  
    static const int REQUIRED_TIME = 4;  
};


class AgriculturePlane : public Airplane {
public:
    std::string getType() const override { return "AgriculturePlane"; }
    int getMaxCircle() const override {return MAX_CIRCLES;}
    int getVppLength() const override {return NEED_VPP_LENGTH;}
    int getTime() const override {return REQUIRED_TIME;}
    static const int MAX_CIRCLES = 5;
    static const int NEED_VPP_LENGTH = 500;  
    static const int REQUIRED_TIME = 2;  
};


class MilitaryPlane : public Airplane {
public:
    std::string getType() const override { return "MilitaryPlane"; }
    int getMaxCircle() const override {return MAX_CIRCLES;}
    int getVppLength() const override {return NEED_VPP_LENGTH;}
    int getTime() const override {return REQUIRED_TIME;}
    static const int MAX_CIRCLES = 20;
    static const int NEED_VPP_LENGTH = 1000;  
    static const int REQUIRED_TIME = 3;  
};


class BusinessPlane : public Airplane {
public:
    std::string getType() const override { return "BusinessPlane"; }
    int getMaxCircle() const override {return MAX_CIRCLES;}
    int getVppLength() const override {return NEED_VPP_LENGTH;}
    int getTime() const override {return REQUIRED_TIME;}
    static const int MAX_CIRCLES = 8;
    static const int NEED_VPP_LENGTH = 800;  
    static const int REQUIRED_TIME = 3;  
};


class RescuePlane : public Airplane {
public:
    std::string getType() const override { return "RescuePlane"; }
    int getMaxCircle() const override {return MAX_CIRCLES;}
    int getVppLength() const override {return NEED_VPP_LENGTH;}
    int getTime() const override {return REQUIRED_TIME;}
    static const int MAX_CIRCLES = 12;
    static const int NEED_VPP_LENGTH = 1500;  
    static const int REQUIRED_TIME = 5;  
};