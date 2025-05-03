#pragma once
#include "Airplane.cpp"
#include <vector>

class VPP {
private:
	int lenght;
	bool status;
public:
	VPP(int lenght_);
	void set_lenght(int lenght_) { lenght = lenght_; }
	int get_lenght()const { return lenght; }

	void set_status(int busy_) { status = busy_; }
	int get_status()const { return status; }
};

class Airport {
private:
	int count_vpp;
	int count_airplane;
	std::vector<VPP> vpps;
	std::vector<Airplane> manager;

public:
	Airport();

	void set_count_vpp(int count_) { count_vpp = count_; }
	void set_count_airplane(int count_) { count_airplane = count_; }
	void set_vpps(std::vector<int>lenght_);

	int get_count_vpp()const { return count_vpp; }
	int get_count_airplane()const { return count_airplane; }
	void get_vpps()const;
};
