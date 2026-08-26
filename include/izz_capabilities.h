//
// Created by jeffrey on 12-10-22.
//
#pragma once

#include <boost/functional/hash.hpp>
#include <map>
#include <set>
#include <string>

namespace izz {

/**
 * Structure of render capabilities.
 */
struct Capabilities {
  /// capabilities that are enabled, identified by keys.
  std::set<std::string> flags;

  /// compile time settings in the form of key-values.
  std::map<std::string, std::string> settings;

  Capabilities& operator+=(const Capabilities& other) {
    *this = this->add(other);
    return *this;
  }

  Capabilities add(const Capabilities& other) {
    Capabilities merge = *this;
    for (const auto& c : other.flags) {
      merge.flags.insert(c);
    }
    for (const auto& [key, value] : other.settings) {
      merge.settings[key] = value;
    }
    return merge;
  }

  bool operator==(Capabilities& other) {
    if (flags.size() != other.flags.size()) {
      return false;
    }

    for (const auto& item : flags) {
      if (!other.flags.contains(item)) {
        return false;
      }
    }

    if (settings.size() != other.settings.size()) {
      return false;
    }

    for (const auto& [key, value] : settings) {
      if (!other.settings.contains(key)) {
        return false;
      }
      if (other.settings.at(key) != value) {
        return false;
      }
    }

    return true;
  }
};

}  // namespace izz

// custom specialization of std::hash can be injected in namespace std
template <>
struct std::hash<izz::Capabilities> {
  std::size_t operator()(izz::Capabilities const& r) const noexcept {
    std::size_t result = 0;
    boost::hash_combine(result, r.flags);
    boost::hash_combine(result, r.settings);
    return result;
  }
};
