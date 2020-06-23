#include "secureMap.h"
#include <unordered_map>
#include <mutex>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>


using namespace std;

template <typename K, typename V> SecureMap<K, V>::SecureMap() {

}

template <typename K, typename V> SecureMap<K, V>::~SecureMap() {

}

template <typename K, typename V> int32_t SecureMap<K, V>::size() {
  int32_t size;
  this->mtx.lock();
  size = this->map.size();
  this->mtx.unlock();
  return size;
}

template <typename K, typename V> bool SecureMap<K, V>::empty() {
  bool empty;
  this->mtx.lock();
  empty = this->map.empty();
  this->mtx.unlock();
  return empty;
}

template <typename K, typename V> void SecureMap<K, V>::insert(K key, V value) {
  this->mtx.lock();
  this->map[key]=value;
  this->mtx.unlock();
}

template <typename K, typename V> void SecureMap<K, V>::update(K key, V value) {
  this->mtx.lock();
  auto it = this->map.find(key);
  if (it != this->map.end()) {
    it->second = value;
  }
  this->mtx.unlock();
}

template <typename K, typename V> V SecureMap<K, V>::get(K key) {
  V value;
  this->mtx.lock();
  auto it = this->map.find(key);
  if (it == this->map.end()) {
    this->mtx.unlock();
    throw invalid_argument("Key not found");
  }
  else {
    value=it->second;
  }
  this->mtx.unlock();
  return value;

}

template <typename K, typename V> bool SecureMap<K, V>::contains(K key) {
  bool contains;
  this->mtx.lock();
  auto it = this->map.find(key);
  if (it == this->map.end()) {
    contains = false;
  }
  else {
    contains = true;
  }
  this->mtx.unlock();
  return contains;
}

template <typename K, typename V> void SecureMap<K, V>::remove(K key) {
  this->mtx.lock();
  this->map.erase(key);
  this->mtx.unlock();
}

template <typename K, typename V> vector<K> SecureMap<K, V>::getkeys() {
  vector<K> new_vector;
  this->mtx.lock();
  for (auto it = this->map.begin(); it!= this->map.end();it++){
    new_vector.push_back(it->first);
  }
  this->mtx.unlock();
  return new_vector;
}
