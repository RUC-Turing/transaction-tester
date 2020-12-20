#include "qry_ycsb.h"

#include <random>

#include "Framework/RecordType.h"

uint64_t QueryGenYCSB::the_n = 0;
double QueryGenYCSB::denom = 0;

void QueryGenYCSB::init() {
    mrand = (myrand *)malloc(sizeof(myrand));
    mrand->init(std::random_device()());
    zeta_2_theta = zeta(2, g_theta_zipf);
    uint64_t table_size = g_table_size_synth / g_cnt_part;
    the_n = table_size - 1;
    denom = zeta(the_n, g_theta_zipf);
}

QryYCSB *QueryGenYCSB::create_query() {
    QryYCSB *query;
    query = gen_requests_zipf();
    return query;
}

void QryYCSB::print() {
    for (uint64_t i = 0; i < requests.size(); i++) {
        printf("%d %ld, ", requests[i]->acctype, requests[i]->key);
    }
    printf("\n");
}

void QryYCSB::init() { requests.resize(YCSB_REQ_PER_QUERY); }

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

BaseQry *QueryGenYCSB::gen_requests_zipf() {
    QryYCSB *query = (QryYCSB *)malloc(sizeof(QryYCSB));
    new (query) QryYCSB();
    query->requests.init(YCSB_REQ_PER_QUERY);

    uint64_t access_cnt = 0;
    set<uint64_t> all_keys;

    uint64_t table_size = g_table_size_synth / g_cnt_part;

    double r_twr = (double)(mrand->next() % 10000) / 10000;

    int rid = 0;
    for (int i = 0; i < YCSB_REQ_PER_QUERY; i++) {
        double r = (double)(mrand->next() % 10000) / 10000;
        rqst_ycsb *req = (rqst_ycsb *)malloc(sizeof(rqst_ycsb));
        if (r_twr < 0.5)
            req->acctype = RD;
        else
            req->acctype = WR;
        uint64_t row_id = zipf(TABLE_SIZE - 1, 0.3);
        uint64_t primary_key = row_id;

        req->key = primary_key;
        req->value = mrand->next() % (1 << 8);
        // Make sure a single row is not accessed twice
        if (all_keys.find(req->key) == all_keys.end()) {
            all_keys.insert(req->key);
            access_cnt++;
        } else {
            i--;
            continue;
        }
        rid++;
        query->requests.add(req);
    }

    // Sort the requests in key order.
    if (g_key_order) {
        for (uint64_t i = 0; i < query->requests.size(); i++) {
            for (uint64_t j = query->requests.size() - 1; j > i; j--) {
                if (query->requests[j]->key < query->requests[j - 1]->key) {
                    query->requests.swap(j, j - 1);
                }
            }
        }
        // std::sort(query->requests.begin(),query->requests.end(),[](rqst_ycsb lhs, rqst_ycsb rhs) { return
        // lhs.key < rhs.key;});
    }

    // query->print();
    return query;
}
