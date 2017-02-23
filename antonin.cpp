#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "init.cpp"

int main() {
	for (auto& r : requests) {
		std::cerr << r.id << std::endl;
		for (auto& p : r.endpoint->server_latency) {
			Server& s = *p.first;
			s.score[r.video] += (double) (p.second - r.endpoint->data_latency) * r.nb / r.video->size;
		}
	}

	for (auto& s : servers) {
		std::vector<std::pair<double, Video*>> vids;
		for (auto& p : s.score) {
			vids.push_back(std::make_pair(p.second, p.first));
		}
		std::sort(vids.begin(), vids.end());
	}
	return 0;
}
