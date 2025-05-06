#pragma once
#include<vector>
#include"Airplane.h"

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

class Airport {
private:
	int vpp_count;
	int plane_count;
	std::vector<VPP*>vpps;
	std::vector<Airplane*>manager;

public:
	Airport();

	void set_vpp_count(int count_) { vpp_count = count_; }
	void set_plane_count(int count_) { plane_count = count_; }
	void set_vpps(std::vector<int>lenghts_);

	int get_vpp_count()const { return vpp_count; }
	int get_plane_count()const { return plane_count; }
	void get_vpps()const;

	void set_manager(std::vector<int>types_);
	void get_manager()const;
};