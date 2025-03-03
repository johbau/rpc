#define rpc.hpp

#include <boost/interprocess/named_pipe.hpp>
#include <flatbuffers/flatbuffer.h>
#include <flatbuffers/flatbuffers.h>
#include <string>
#include <memory>

using namespace boost::interprocess;

namespace rpc {

class rpcClient {
public:
    explicit rpcClient(const std::string& serverName);
    
    ~rpcClient();
    
    template<typename RequestT, typename ResponseT>
    ResponseT sendRequest(const RequestT& request);
    
private:
    named_pipe_client requestPipe_;
    named_pipe_server responsePipe_;
    std::string serverName_;
};

} // namespace rpc

#endif // rpc.hpp
