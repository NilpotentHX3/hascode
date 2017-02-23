#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>

struct CacheAffectations {
    int cache;
    int available;
    std::set<Video*> videos;
};
std::vector<CacheAffectations> cache_affects;
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




struct ScorePerVideoServ {
    int score;
    std::vector<Request*> requests;
    ScorePerVideoServ() { score = 0; }
};
struct ScorePerVideoServCompare {
    typedef std::pair< std::pair<Video*, Server*>, ScorePerVideoServ > type;
    bool operator ()(type const& a, type const& b) const {
        return a.second.score < b.second.score;
    }
};

typedef std::map< std::pair<Video*, Server*>, ScorePerVideoServ> VideoServMap;
VideoServMap mapper;

std::vector<VideoServMap::key_type> sorted_keys;

void fillMapper() {
    for(Request& r: requests) {
        Endpoint* e = r.endpoint;
        for(std::pair<Server*, int> p : e->server_latency) {
            ScorePerVideoServ& spvs = mapper[std::make_pair(r.video, p.first)];
            spvs.score += r.nb*(e->data_latency - p.second);
            spvs.requests.push_back(&r);

            sorted_keys.push_back(std::make_pair(r.video, p.first));
        }
    }

    std::sort(sorted_keys.begin(), sorted_keys.end(),  [=](VideoServMap::key_type& a, VideoServMap::key_type& b) {
        return mapper[a].score < mapper[b].score;
    });
}

void fillCaches() {

    cache_affects.resize(servers.size());
    for(unsigned int i = 0; i < servers.size(); i++) {
        cache_affects[i].cache = i;
        cache_affects[i].available = X;
    }

    std::vector<VideoServMap::key_type>::iterator it = sorted_keys.begin();
    while(it != sorted_keys.end()) {
        ScorePerVideoServ& spvs = mapper[*it];
        int cache_id = it->second->id;
        if(cache_affects[cache_id].available >= it->first->size) {
            cache_affects[cache_id].available -= it->first->size;
            cache_affects[cache_id].videos.insert(it->first);

            for(Request* r: spvs.requests) {
                Endpoint* e = r->endpoint;
                for(std::pair<Server*, int> p : e->server_latency) {
                    ScorePerVideoServ& spvs = mapper[std::make_pair(it->first, p.first)];
                    spvs.score -= r->nb*(e->data_latency - p.second);
                    spvs.requests.erase(std::remove(spvs.requests.begin(), spvs.requests.end(), r), spvs.requests.end());
                }
            }

            // std::sort(mapper.begin(), mapper.end(), ScorePerVideoServCompare());
            std::sort(it, sorted_keys.end(),  [=](VideoServMap::key_type& a, VideoServMap::key_type& b) {
                return mapper[a].score < mapper[b].score;
            });
        }
        it++;
    }
}

void test() {
    fillMapper();
    fillCaches();
    outputFile();
}
