#include <iostream>
#include <vector>
#include <cmath>
#include <bitset>
#include <fstream>
#include <string>
#include <algorithm>
#include <iomanip>

#define ll long long

using namespace std;

const int K = 12; // Number of bits taken from branch address (lower bits).
const int M = 3;  // Number of last branches to choose from

// N-bit predictor. This will most likely be 2 only, as I implemented the 2-bit predictor state machine
const int N = 2;

const char TAKEN = 'T';
const char NOT_TAKEN = 'N';

string getBinary(ll number, int bits_num) {
    vector<char> bits(bits_num);
    for (int i = 0; i < bits_num; i++) {
        bits[i] = (number % 2 == 0) ? '0' : '1';
        number /= 2;
    }
    reverse(bits.begin(), bits.end());
    string result(bits.begin(), bits.end());
    return result;
}

char getPrediction(int state){
    char result;
    switch(state){
        case 0: result = NOT_TAKEN; break;
        case 1: result = NOT_TAKEN; break;
        case 2: result = TAKEN;     break;
        case 3: result = TAKEN;     break;
    }
    return result;
}

void change_behaviour_state(bitset<M> &state, bool is_equal) {
    for (int i = M - 1; i > 0; i--) state[i] = state[i - 1];
    if (is_equal) state[M - 1] = 0;
    else state[M - 1] = 1;
}

void change_predicition_state(bitset<N> &state, bool is_taken) {
    for (int i = 0; i < N - 1; i++) state[i] = state[i + 1];
    if (is_taken) state[N - 1] = 1;
    else state[N - 1] = 0;
}

int main()
{
    ll branches_count = 0;
    ll wrong_prediction_count = 0;

    // Local History Table, that stores the behavior for the last M branches.
    vector< bitset<M> > LHT((int)pow(2.0, K), 0);

    // Local Predicator Table.
    vector< bitset<N> > LPT((int)pow(2.0, M));

    // Local Predicator Table for each address.
    vector< vector< bitset<N> > > LPTs((int)pow(2.0, K), LPT);

    ll address; char actual_state;
    ifstream infile("branch-trace-gcc.txt");
    while (infile >> address >> actual_state) {
        branches_count++;
        int branch_address = bitset<K>(getBinary(address, K)).to_ulong();
        int selector = LHT[branch_address].to_ulong();
        char state = getPrediction(LPTs.at(branch_address).at(selector).to_ulong());

        if (state != actual_state) wrong_prediction_count++;

        if (actual_state == TAKEN) {
            change_predicition_state(LPTs.at(branch_address).at(selector), true);
            change_behaviour_state(LHT[branch_address], true);
        } else {
            change_predicition_state(LPTs.at(branch_address).at(selector), false);
        }
    }

    double accuarcy = (double) abs(branches_count - wrong_prediction_count) / branches_count;

    cout << branches_count << " " << wrong_prediction_count << endl;
    cout << setprecision(20) << accuarcy * 100 << endl;
    return 0;
}
