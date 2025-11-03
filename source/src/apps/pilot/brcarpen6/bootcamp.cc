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
// Rosetta headers
#include <core/pose/Pose.hh>
#include <core/import_pose/import_pose.hh>
#include <core/types.hh>
#include <utility/pointer/owning_ptr.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>
#include <numeric/random/random.hh>
#include <protocols/moves/MonteCarlo.hh>


using namespace std;
using namespace core::import_pose;
using namespace core::pose;
using namespace core::scoring;
using namespace numeric::random;
using namespace protocols::moves;

static basic::Tracer TR( "bootcamp" );

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

    PoseOP mypose = pose_from_file(filenames[1]);

    ScoreFunctionOP sfxn = get_score_function();

    MonteCarlo mc(*mypose, *sfxn, 1.0);

    for(int i = 1; i <= 100; ++i){
        

        double uniform_random_number = uniform();
        core::Size N = mypose->size();
        core::Size randres = static_cast<core::Size> (uniform_random_number * N + 1);
        core::Real pert1 = gaussian();
        core::Real pert2 = gaussian();
        core::Real orig_phi = mypose->phi( randres );
        core::Real orig_psi = mypose->psi( randres );

        mypose->set_phi( randres, orig_phi + pert1);
        mypose->set_psi( randres, orig_psi + pert1);

        core::Real score = sfxn -> score ( *mypose );

        mc.boltzmann(*mypose);

        TR << "Cycle: " << i << " score: " << score << " best: " << mc.lowest_score() << endl;
    }

    




        

    

}