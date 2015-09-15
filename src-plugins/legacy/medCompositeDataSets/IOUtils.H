/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once


#include <iostream>
#include <string>
#include <limits>
#include <locale>

//  Various stream manipulators to handle various kinds of token
//  in the input stream (comments, strings, white spaces,
//  optional elements...).

namespace io_utils {

    //  Copy the imanip object from the old C++ library since it is apparently
    //  not in the standard. Remove it for a more standard solution ? (TODO).

    template <typename> class imanip;

    template <typename TP>
    inline std::istream& operator>>(std::istream& i,const imanip<TP>& m) { return (*m.func)(i,m.obj); }

    template <typename TP>
    class imanip {
        std::istream& (*func)(std::istream&, TP);
        TP obj;
    public:
        imanip(std::istream& (*f)(std::istream&, TP), TP a): func(f), obj(a) {}

        friend std::istream& operator>> <>(std::istream&,const imanip<TP>&);
    };

    template <typename,typename> class imanip2;

    template <typename T1,typename T2>
    inline std::istream& operator>>(std::istream& i,const imanip2<T1,T2>& m) { return (*m.func)(i,m.obj1,m.obj2); }

    template <typename T1,typename T2>
    class imanip2 {
        std::istream& (*func)(std::istream&,T1,T2);
        T1 obj1;
        T2 obj2;
    public:
        imanip2(std::istream& (*f)(std::istream&,T1,T2),T1 a,T2 b): func(f), obj1(a),obj2(b) {}
        friend std::istream& operator>> <>(std::istream&,const imanip2<T1,T2>&);
    };

    template <typename,typename,typename> class imanip3;

    template <typename T1,typename T2,typename T3>
    inline std::istream& operator>>(std::istream& i,const imanip3<T1,T2,T3>& m) { return (*m.func)(i,m.obj1,m.obj2,m.obj3); }

    template <typename T1,typename T2,typename T3>
    class imanip3 {
        std::istream& (*func)(std::istream&,T1,T2,T3);
        T1 obj1;
        T2 obj2;
        T3 obj3;
    public:
        imanip3(std::istream& (*f)(std::istream&,T1,T2,T3),T1 a,T2 b,T3 c): func(f), obj1(a),obj2(b),obj3(c) {}
        friend std::istream& operator>> <>(std::istream&,const imanip3<T1,T2,T3>&);
    };

    template <typename> class omanip;

    template <typename TP>
    inline std::ostream& operator<<(std::ostream& i,const omanip<TP>& m) { return (*m.func)(i,m.obj); }

    template <typename TP>
    class omanip {
        std::ostream& (*func)(std::ostream&,TP);
        TP obj;
    public:
        omanip(std::ostream& (*f)(std::ostream&,TP),TP a): func(f), obj(a) {}
        friend std::ostream& operator<< <>(std::ostream&,const omanip<TP>&);
    };

    template <typename,typename> class omanip2;

    template <typename T1,typename T2>
    inline std::ostream& operator<<(std::ostream& i,const omanip2<T1,T2>& m) { return (*m.func)(i,m.obj1,m.obj2); }

    template <typename T1,typename T2>
    class omanip2 {
        std::ostream& (*func)(std::ostream&,T1,T2);
        T1 obj1;
        T2 obj2;
    public:
        omanip2(std::ostream& (*f)(std::ostream&,T1,T2),T1 a,T2 b): func(f), obj1(a),obj2(b) {}
        friend std::ostream& operator<< <>(std::ostream& i,const omanip2<T1,T2>& m);
    };

	//  Ignoring the istream till the end of the line.

	inline std::istream&
	skip_line(std::istream& is) {
		return is.ignore(std::numeric_limits<int>::max(),'\n');
	}

	//	Eat pure white spaces (' ' and '\t')

	inline std::istream&
	skip_spaces(std::istream& is) {

		char separator;

		while (is.get(separator) && ((separator==' ') || (separator=='\t')));
		return is.putback(separator);
	}

	//  Eat up comments starting with the given character.
	//	Example: To remove the lines begining with a '#' then read data.
	//		cin >> skip_comments('#') >> data;


    inline std::istream&
    skip_lines_internal(std::istream& is,const unsigned char c) {
        is >> std::ws;
        while (is.peek()==c) {
            skip_line(is);
            is >> std::ws;
        }
        return is;
    }

	inline imanip<const unsigned char>
	skip_comments(const unsigned char c) {
		return imanip<const unsigned char>(skip_lines_internal,c);
	}

    //	Test if the input character stream is equal to s.

    inline bool
    match_string(std::istream& is,const char*& s)
    {
        is >> std::ws;
        while (*s && is.peek()==static_cast<const unsigned char>(*s)) {
            is.get();
            s++;
        }

        return *s=='\0';
    }

	//  Eat up comments beginning with the given string.
	//	Example: To remove the lines begining with a "//" then read data.
	//		cin >> skip_comments("//") >> data;

    inline std::istream&
    skip_lines_internal(std::istream& is,const char* s)
    {
        const char* s1 = s;
        while (match_string(is,s1)) {
            skip_line(is);
            s1 = s;
        }

        return is;
    }

	inline imanip<const char*>
	skip_comments(const char* s) {
		return imanip<const char*>(skip_lines_internal,s);
	}

	//	Match the given string with an input stream.
	//	Example: To parse lines such as "x=156 y=34".
	//		cin >> match("x=") >> x >> match("y=") >> y;

    inline std::istream&
    match_internal(std::istream& is,const char* s) {
        is >> std::ws;
        if (!match_string(is,s))
            is.setstate(std::ios::failbit);

        return is;
    }

    inline std::istream&
    match_internal(std::istream& is,const char c) {
        char cc;
        is >> cc;
        if (cc!=c)
            is.setstate(std::ios::failbit);

        return is;
    }

    inline std::istream&
    match_internal(std::istream& is,const int i) {
        int num;
        is >> num;
        if (i!=num)
            is.setstate(std::ios::failbit);

        return is;
    }

    inline std::istream&
    match_internal(std::istream& is,const unsigned i) {
        unsigned num;
        is >> num;
        if (i!=num)
            is.setstate(std::ios::failbit);

        return is;
    }

	inline imanip<const char*>
	match(const char* str) {
		return imanip<const char*>(match_internal,str);
	}

    inline imanip<const char*>
	match(const std::string& str) {
		return imanip<const char*>(match_internal,str.c_str());
	}

    inline imanip<const char>
    match(const char c) {
        return imanip<const char>(match_internal,c);
    }

    inline imanip<const int>
    match(const int i) {
        return imanip<const int>(match_internal,i);
    }

    inline imanip<const unsigned>
    match(const unsigned i) {
        return imanip<const unsigned>(match_internal,i);
    }

    //	Restore the string [s,s1[ to the input stream.

    inline void
    restore_string(std::istream& is,const char* s,const char *s1) {
        while (s!=s1--)
            is.putback(*s1);
    }

    //  The string equivalent.

    inline void
    restore_string(std::istream& is,const std::string& s) {
        restore_string(is,&*s.begin(),&*s.end());
    }

    //  Like match but the matching is optional and the
    //  presence of the string is given in result.

    inline std::istream&
    match_string_internal(std::istream& is,const char* s,bool& res)
    {
        std::ios::iostate state = is.rdstate();
        is >> std::ws;
        const char* s1 = s;
        res = match_string(is,s1);
        if (!res) {
            is.setstate(state);
            restore_string(is,s,s1);
        }

        return is;
    }

    inline imanip2<const char*,bool&>
    match_optional(const char* str,bool& res) {
        return imanip2<const char*,bool&>(match_string_internal,str,res);
    }

    inline imanip2<const char*,bool&>
    match_optional(const std::string& str,bool& res) {
        return imanip2<const char*,bool&>(match_string_internal,str.c_str(),res);
    }

    //	Eat the given string if present on an input stream.
    //	Example: To parse lines such as "x=156 y=34".
    //		cin >> eat("x=") >> x >> eat("y=") >> y;
    //	The difference with match is that it will silently
    //	ignore the strings if they are not on the stream
    //	and the stream will remain in the good state.
    //	So 156 34 will also be accepted as an input line.
    //	Note that x156 34 or any other variation will not
    //	be accepted.


    inline std::istream&
    eat_string_internal(std::istream& is,const char* s)
    {
        is >> std::ws;
        const char* s1 = s;
        if (!match_string(is,s1))
            restore_string(is,s,s1);

        return is;
    }

    inline imanip<const char*>
    eat(const char* str) {
        return imanip<const char*>(eat_string_internal,str);
    }

    inline imanip<const char*>
    eat(const std::string& str) {
        return imanip<const char*>(eat_string_internal,str.c_str());
    }

    //	Skip everything until string s is found on the stream.

    inline std::istream&
    skip_to_internal(std::istream& is,const char* s)
    {
        const char* s1 = s;

        while(is) {
            is.ignore(std::numeric_limits<int>::max(),*s);
            if (match_string(is,++s1))
                break;
            restore_string(is,s+1,s1);
            s1 = s;
        }

        return is;
    }

    inline imanip<const char*>
    skip_to(const char* str) {
        return imanip<const char*>(skip_to_internal,str);
    }

    inline imanip<const char*>
    skip_to(const std::string& str) {
        return imanip<const char*>(skip_to_internal,str.c_str());
    }

    //  An optional input is a field eventually put before the
    //  end of the line.

    template <typename T>
    inline std::istream&
    optional_internal(std::istream& is,T& t) {

        skip_spaces(is);
        if (is.peek()!='\n')
            is >> t;
        return is;
    }

    template <typename T>
    inline imanip<T&>
    optional(T& t) {
        return imanip<T&>(optional_internal,t);
    }

    //  Force numeric reading.

    template <typename T>
    inline std::istream&
    numeric_internal(std::istream& is,T& t) {
        return is >> t;
    }

    inline std::istream&
    numeric_internal(std::istream& is,char& t) {
        int i;
        is >> i;
        t = static_cast<char>(i);
        return is;
    }

    inline std::istream&
    numeric_internal(std::istream& is,unsigned char& t) {
        unsigned i;
        is >> i;
        t = static_cast<unsigned char>(i);
        return is;
    }

    template <typename T>
    inline imanip<T&>
    numeric(T& t) {
        return imanip<T&>(numeric_internal,t);
    }

    //  File reading.

    inline std::istream&
    file_internal(std::istream& is,std::string& filename,const char delimiter,const bool force_delimiter) {
        skip_spaces(is);

        //  Read the initial delimiter if necessary.

        char c;
        bool delimiter_seen = false;

        if (is.peek()!=delimiter) {
            if (force_delimiter) {
                is.setstate(std::ios::failbit);
                return is;
            }
        } else {
            is.get(c);
            delimiter_seen = true;
        }

        //  Read the string content.

        while (is.get(c) && c!=delimiter) {
            if (delimiter_seen && c==delimiter)
                break;
            if (!delimiter_seen && std::isspace(c,is.getloc()))
                break;
            filename += c;
        }

        return is;
    }

    inline imanip3<std::string&,const char,const bool>
    filename(std::string& filename,const char delimiter='"',const bool force_delimiter=true) {
        return imanip3<std::string&,const char,const bool>(file_internal,filename,delimiter,force_delimiter);
    }

}

