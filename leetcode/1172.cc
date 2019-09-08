#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_set>

using namespace std;

class DinnerPlates {
    int capacity;
    vector<stack<int>> plates;
    priority_queue<int> max_heap;  // can pop
    priority_queue<int, vector<int>, std::greater<int>> min_heap;  // can push
    
public:
    DinnerPlates(int capacity) : capacity(capacity) {}
    
    void push(int val) {
        if (min_heap.empty()) {
            int idx = plates.size();
            plates.emplace_back();
            plates.back().push(val);
            max_heap.push(idx);
            if (capacity > 1)
                min_heap.push(idx);
        } else {
            int idx = min_heap.top();
            if (plates[idx].empty())
                max_heap.push(idx);
            plates[idx].push(val);
            if (plates[idx].size() == capacity)
                min_heap.pop();  // can no longer push
        }
    }
    
    int pop() {
        while (!max_heap.empty() && plates[max_heap.top()].empty())
            max_heap.pop();
        if (!max_heap.empty()) {
            int idx = max_heap.top();
            int val = plates[idx].top();
            if (plates[idx].size() == capacity)
                min_heap.push(idx);
            plates[idx].pop();
            if (plates[idx].empty())
                max_heap.pop();  // can no longer pop
            return val;
        } else {
            return -1;
        }
    }
    
    int popAtStack(int idx) {
        if (idx < plates.size() && !plates[idx].empty()) {
            int val = plates[idx].top();
            if (plates[idx].size() == capacity)
                min_heap.push(idx);
            plates[idx].pop();
            return val;   
        } else {
            return -1;
        }
    }
};