// Copyright 2020, University of Freiburg
// Authors: Axel Lehmann <lehmann@cs.uni-freiburg.de>.

#ifndef OSM2NT_NT_WRITER_H_
#define OSM2NT_NT_WRITER_H_

#include <fstream>
#include <ostream>
#include <string>

#include "osmium/geom/wkt.hpp"
#include "osmium/osm/area.hpp"
#include "osmium/osm/location.hpp"
#include "osmium/osm/node.hpp"
#include "osmium/osm/relation.hpp"
#include "osmium/osm/tag.hpp"
#include "osmium/osm/way.hpp"

#include "osm2nt/config/Config.h"

#include "osm2nt/nt/BlankNode.h"
#include "osm2nt/nt/IRI.h"
#include "osm2nt/nt/Literal.h"

#include "osm2nt/osm/SimplifyingWKTFactory.h"

namespace osm2nt {
namespace nt {

class Writer {
 public:
  explicit Writer(const osm2nt::config::Config& config);
  void writeHeader();

  // OSM
  void writeOsmArea(const osmium::Area& area);
  void writeOsmBox(const osm2nt::nt::BlankNode& s, const osm2nt::nt::IRI& p,
                   const osmium::Box& box);
  void writeOsmBox(const osm2nt::nt::IRI& s, const osm2nt::nt::IRI& p,
                   const osmium::Box& box);
  void writeOsmLocation(const osm2nt::nt::BlankNode& s,
                        const osmium::Location& l);
  void writeOsmLocation(const osm2nt::nt::IRI& s,
                        const osmium::Location& l);
  void writeOsmNode(const osmium::Node& node);
  void writeOsmRelation(const osmium::Relation& relation);
  void writeOsmRelationMembers(const osm2nt::nt::BlankNode& s,
                               const osmium::RelationMemberList& members);
  void writeOsmRelationMembers(const osm2nt::nt::IRI& s,
                               const osmium::RelationMemberList& members);
  void writeOsmTag(const osm2nt::nt::BlankNode& s, const osmium::Tag& tag);
  void writeOsmTag(const osm2nt::nt::IRI& s, const osmium::Tag& tag);
  void writeOsmTagList(const osm2nt::nt::BlankNode& s,
                       const osmium::TagList& tags);
  void writeOsmTagList(const osm2nt::nt::IRI& s,
                       const osmium::TagList& tags);
  void writeOsmWay(const osmium::Way& way);
  void writeOsmWayNodeList(const osm2nt::nt::BlankNode& s,
                           const osmium::WayNodeList& nodes);
  void writeOsmWayNodeList(const osm2nt::nt::IRI& s,
                           const osmium::WayNodeList& nodes);

 protected:
  // Triple
  void writeTriple(const osm2nt::nt::BlankNode& s, const osm2nt::nt::IRI& p,
                   const osm2nt::nt::IRI& o);
  void writeTriple(const osm2nt::nt::BlankNode& s, const osm2nt::nt::IRI& p,
                   const osm2nt::nt::BlankNode& o);
  void writeTriple(const osm2nt::nt::BlankNode& s, const osm2nt::nt::IRI& p,
                   const osm2nt::nt::Literal& o);
  void writeTriple(const osm2nt::nt::IRI& s, const osm2nt::nt::IRI& p,
                   const osm2nt::nt::IRI& o);
  void writeTriple(const osm2nt::nt::IRI& s, const osm2nt::nt::IRI& p,
                   const osm2nt::nt::BlankNode& o);
  void writeTriple(const osm2nt::nt::IRI& s, const osm2nt::nt::IRI& p,
                   const osm2nt::nt::Literal& o);
  // Element
  void writeBlankNode(const osm2nt::nt::BlankNode& b);
  void writeIRI(const osm2nt::nt::IRI& i);
  void writeLangTag(const osm2nt::nt::LangTag& l);
  void writeLiteral(const osm2nt::nt::Literal& l);

  static bool tagKeyEndsWith(const osmium::Tag& tag, const std::string& needle);
  static std::string urlencode(const std::string& s);

  osm2nt::config::Config _config;
  std::ostream* _out;
  std::ofstream _outFile;
  osmium::geom::WKTFactory<> _wktFactory;
  osm2nt::osm::SimplifyingWKTFactory<> _simplifyingWktFactory;
};

}  // namespace nt
}  // namespace osm2nt

#endif  // OSM2NT_NT_WRITER_H_
