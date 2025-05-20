#pragma once
#include<vector>
#include"Airplane.h"
#include "VPP.h"

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
	Airplane* set_manager();
	void get_manager()const;
};