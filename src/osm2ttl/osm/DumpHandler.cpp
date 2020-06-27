// Copyright 2020, University of Freiburg
// Authors: Axel Lehmann <lehmann@cs.uni-freiburg.de>.

#include "osm2ttl/osm/DumpHandler.h"

#include <iostream>
#include <ostream>
#include <string>

#include "osmium/osm/area.hpp"
#include "osmium/osm/node.hpp"
#include "osmium/osm/relation.hpp"
#include "osmium/osm/tag.hpp"
#include "osmium/osm/way.hpp"

#include "osm2ttl/config/Config.h"

#include "osm2ttl/ttl/Writer.h"

// ____________________________________________________________________________
osm2ttl::osm::DumpHandler::DumpHandler(const osm2ttl::config::Config& config,
  osm2ttl::ttl::Writer* writer, osm2ttl::osm::AreaHandler* areaHandler,
  osm2ttl::osm::MembershipHandler* membershipHandler) :
  _config(config), _writer(writer), _areaHandler(areaHandler),
  _membershipHandler(membershipHandler) {
}

// ____________________________________________________________________________
void osm2ttl::osm::DumpHandler::area(const osmium::Area& area) {
  if (_config.noAreaDump) {
    return;
  }
  if (!_config.addUnnamed && area.tags()["name"] == nullptr) {
    return;
  }
  _writer->writeOsmiumArea(area);
  _writer->writeOSM2TTLArea(osm2ttl::osm::Area(area));
}

// ____________________________________________________________________________
void osm2ttl::osm::DumpHandler::node(const osmium::Node& node) {
  if (_config.noNodeDump) {
    return;
  }
  if (!_config.addMemberNodes &&
      (_membershipHandler->isNodeMemberOfAnyRelation(node) ||
       _membershipHandler->isNodeMemberOfAnyWay(node))) {
    return;
  }
  if (!_config.addUnnamed && node.tags()["name"] == nullptr) {
    return;
  }
  _writer->writeOsmiumNode(node);
}

// ____________________________________________________________________________
void osm2ttl::osm::DumpHandler::relation(const osmium::Relation& relation) {
  if (_config.noRelationDump) {
    return;
  }
  if (!_config.addAreaSources &&
      _membershipHandler->isRelationMemberOfAnyArea(relation)) {
    return;
  }
  if (!_config.addUnnamed && relation.tags()["name"] == nullptr) {
    return;
  }
  _writer->writeOsmiumRelation(relation);
}

// ____________________________________________________________________________
void osm2ttl::osm::DumpHandler::way(const osmium::Way& way) {
  if (_config.noWayDump) {
    return;
  }
  if (!_config.addAreaSources &&
      _membershipHandler->isWayMemberOfAnyArea(way)) {
    return;
  }
  if (!_config.addUnnamed && way.tags()["name"] == nullptr) {
    return;
  }
  _writer->writeOsmiumWay(way);
}