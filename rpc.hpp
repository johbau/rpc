#define rpc.hpp

#include <boost/interprocess/named_pipe.hpp>
#include <flatbuffers/flatbuffer.h>
#include <flatbuffers/flatbuffers.h>
#include "hello_generated.h"
#include <string>
#include <memory>

using namespace boost::interprocess;

namespace rpc {

class RpcClient {
public:
    explicit RpcClient(const std::string& serverName);
    
    ~RpcClient();
    
    template<typename RequestT, typename ResponseT>
    ResponseT sendRequest(const RequestT& request);
    
private:
    named_pipe_client requestPipe_;
    named_pipe_server responsePipe_;
    std::string serverName_;
};

} // namespace rpc

#endif // rpc.hpp
