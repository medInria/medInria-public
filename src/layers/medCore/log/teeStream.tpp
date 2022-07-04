#include <streambuf>

template <typename char_type, typename traits = std::char_traits<char_type> >
class basic_teebuf : public std::basic_streambuf<char_type, traits>
{
public:
    using int_type = traits::int_type;
    using streambuf = std::basic_streambuf<char_type, traits>;

    basic_teebuf(streambuf * sb1, streambuf * sb2) : m_sb1(sb1) , m_sb2(sb2) { }

private:
    // Sync both teed buffers.
    virtual int sync()
    {
        int const r1 = m_sb1->pubsync();
        int const r2 = m_sb2->pubsync();
        return r1 == 0 && r2 == 0 ? 0 : -1;
    }

    // This tee buffer has no buffer. So every character "overflows"
    // and can be put directly into the teed buffers.
    virtual int_type overflow(int_type c)
    {
        int_type itypeRes;

        int_type const eof = traits::eof();

        if (traits::eq_int_type(c, eof))
        {
            itypeRes = traits::not_eof(c);
        }
        else
        {
            char_type const ch = traits::to_char_type(c);
            int_type const r1 = m_sb1->sputc(ch);
            int_type const r2 = m_sb2->sputc(ch);

            itypeRes = traits::eq_int_type(r1, eof) || traits::eq_int_type(r2, eof) ? eof : c;
        }

        return itypeRes;
    }

private:
    streambuf * m_sb1;
    streambuf * m_sb2;
};


template <typename char_type, typename traits = std::char_traits<char_type> >
class basic_teestream : public std::ostream
{
public:
    // Construct an ostream which tees output to the supplied
    // ostreams.
    basic_teestream(std::ostream & o1, std::ostream & o2) : std::ostream(&tbuf), tbuf(o1.rdbuf(), o2.rdbuf()) {}
private:
    basic_teebuf<char_type, traits> tbuf;
};

typedef basic_teebuf<char> teebuf;
typedef basic_teestream<char> teestream;