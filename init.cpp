

struct Video {
	int id;
	int size;
};

struct Server {
	int id;
};

struct Endpoint {
	int id;
	int data_latency;
	std::vector<std::pair<Server*, int>> server_latency;
};

struct Request {
	int id;
	Video* video;
	Endpoint* endpoint;
	int nb;
};

int V, E, R, C, X;
std::vector<Video> videos;
std::vector<Server> servers;
std::vector<Endpoint> endpoints;
std::vector<Request> requests;

void init() {
	std::cin >> V >> E >> R >> C >> X;

	servers.resize(C);
	for (int c = 0 ; c < C ; ++c) {
		servers[c].id = c;
	}

	videos.resize(V);
	for (int v = 0 ; v < V ; ++v) {
		videos[v].id = v;
		std::cin >> videos[v].size;
	}

	endpoints.resize(E);
	for (int e = 0 ; e < E ; ++e) {
		endpoints[e].id = e;
		std::cin >> endpoints[e].data_latency;
		int K;
		std::cin >> K;
		endpoints[e].server_latency.resize(K);
		for (int k = 0 ; k < K ; ++k) {
			int c, l;
			std::cin >> c >> l;
			endpoints[e].server_latency[k] = std::make_pair(&servers[c], l);
		}
	}

	requests.resize(R);
	for (int r = 0 ; r < R ; ++r) {
		int v, e, n;
		std::cin >> v >> e >> n;
		requests[r].video = &videos[v];
		requests[r].endpoint = &endpoints[e];
		requests[r].nb = n;
        requests[r].id = r;
	}

    std::cout << "Initialized ! " << std::endl;
    std::cout << "R = " << requests.size() << std::endl;
}
