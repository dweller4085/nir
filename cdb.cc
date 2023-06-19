#include "cdb.hh"

ClauseDB::~ClauseDB() {
    if (rows.matrix) free(rows.matrix);
    if (cols.matrix) free(cols.matrix);
}

ClauseDB& ClauseDB::operator = (ClauseDB&& other) noexcept {
    if (rows.matrix) free(rows.matrix);
    if (cols.matrix) free(cols.matrix);

    clauseCnt = other.clauseCnt;
    varCnt = other.varCnt;

    rows = other.rows;
    cols = other.cols;

    other.rows.matrix = nullptr;
    other.cols.matrix = nullptr;

    return *this;
}

ClauseDB::ClauseDB(std::string const& cnf, bool& ok): ClauseDB {} {
    auto static constexpr isWS = [](char const * s) -> bool {
        return {*s == ' ' || *s == '\n'};
    };
    auto static constexpr isNewLine = [](char const * s) -> bool {
        return *s == '\n';
    };
    auto static constexpr skipWS = [](char const *& s) {
        while (isWS(s)) s++;
    };
    auto static constexpr isDigit = [](char const * d) -> bool {
        return {*d >= 0x30 && *d < 0x3A};
    };
    auto static constexpr digit = [](char const *& d) -> int {
        return {*(d++) - 0x30};
    };
    auto static constexpr ternary = [](char const *& d) -> Ternary {
        Ternary val;
        switch (*(d++)) {
            case '0': val = False; break;
            case '1': val = True; break;
            case '-': val = Undef; break;
            default: break;
        }

        return val;
    };
    auto static constexpr integer = [](char const *& d) -> int {
        int s = 0;
        while (isDigit(d)) {
            s *= 10;
            s += digit(d);
        } return s;
    };

    char const * raw = cnf.c_str();

    {
        skipWS(raw);

        if (*raw == 'n') {
            varCnt = integer(raw += 2);
        }

        else {
            ok = false;
            return;
        } skipWS(raw);

        if (*raw == 'm') {
            clauseCnt = integer(raw += 2);
        }

        else {
            ok = false;
            return;
        } skipWS(raw);
    }
    

    cols.words = (clauseCnt - 1) / 32 + 1;
    rows.words = (varCnt - 1) / 32 + 1;

    cols.matrix = (u64 *) calloc((usize) cols.words * varCnt, sizeof(u64));
    rows.matrix = (u64 *) calloc((usize) rows.words * clauseCnt, sizeof(u64));
    

    u32 i = 0;
    while (*raw != '\0' && i < clauseCnt) {
        for (u32 j = 0; j < varCnt; j += 1) {
            auto value = ternary(raw);
            
            {
                u64 const k = (i % 32) * 2;
                u64 * const word = cols.matrix + cols.words * j + i / 32;
                *word &= ~(u64 {0b11} << k);
                *word |= (u64) value << k;
            }

            {
                u64 const k = (j % 32) * 2;
                u64 * const word = rows.matrix + rows.words * i + j / 32;
                *word &= ~(u64 {0b11} << k);
                *word |= (u64) value << k;
            }

        }
        
        skipWS(raw);
        i += 1;
    }

    if (i != clauseCnt) {
        ok = false;
    }

    else {
        ok = true;
    }
}
