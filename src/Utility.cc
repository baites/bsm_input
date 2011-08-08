// Utility methods
//
// Created by Samvel Khalatyan, Jun 01, 2011
// Copyright 2011, All rights reserved

#include <ostream>
#include <iomanip>

#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Electron.pb.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Muon.pb.h"
#include "bsm_input/interface/Physics.pb.h"
#include "bsm_input/interface/PrimaryVertex.pb.h"
#include "bsm_input/interface/Utility.h"

using namespace std;

using bsm::Event;
using bsm::EventSearcher;
using bsm::Format;
using bsm::ShortFormat;
using bsm::MediumFormat;
using bsm::FullFormat;

// Event Searcher
//
EventSearcher::EventSearcher(const Event::Extra &extra):
    _extra(extra)
{
}

bool EventSearcher::operator()(const Event::Extra &extra)
{
    return (extra.run() ? extra.run() == _extra.run() : true)
        && (extra.lumi() ? extra.lumi() == _extra.lumi() : true)
        && extra.id() == _extra.id();
}



// Format
//
Format::~Format()
{
}



// Event Format
//
/*
Format::EventFormat::EventFormat(const Format &format, const Event &event):
    _format(format),
    _event(event)
{
}

const Format &Format::EventFormat::format() const
{
    return _format;
}

const Event &Format::EventFormat::event() const
{
    return _event;
}
*/



// Short Format
//
ShortFormat::ShortFormat()
{
}

ShortFormat::~ShortFormat()
{
}

ostream &ShortFormat::write(ostream &out, const Electron &electron) const
{
    return out << electron.physics_object().p4();
}

ostream &ShortFormat::write(ostream &out, const Event &event) const
{
    if (event.has_extra())
    {
        const Event::Extra &extra = event.extra();

        out << "r: " << extra.run() << " l: " << extra.lumi() << " e: " << extra.id();
    }
    else
    {
        out << "event extra info is not available";
    }

    return  out;
}

ostream &ShortFormat::write(ostream &out, const Jet &jet) const
{
    return out << jet.physics_object().p4();
}

ostream &ShortFormat::write(ostream &out, const GenParticle &gen_particle) const
{
    return out << gen_particle.physics_object().p4();
}

ostream &ShortFormat::write(ostream &out, const MissingEnergy &met) const
{
    return out << met.p4();
}

ostream &ShortFormat::write(ostream &out, const Muon &muon) const
{
    return out << muon.physics_object().p4();
}

ostream &ShortFormat::write(ostream &out,
        const PrimaryVertex &primary_vertex) const
{
    return out << primary_vertex.vertex();
}



// Medium Format
//
MediumFormat::MediumFormat()
{
}

MediumFormat::~MediumFormat()
{
}

ostream &MediumFormat::write(ostream &out, const Electron &electron) const
{
    ShortFormat::write(out, electron) << endl;
    return out << electron.physics_object().vertex();
}

ostream &MediumFormat::write(ostream &out, const Event &event) const
{
    ShortFormat::write(out, event) << endl;

    out << event.primary_vertices().size() << " primary vertices" << endl;
    typedef ::google::protobuf::RepeatedPtrField<PrimaryVertex> PrimaryVertices;

    for(PrimaryVertices::const_iterator primary_vertex =
                event.primary_vertices().begin();
            event.primary_vertices().end() != primary_vertex;
            ++primary_vertex)
    {
        write(out, *primary_vertex) << endl;
    }

    out << endl;

    out << event.pf_muons().size() << " particle flow muons" << endl;
    typedef ::google::protobuf::RepeatedPtrField<Muon> Muons;

    for(Muons::const_iterator muon = event.pf_muons().begin();
            event.pf_muons().end() != muon;
            ++muon)
    {
        write(out, *muon) << endl
            << " ---" << endl;
    }

    out << endl;

    out << event.pf_electrons().size() << " particle flow electrons" << endl;
    typedef ::google::protobuf::RepeatedPtrField<Electron> Electrons;

    for(Electrons::const_iterator electron = event.pf_electrons().begin();
            event.pf_electrons().end() != electron;
            ++electron)
    {
        write(out, *electron) << endl
            << " ---" << endl;
    }

    return  out;
}

ostream &MediumFormat::write(ostream &out, const Jet &jet) const
{
    ShortFormat::write(out, jet) << endl;
    return out << jet.children().size() << " constituents";
}

ostream &MediumFormat::write(ostream &out, const GenParticle &particle) const
{
    ShortFormat::write(out, particle) << endl;
    return out << "id: " << particle.id()
        << " status: " << particle.status()
        << " " << particle.children().size() << " children";
}

ostream &MediumFormat::write(ostream &out, const MissingEnergy &met) const
{
    return ShortFormat::write(out, met);
}

ostream &MediumFormat::write(ostream &out, const Muon &muon) const
{
    ShortFormat::write(out, muon) << endl;
    return out << muon.physics_object().vertex();
}

ostream &MediumFormat::write(ostream &out,
        const PrimaryVertex &primary_vertex) const
{
    return ShortFormat::write(out, primary_vertex);
}



// Full Format
//
FullFormat::FullFormat()
{
}

FullFormat::~FullFormat()
{
}

ostream &FullFormat::write(ostream &out, const Electron &elecron) const
{
    MediumFormat::write(out, elecron) << endl;

    if (elecron.has_extra())
    {
        const Electron::Extra &extra = elecron.extra();
        out << " d0: " << extra.d0_bsp();
    }
    else
    {
        out << "no extra info is available";
    }

    return out;
}

ostream &FullFormat::write(ostream &out, const Event &event) const
{
    return MediumFormat::write(out, event) << endl;
}

ostream &FullFormat::write(ostream &out, const Jet &jet) const
{
    MediumFormat::write(out, jet) << endl;

    return out;
}

ostream &FullFormat::write(ostream &out, const GenParticle &particle) const
{
    MediumFormat::write(out, particle) << endl;

    return out;
}

ostream &FullFormat::write(ostream &out, const MissingEnergy &met) const
{
    return MediumFormat::write(out, met);
}

ostream &FullFormat::write(ostream &out, const Muon &muon) const
{
    MediumFormat::write(out, muon) << endl;

    if (muon.has_extra())
    {
        const Muon::Extra &extra = muon.extra();
        out << "global: " << boolalpha << extra.is_global()
            << " tracker: " << boolalpha << extra.is_tracker()
            << " d0: " << extra.d0_bsp();
    }
    else
    {
        out << "no extra info is available";
    }

    return out;
}

ostream &FullFormat::write(ostream &out,
        const PrimaryVertex &primary_vertex) const
{
    MediumFormat::write(out, primary_vertex) << endl;

    if (primary_vertex.has_extra())
    {
        const PrimaryVertex::Extra &extra = primary_vertex.extra();
        out << "rho: " << extra.rho() << " ndof: " << extra.ndof();
    }
    else
    {
        out << "no extra info available";
    }

    return out;
}



// Helpers
//
ostream &bsm::operator <<(ostream &out, const LorentzVector &p4)
{
    out << "pt: " << pt(p4)
        << " eta: " << eta(p4)
        << " phi: " << phi(p4);

    return out;
}

ostream &bsm::operator <<(ostream &out, const Vector &v)
{
    out << "x: " << v.x()
        << " y: " << v.y()
        << " z: " << v.z();

    return out;
}
