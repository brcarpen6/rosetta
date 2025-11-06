// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/moves/MoverFactory.cc
/// @brief
/// @author brcarpen6

#include <protocols/bootcamp/BootCampMoverCreator.hh>
#include <protocols/bootcamp/BootCampMover.hh>
#include <protocols/moves/MoverFactory.hh>
#include <protocols/moves/MoverCreator.hh>
#include <utility/pointer/std/owning_ptr.hh>
#include <protocols/moves/MoverRegistrator.hh>

namespace protocols { namespace bootcamp {

protocols::moves::MoverOP BootCampMoverCreator::create_mover() const {
  return protocols::moves::MoverOP( new BootCampMover );
}

std::string BootCampMoverCreator::keyname() const {
  return BootCampMover::mover_name(); // "BootCampMover"
}

void BootCampMoverCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const {
  BootCampMover::provide_xml_schema( xsd );
}

}} // namespaces

// Register with the factory:

static protocols::moves::MoverRegistrator< protocols::bootcamp::BootCampMoverCreator > reg_bcm_;
