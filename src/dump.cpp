// Dump event in different modes
//
// Created by Samvel Khalatyan, Aug 06, 2011
// Copyright 2011, All rights reserved

#include <iostream>

#include <boost/shared_ptr.hpp>

#include <TRandom3.h>

#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Muon.pb.h"
#include "bsm_input/interface/Utility.h"

using namespace std;

using boost::shared_ptr;

using namespace bsm;

void dumpOneLine(const Event &);
void dumpMedium(const Event &);
void dumpFull(const Event &);

int main(int argc, char *argv[])
{
    shared_ptr<TRandom> randomizer(new TRandom3());
    shared_ptr<Event> event(new Event());
    Event::Extra *extra = event->mutable_extra();
    extra->set_id(100);
    extra->set_lumi(50);
    extra->set_run(1000);

    for(int m = 0, muons = randomizer->Poisson(10); muons > m; ++m)
    {
        Muon *muon = event->add_pf_muons();
        LorentzVector *p4 = muon->mutable_physics_object()->mutable_p4();

        p4->set_e(randomizer->Gaus(.105, 0.005));
        p4->set_px(randomizer->Gaus(10, 3));
        p4->set_py(randomizer->Gaus(10, 2));
        p4->set_pz(randomizer->Gaus(10, 1));

        Vector *vertex = muon->mutable_physics_object()->mutable_vertex();

        vertex->set_x(randomizer->Gaus(1, 2));
        vertex->set_y(randomizer->Gaus(1, 3));
        vertex->set_z(randomizer->Gaus(1, 4));
    }

    dumpOneLine(*event);
    cout << endl;

    dumpMedium(*event);
    cout << endl;

    dumpFull(*event);

    return 1;
}

void dumpOneLine(const Event &event)
{
    bsm::ShortFormat format;

    cout << "Short Format" << endl;
    cout << format(event) << endl;
}

void dumpMedium(const Event &event)
{
    bsm::MediumFormat format;

    cout << "Medium Format" << endl;
    cout << format(event) << endl;
}

void dumpFull(const Event &event)
{
    bsm::FullFormat format;

    cout << "Full Format" << endl;
    cout << format(event) << endl;
}
