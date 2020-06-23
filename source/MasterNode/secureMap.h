#pragma once

#include <unordered_map>
#include <mutex>
#include <cstdint>
#include <vector>

using namespace std;

/*thread safe dictionary based on std::unordered map. Used to implement
BlockMap and GroupMap*/
template <typename K, typename V> class SecureMap{
  public:
    SecureMap();
    ~SecureMap();

    int32_t size();
    bool empty();
    void insert(K key, V value);
    void update(K key, V value);
    V get(K key);
    bool contains(K key);
    void remove(K key);
    vector<K> getkeys();

  private:
    mutex mtx;
    unordered_map<K,V> map;

};

#include "secureMap-inl.h"
