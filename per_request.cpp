#include <queue>
#include <fstream>

struct CacheVideo {
    int score;
    Server* cache_serv;
};

class CacheVideoCompare {
public:
    bool operator() (CacheVideo c1, CacheVideo c2) {
        return (c1.score <= c2.score);
    }
};

typedef std::priority_queue<CacheVideo, std::vector<CacheVideo>, CacheVideoCompare> SortedCachesQueue;
std::vector<SortedCachesQueue> sorted_caches_per_requests;

struct SortedRequests {
    Request* r;
    int score;
};

class SortedRequestsCompare {
public:
    bool operator() (SortedRequests c1, SortedRequests c2) {
        return (c1.score <= c2.score);
    }
};

typedef std::priority_queue<SortedRequests, std::vector<SortedRequests>, SortedRequestsCompare> SortedRequestsQueue;
SortedRequestsQueue sorted_requests;

void sortRequests() {
    sorted_caches_per_requests.resize(requests.size());
    for(Request& r: requests) {
        for(std::pair<Server*, int> p : r.endpoint->server_latency) {
            CacheVideo cv;
            cv.score = r.endpoint->data_latency - p.second;
            cv.cache_serv = p.first;
            sorted_caches_per_requests[r.id].push(cv);

            SortedRequests sr;
            sr.r = &r;
            sr.score = cv.score;
            sorted_requests.push(sr);
        }
    }
}

struct CacheAffectations {
    int cache;
    int available;
    std::vector<Video*> videos;
};
std::vector<CacheAffectations> cache_affects;

void affectVideos() {
    std::vector<bool> requestUsed;
    requestUsed.resize(requests.size());
    for(unsigned int i = 0; i < requests.size(); i++) {
        requestUsed[i] = false;
    }

    cache_affects.resize(servers.size());
    while(!sorted_requests.empty()) {
        SortedRequests sr = sorted_requests.top();
        if(!requestUsed[sr.r->id]) {
            SortedCachesQueue& scq = sorted_caches_per_requests[sr.r->id];
            while(!scq.empty()) {
                CacheVideo cv = scq.top();
                if(cache_affects[cv.cache_serv->id].available >= sr.r->video->size) {
                    cache_affects[cv.cache_serv->id].available -= sr.r->video->size;
                    cache_affects[cv.cache_serv->id].videos.push_back(sr.r->video);
                    break;
                }
                scq.pop();
            }
            requestUsed[sr.r->id] = true;
        }
        sorted_requests.pop();
    }
}

void outputFile() {
    std::ofstream file("out.txt");
    int N = 0;
    for(CacheAffectations& c: cache_affects) {
        if(c.videos.size() > 0) N++;
    }

    file << N << std::endl;

    for(CacheAffectations& c: cache_affects) {
        if(c.videos.size() > 0) {
            file << c.cache << " ";
            for(Video* v: c.videos) {
                file << v->id << " ";
            }
            file << std::endl;
        }
    }

}

void test() {
    sortRequests();
    affectVideos();
    outputFile();
}
