#pragma once

// Copyright (c) 2018-present The Alive2 Authors.
// Distributed under the MIT license that can be found in the LICENSE file.

#include "smt/expr.h"
#include <map>
#include <ostream>
#include <set>
#include <utility>

namespace smt {

class AndExpr {
  std::set<expr> exprs;

public:
  void add(expr &&e);
  void add(const AndExpr &other);
  void del(const AndExpr &other);
  bool contains(const expr &e) const;
  expr operator()() const;
  friend std::ostream &operator<<(std::ostream &os, const AndExpr &e);
};


class OrExpr {
  std::set<expr> exprs;

public:
  void add(expr &&e);
  expr operator()() const;
  friend std::ostream &operator<<(std::ostream &os, const OrExpr &e);
};


template <typename T>
class DisjointExpr {
  std::map<T, expr> vals; // val -> domain
  std::optional<T> default_val;

public:
  DisjointExpr() {}
  DisjointExpr(const T &default_val) : default_val(default_val) {}
  DisjointExpr(const std::optional<T> &default_val) : default_val(default_val){}
  DisjointExpr(T &&default_val) : default_val(std::move(default_val)) {}
  DisjointExpr(const expr &e, bool unpack_ite);

  template <typename V, typename D>
  void add(V &&val, D &&domain) {
    if (domain.isFalse())
      return;
    if (domain.isTrue())
      vals.clear();

    auto [I, inserted] = vals.try_emplace(std::forward<V>(val),
                                          std::forward<D>(domain));
    if (!inserted)
      I->second |= std::forward<D>(domain);
  }

  std::optional<T> operator()() const {
    std::optional<T> ret;
    for (auto &[val, domain] : vals) {
      if (domain.isTrue())
        return val;

      ret = ret ? T::mkIf(domain, val, *ret) : val;
    }
    if (ret)
      return *ret;
    if (default_val)
      return *default_val;
    return {};
  }

  auto begin() const { return vals.begin(); }
  auto end() const   { return vals.end(); }
};


class FunctionExpr {
  std::map<expr, expr> fn; // key -> val
  std::optional<expr> default_val;

public:
  FunctionExpr() {}
  FunctionExpr(expr &&default_val) : default_val(std::move(default_val)) {}
  void add(const expr &key, expr &&val);
  void add(const FunctionExpr &other);
  void del(const expr &key);

  std::optional<expr> operator()(const expr &key) const;
  const expr* lookup(const expr &key) const;

  FunctionExpr simplify() const;

  auto begin() const { return fn.begin(); }
  auto end() const { return fn.end(); }

  // for container use only
  bool operator<(const FunctionExpr &rhs) const;

  friend std::ostream& operator<<(std::ostream &os, const FunctionExpr &e);
};

}
