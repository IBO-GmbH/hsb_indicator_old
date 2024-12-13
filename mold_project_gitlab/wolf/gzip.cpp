#include "gzip.hpp"

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filter/gzip.hpp>


using namespace wolf;


gzip::data gzip::compress(const gzip::data &to_compress)
{
    std::vector<char> result;
    {
        boost::iostreams::array_source input(reinterpret_cast<const char*>(to_compress.data()),
                                             to_compress.size());
        boost::iostreams::back_insert_device<decltype (result)> result_inserter(result);
        boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
        in.push(boost::iostreams::gzip_compressor());
        in.push(input);
        boost::iostreams::copy(in, result_inserter);
    }
    return data(reinterpret_cast<const data::value_type*>(result.data()),
                reinterpret_cast<const data::value_type*>(result.data()+result.size()));
}

gzip::data gzip::decompress(const gzip::data &to_decompress)
{
    std::vector<char> result;
    {
        using decompressor = boost::iostreams::gzip_decompressor;

        boost::iostreams::array_source input(reinterpret_cast<const char*>(to_decompress.data()),
                                             to_decompress.size());
        boost::iostreams::back_insert_device<decltype (result)> result_inserter(result);
        boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
        in.push(decompressor());
        in.push(input);
        boost::iostreams::copy(in, result_inserter);
    }
    return data(reinterpret_cast<const data::value_type*>(result.data()),
                reinterpret_cast<const data::value_type*>(result.data()+result.size()));
}
