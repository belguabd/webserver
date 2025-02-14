#pragma once
#include <httprequest.hpp>

enum Body
{
    boundary,
    chunked,
    contentLength,
    boundaryChunked
}

class Post
{
private:
    Body body_type;
public:

    // Post(map <key, value> headers);
    Post(/* args */);
    ~Post();
};

Post::Post(/* args */)
{
    
}

Post::~Post()
{
}
