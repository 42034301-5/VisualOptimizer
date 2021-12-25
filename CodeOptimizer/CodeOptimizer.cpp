
#include "CodeOptimizer.h"
#include "global.hpp"

handler::handler()
{
    //ctor
}
handler::handler(utility::string_t url) :m_listener(url)
{
    m_listener.support(methods::GET, std::bind(&handler::handle_get, this, std::placeholders::_1));
    m_listener.support(methods::PUT, std::bind(&handler::handle_put, this, std::placeholders::_1));
    m_listener.support(methods::POST, std::bind(&handler::handle_post, this, std::placeholders::_1));
    m_listener.support(methods::DEL, std::bind(&handler::handle_delete, this, std::placeholders::_1));
    m_listener.support(methods::OPTIONS, std::bind(&handler::handle_options, this, std::placeholders::_1));
}
handler::~handler()
{
    //dtor
}

void handler::handle_options(http_request message)
{
    http_response rep;
    rep.headers().add(U("Access-Control-Allow-Origin"), message.headers().operator[](U("Origin")));
    rep.headers().add(U("Access-Control-Request-Method"), U("GET,POST,OPTIONS"));
    rep.headers().add(U("Access-Control-Allow-Credentials"), U("true"));
    rep.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type,Access-Token,x-requested-with,Authorization"));
    rep.set_status_code(status_codes::OK);
    message.reply(rep);
}

void handler::handle_error(pplx::task<void>& t)
{
    try
    {
        t.get();
    }
    catch (...)
    {
        // Ignore the error, Log it if a logger is available
    }
}

inline std::wstring StringToWString(const std::string& input)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(input);
}
 
inline std::string WStringToString(const std::wstring& input)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(input);
}





tuple<vector<ForeEndNode>, vector<ForeEndEdge>,string> initWithCode(const string& codes)
{
    vector<ForeEndNode> nodes;
    vector<ForeEndEdge> edges;

    nodes.push_back({ 1ull,"Code1\nemmm\nhhhhhhh\nwhat\nactive: a b c",0ull });
    nodes.push_back({ 2ull ,"Code2\nemmm\nhhhhhhh\nwhat\nactive: a b c",1ull });
    nodes.push_back({ 3ull,"Code3\nemmm\nhhhhhhh\nwhat\nactive: a b c",0ull });
    nodes.push_back({ 4ull,"Code4\nemmm\nhhhhhhh\nwhat\nactive: a b c",1ull });
    nodes.push_back({ 5ull,"Code5\nemmm\nhhhhhhh\nwhat\nactive: a b c",2ull });

    edges.push_back({1ull,1ull,2ull,0ull});
    edges.push_back({ 2ull,2ull,3ull,0ull });
    edges.push_back({ 3ull,1ull,3ull,1ull });
    edges.push_back({ 4ull,3ull,4ull,2ull, });
    edges.push_back({ 5ull,4ull,1ull,1ull });
    edges.push_back({ 6ull,4ull,5ull,3ull });
    return std::make_tuple(nodes,edges,string("empty"));
}

void recordActivate(const vector<string>&active)
{

}


tuple<vector<ForeEndNode>, vector<ForeEndEdge>> establishDAG(const string& code)
{
    vector<ForeEndNode> nodes;
    vector<ForeEndEdge> edges;

    nodes.push_back({ 1ull,"Code1",0ull });
    nodes.push_back({ 2ull ,"Code2",1ull });
    nodes.push_back({ 3ull,"Code3",0ull });
    nodes.push_back({ 4ull,"Code4",1ull });
    nodes.push_back({ 5ull,"Code5",2ull });

    edges.push_back({ 1ull,1ull,2ull,0ull });
    edges.push_back({ 2ull,2ull,3ull,0ull });
    edges.push_back({ 3ull,1ull,3ull,1ull });
    edges.push_back({ 4ull,3ull,4ull,2ull, });
    edges.push_back({ 5ull,4ull,1ull,1ull });

    return std::make_tuple(nodes, edges);
}

tuple<vector<ForeEndNode>, vector<ForeEndEdge>> simplifyDAG()
{
    vector<ForeEndNode> nodes;
    vector<ForeEndEdge> edges;

    nodes.push_back({ 1ull,"T1",0ull });
    nodes.push_back({ 2ull ,"T2",1ull });
    nodes.push_back({ 3ull,"T3",0ull });
    nodes.push_back({ 4ull,"T4",1ull });

    edges.push_back({ 1ull,1ull,2ull,0ull });
    edges.push_back({ 2ull,2ull,3ull,0ull });
    edges.push_back({ 3ull,1ull,3ull,1ull });
    edges.push_back({ 4ull,3ull,4ull,2ull, });

    return std::make_tuple(nodes, edges);
}


json::value handleInitWithCode(json::value codes)
{

    ucout << codes.to_string() << std::endl;


    auto code = codes.operator[](U("codes")).as_string();

    ucout << code << std::endl;
    string process = WStringToString(code);

    std::cout << process << std::endl;
    auto [vecnodes, vecedges,value] = initWithCode(process);

    json::value ret= json::value::object();
    json::value nodes = json::value::array();
    json::value edges = json::value::array();

    for (size_t i = 0; i < vecnodes.size(); i++)
    {
        json::value node= json::value::object();
        node.operator[](U("id")) = json::value::value(vecnodes[i].id);
        node.operator[](U("label")) = json::value::value(StringToWString(vecnodes[i].label));
        node.operator[](U("group")) = json::value::value(vecnodes[i].group);
        nodes[i] = node;
    }
    for (size_t i = 0; i < vecedges.size(); i++)
    {
        json::value edge= json::value::object();
        edge.operator[](U("id")) = json::value::value(vecedges[i].id);
        edge.operator[](U("from")) = json::value::value(vecedges[i].from);
        edge.operator[](U("to")) = json::value::value(vecedges[i].to);
        edge.operator[](U("group")) = json::value::value(vecedges[i].group);
        edges[i] = edge;
    }
    ret.operator[](U("nodes")) = nodes;
    ret.operator[](U("edges")) = edges;
    ret.operator[](U("value")) = json::value::value(StringToWString(value));

    ucout << ret.to_string() << std::endl;
    return ret;
}


//
// Get Request 
//
void handler::handle_get(http_request message)
{
    auto data = message.extract_json().get();
    auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    auto operation = *paths.rbegin();

    ucout << message.to_string() << std::endl;

    json::value ret;
    if (operation == utility::string_t(U("init")))
    {
        ret = handleInitWithCode(data);
    }

    http_response rep;
    rep.set_body(ret);
    rep.headers().add(U("Access-Control-Allow-Origin"), message.headers().operator[](U("Origin")));
    rep.headers().add(U("Access-Control-Request-Method"), U("GET,POST,OPTIONS"));
    rep.headers().add(U("Access-Control-Allow-Credentials"), U("true"));
    rep.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type,Access-Token,x-requested-with,Authorization"));
    rep.set_status_code(status_codes::OK);
    message.reply(rep);
    return;
};

//
// A POST request
//
void handler::handle_post(http_request message)
{
    auto data = message.extract_json().get();
    auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    auto operation = *paths.rbegin();

    ucout << message.to_string() << std::endl;

    json::value ret;
    if (operation == utility::string_t(U("init")))
    {
        ret = handleInitWithCode(data);
    }


    http_response rep;
    rep.set_body(ret);
    rep.headers().add(U("Access-Control-Allow-Origin"), message.headers().operator[](U("Origin")));
    rep.headers().add(U("Access-Control-Request-Method"), U("GET,POST,OPTIONS"));
    rep.headers().add(U("Access-Control-Allow-Credentials"), U("true"));
    rep.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type,Access-Token,x-requested-with,Authorization"));
    rep.set_status_code(status_codes::OK);
    message.reply(rep);
    return;
};

//
// A DELETE request
//
void handler::handle_delete(http_request message)
{
    ucout << message.to_string() << endl;

    string_t rep = U("WRITE YOUR OWN DELETE OPERATION");
    message.reply(status_codes::OK, rep);
    return;
};


//
// A PUT request 
//
void handler::handle_put(http_request message)
{
    ucout << message.to_string() << endl;
    string_t rep = U("WRITE YOUR OWN PUT OPERATION");
    message.reply(status_codes::OK, rep);
    return;
};

