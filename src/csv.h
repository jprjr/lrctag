#ifndef LRCTAG_CSV_INCLUDE_GUARD
#define LRCTAG_CSV_INCLUDE_GUARD

#include <iostream>
#include <string>

namespace LrcTag {
    class CSV {
        public:
        static inline void escape(std::ostream& out, const std::string& s) {
            std::string::size_type ppos = 0;
            std::string::size_type pos = 0;
            out << '"';
            while( (pos = s.find('"', ppos)) != std::string::npos) {
                out << s.substr(ppos, pos - ppos + 1);
                out << '"';
                ppos = pos + 1;
            }
            out << s.substr(ppos, s.length() - ppos);
            out << '"';
        }

    };
}

#endif
