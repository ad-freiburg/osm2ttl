// Copyright 2020, University of Freiburg
// Authors: Axel Lehmann <lehmann@cs.uni-freiburg.de>.

// This file is part of osm2rdf.
//
// osm2rdf is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// osm2rdf is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with osm2rdf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef OSM2RDF_OSM_RELATION_H_
#define OSM2RDF_OSM_RELATION_H_

#include <vector>

#include "RelationHandler.h"
#include "boost/serialization/nvp.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/version.hpp"
#include "osm2rdf/geometry/Polygon.h"
#include "osm2rdf/geometry/Relation.h"
#include "osm2rdf/osm/Box.h"
#include "osm2rdf/osm/Generic.h"
#include "osm2rdf/osm/RelationHandler.h"
#include "osm2rdf/osm/RelationMember.h"
#include "osm2rdf/osm/TagList.h"
#include "osmium/osm/relation.hpp"

namespace osm2rdf::osm {

class Relation {
 public:
  typedef uint32_t id_t;
  Relation();
  explicit Relation(const osmium::Relation& relation);
  [[nodiscard]] id_t id() const noexcept;
  [[nodiscard]] osm2rdf::osm::generic::changeset_id_t changeset() const noexcept;
  [[nodiscard]] std::time_t timestamp() const noexcept;
  [[nodiscard]] std::string user() const noexcept;
  [[nodiscard]] osm2rdf::osm::generic::version_t version() const noexcept;
  [[nodiscard]] bool visible() const noexcept;
  [[nodiscard]] const std::vector<osm2rdf::osm::RelationMember>& members()
      const noexcept;
  [[nodiscard]] const osm2rdf::osm::TagList& tags() const noexcept;
  [[nodiscard]] bool hasCompleteGeometry() const noexcept;
  [[nodiscard]] bool isArea() const noexcept;
#if BOOST_VERSION >= 107800
  [[nodiscard]] bool hasGeometry() const noexcept;
  [[nodiscard]] const osm2rdf::geometry::Box& envelope() const noexcept;
  [[nodiscard]] const osm2rdf::geometry::Relation& geom() const noexcept;
  [[nodiscard]] const osm2rdf::geometry::Polygon& convexHull() const noexcept;
  [[nodiscard]] const osm2rdf::geometry::Polygon& orientedBoundingBox() const noexcept;
  void buildGeometry(osm2rdf::osm::RelationHandler& relationHandler);
#endif  // BOOST_VERSION >= 107800

  bool operator==(const osm2rdf::osm::Relation& other) const noexcept;
  bool operator!=(const osm2rdf::osm::Relation& other) const noexcept;

 protected:
  id_t _id;
  osm2rdf::osm::generic::changeset_id_t _changeset;
  std::time_t _timestamp;
  std::string _user;
  osm2rdf::osm::generic::version_t _version;
  bool _visible;
  osm2rdf::osm::TagList _tags;
  std::vector<osm2rdf::osm::RelationMember> _members;
#if BOOST_VERSION >= 107800
  osm2rdf::geometry::Box _envelope;
  osm2rdf::geometry::Relation _geom;
  osm2rdf::geometry::Polygon _convexHull;
  osm2rdf::geometry::Polygon _obb;
#endif  // BOOST_VERSION >= 107800
  bool _hasCompleteGeometry;

  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
    ar& boost::serialization::make_nvp("_id", _id);
    ar& boost::serialization::make_nvp("_members", _members);
#if BOOST_VERSION >= 107800
    ar& boost::serialization::make_nvp("_envelope", _envelope);
    ar& boost::serialization::make_nvp("_geom", _geom);
    ar& boost::serialization::make_nvp("_convexHull", _envelope);
    ar& boost::serialization::make_nvp("_obb", _geom);
#endif  // BOOST_VERSION >= 107800
    ar& boost::serialization::make_nvp("_hasCompleteGeometry",
                                       _hasCompleteGeometry);
  }
};

}  // namespace osm2rdf::osm

#endif  // OSM2RDF_OSM_RELATION_H_
