// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

#include <iostream>
// devel headers
#include <devel/init.hh>

// protocol headers
#include <protocols/jd2/JobDistributor.hh>
// #include <protocols/bootcamp/BootcampMover.hh>

// utility headers
#include <utility/excn/Exceptions.hh>

// basic headers
#include <basic/Tracer.hh>
#include <basic/options/keys/in.OptionKeys.gen.hh>
#include <basic/options/option.hh>
#include <basic/options/keys/OptionKeys.hh>
#include <utility/options/OptionCollection.hh>
#include <basic/options/option_macros.hh>
using namespace std;

int main(int argc, char ** argv) {
    cout << "Hello world!!" << endl;

    devel::init (argc, argv);

    utility::vector1<string> filenames = basic::options::option[
    basic::options::OptionKeys::in::file::s ].value();

    if (filenames.size() > 0) {
        cout << "You entered: " << filenames[ 1 ] << "as the PDB file to be read" << endl;   
    } else{
        cout << "You didn't provide a PDB file with the -in::file::s option" << endl;
        return 1;
    }

        

    

}