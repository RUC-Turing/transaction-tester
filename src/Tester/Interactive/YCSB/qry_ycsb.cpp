#include "qry_ycsb.h"

#include <random>
#include <set>

#include "Framework/RecordType.h"

uint64_t QueryGenYCSB::the_n = 0;
double QueryGenYCSB::denom = 0;

constexpr double g_theta_zipf = 0.3;
constexpr size_t g_tableSize_synth = 10000;

void QueryGenYCSB::init() {
    mrand = (myrand *)malloc(sizeof(myrand));
    mrand->init(std::random_device()());
    zeta_2_theta = zeta(2, g_theta_zipf);
    uint64_t tableSize = g_tableSize_synth;
    the_n = tableSize - 1;
    denom = zeta(the_n, g_theta_zipf);
}

QryYCSB *QueryGenYCSB::create_query(size_t tableSize, size_t requestsPerTransaction) {
    QryYCSB *query;
    query = gen_requests_zipf(tableSize, requestsPerTransaction);
    return query;
}

void QryYCSB::print() {
    // for (uint64_t i = 0; i < requests.size(); i++) {
    //     printf("%d %ld, ", requests[i]->acctype, requests[i]->key);
    // }
    // printf("\n");
}

void QryYCSB::init(size_t requestsPerTransaction) { requests.reserve(requestsPerTransaction); }

void QryYCSB::release_requests() {
    // A bit of a hack to ensure that original requests in client query queue aren't freed
    for (uint64_t i = 0; i < requests.size(); i++) {
        free(requests[i]);
    }
}

void QryYCSB::reset() {
    // BaseQry::clear();
    requests.clear();
}

void QryYCSB::release() {
    // BaseQry::release();
    // requests.release();
}

// The following algorithm comes from the paper:
// Quickly generating billion-record synthetic databases
// However, it seems there is a small bug.
// The original paper says zeta(theta, 2.0). But I guess it should be
// zeta(2.0, theta).
double QueryGenYCSB::zeta(uint64_t n, double theta) {
    double sum = 0;
    for (uint64_t i = 1; i <= n; i++) sum += pow(1.0 / i, theta);
    return sum;
}

uint64_t QueryGenYCSB::zipf(uint64_t n, double theta) {
    double alpha = 1 / (1 - theta);
    double zetan = denom;
    double eta = (1 - pow(2.0 / n, 1 - theta)) / (1 - zeta_2_theta / zetan);
    double u = (double)(mrand->next() % 10000000) / 10000000;
    double uz = u * zetan;
    if (uz < 1)
        return 1;
    if (uz < 1 + pow(0.5, theta))
        return 2;
    return 1 + (uint64_t)(n * pow(eta * u - eta + 1, alpha));
}

QryYCSB *QueryGenYCSB::gen_requests_zipf(size_t tableSize, size_t requestsPerTransaction) {
    QryYCSB *query = (QryYCSB *)malloc(sizeof(QryYCSB));
    new (query) QryYCSB();
    query->requests.reserve(requestsPerTransaction);

    uint64_t access_cnt = 0;
    std::set<RecordKey> all_keys;

    double r_twr = (double)(mrand->next() % 10000) / 10000;

    int rid = 0;
    for (int i = 0; i < (int)requestsPerTransaction; i++) {
        // double r = (double)(mrand->next() % 10000) / 10000;
        rqst_ycsb *req = (rqst_ycsb *)malloc(sizeof(rqst_ycsb));
        if (r_twr < 0.5)
            req->acctype = RD;
        else
            req->acctype = WR;
        uint64_t row_id = zipf(tableSize - 1, 0.3);
        uint64_t primary_key = row_id;

        req->key = primary_key;
        req->value = 'a' + mrand->next() % ('z' - 'a' + 1);
        // Make sure a single row is not accessed twice
        if (all_keys.find(req->key) == all_keys.end()) {
            all_keys.insert(req->key);
            access_cnt++;
        } else {
            i--;
            continue;
        }
        rid++;
        query->requests.push_back(req);
    }

    // query->print();
    return query;
}
