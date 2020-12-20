
#ifndef _QryYCSB_H_
#define _QryYCSB_H_

#include <iostream>
#include "array.h"


// Each QryYCSB contains several rqst_ycsbs, 
// each of which is a RD, WR or SCAN 
// to a single table
class myrand {
public:
    void myrand::init(uint64_t rand_seed) {
        this->rand_seed = rand_seed;
    }
    uint64_t myrand::next() {
        rand_seed = (rand_seed * 1103515247UL + 12345UL) % (1UL<<63);
        return (rand_seed / 65537) % RAND_MAX;
    }
private:
	uint64_t rand_seed;
};

enum access_t {RD, WR};
class rqst_ycsb {
public:
    rqst_ycsb() {}
    rqst_ycsb(const rqst_ycsb& req) : acctype(req.acctype), key(req.key), value(req.value) { }
	access_t acctype; 
	RecordKey key;
	char value;
};

class QueryGenYCSB {
public:
  void init();
  QryYCSB * create_query(WLSchema * h_wl, uint64_t home_partition_id);

private:
	QryYCSB * gen_requests_zipf();
	// for Zipfian distribution
	double zeta(uint64_t n, double theta);
	uint64_t zipf(uint64_t n, double theta);
	
	myrand * mrand;
	static uint64_t the_n;
	static double denom;
	double zeta_2_theta;
};

class QryYCSB{
public:
    QryYCSB() {
    }
    ~QryYCSB() {
    }

    void print();
  
	void init(uint64_t thd_id, WLSchema * h_wl) {};
    void init();
    void release();
    void release_requests();
    void reset();

    std::Array<rqst_ycsb*> requests;
};

#endif
