// Utility methods
//
// Created by Samvel Khalatyan, Jun 01, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_IO_UTILITY
#define BSM_IO_UTILITY

#include <iosfwd>

#include "bsm_input/interface/bsm_input_fwd.h"

namespace bsm
{
    class EventSearcher
    {
        public:
            EventSearcher(const Event_Extra &extra);

            bool operator()(const Event_Extra &extra);

        private:
            const Event_Extra &_extra;
    };

    class Format;

    template<typename T>
        class FormatObject
    {
        public:
            const Format &format() const;
            const T &object() const;

        private:
            friend class Format;

            FormatObject(const Format &, const T &object);

            const Format &_format;
            const T &_object;
    };

    class Format
    {
        public:
            virtual ~Format();

            template<typename T>
                FormatObject<T> operator()(const T &t) const;

            virtual std::ostream &write(std::ostream &,
                    const Electron &) const = 0;

            virtual std::ostream &write(std::ostream &,
                    const Event &) const = 0;

            virtual std::ostream &write(std::ostream &,
                    const Jet &) const = 0;

            virtual std::ostream &write(std::ostream &,
                    const GenParticle &,
                    const uint32_t &level = 0) const = 0;

            virtual std::ostream &write(std::ostream &,
                    const MissingEnergy &) const = 0;

            virtual std::ostream &write(std::ostream &,
                    const Muon &) const = 0;

            virtual std::ostream &write(std::ostream &,
                    const PrimaryVertex &) const = 0;
    };

    class ShortFormat: public Format
    {
        public:
            ShortFormat();
            virtual ~ShortFormat();

            virtual std::ostream &write(std::ostream &,
                    const Electron &) const;

            virtual std::ostream &write(std::ostream &,
                    const Event &) const;

            virtual std::ostream &write(std::ostream &,
                    const Jet &) const;

            virtual std::ostream &write(std::ostream &,
                    const GenParticle &,
                    const uint32_t &level = 0) const;

            virtual std::ostream &write(std::ostream &,
                    const MissingEnergy &) const;

            virtual std::ostream &write(std::ostream &,
                    const Muon &) const;

            virtual std::ostream &write(std::ostream &,
                    const PrimaryVertex &) const;
    };

    class MediumFormat: public ShortFormat
    {
        public:
            MediumFormat();
            virtual ~MediumFormat();

            virtual std::ostream &write(std::ostream &,
                    const Electron &) const;

            virtual std::ostream &write(std::ostream &,
                    const Event &) const;

            virtual std::ostream &write(std::ostream &,
                    const Jet &) const;

            virtual std::ostream &write(std::ostream &,
                    const GenParticle &,
                    const uint32_t &level = 0) const;

            virtual std::ostream &write(std::ostream &,
                    const MissingEnergy &) const;

            virtual std::ostream &write(std::ostream &,
                    const Muon &) const;

            virtual std::ostream &write(std::ostream &,
                    const PrimaryVertex &) const;
    };

    class FullFormat: public MediumFormat
    {
        public:
            FullFormat();
            virtual ~FullFormat();

            virtual std::ostream &write(std::ostream &,
                    const Electron &) const;

            virtual std::ostream &write(std::ostream &,
                    const Event &) const;

            virtual std::ostream &write(std::ostream &,
                    const Jet &) const;

            virtual std::ostream &write(std::ostream &,
                    const MissingEnergy &) const;

            virtual std::ostream &write(std::ostream &,
                    const Muon &) const;

            virtual std::ostream &write(std::ostream &,
                    const PrimaryVertex &) const;
    };

    // Helpers
    //
    std::ostream &operator <<(std::ostream &, const LorentzVector &);
    std::ostream &operator <<(std::ostream &, const Vector &);

    template<typename T>
    std::ostream &operator <<(std::ostream &out,
            const FormatObject<T> &format_object)
    {
        return format_object.format().write(out, format_object.object());
    }
}

template<typename T>
    bsm::FormatObject<T>::FormatObject(const Format &format, const T &object):
        _format(format),
        _object(object)
    {
    }

template<typename T>
    const bsm::Format &bsm::FormatObject<T>::format() const
{
    return _format;
}

template<typename T>
    const T &bsm::FormatObject<T>::object() const
{
    return _object;
}

template<typename T>
    bsm::FormatObject<T> bsm::Format::operator()(const T &t) const
{
    return FormatObject<T>(*this, t);
}

#endif
