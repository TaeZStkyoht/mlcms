#pragma once

#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

namespace middleware {
	template <typename StubType>
	class BaseGrpcClient {
	private:
		static grpc::ChannelArguments GetChannelArguments()
		{
			grpc::ChannelArguments channelArguments;
			channelArguments.SetInt(GRPC_ARG_MAX_CONCURRENT_STREAMS, INT_MAX);
			channelArguments.SetInt(GRPC_ARG_INITIAL_RECONNECT_BACKOFF_MS, 1000);
			channelArguments.SetInt(GRPC_ARG_MIN_RECONNECT_BACKOFF_MS, 100);
			channelArguments.SetInt(GRPC_ARG_MAX_RECONNECT_BACKOFF_MS, 500);
			return channelArguments;
		}

		std::string _url;
		std::shared_ptr<grpc::Channel> _channel;

	public:
		virtual ~BaseGrpcClient() = 0;

		[[nodiscard]] explicit BaseGrpcClient(std::string url)
			: _url(std::move(url)), _channel(CreateCustomChannel(_url, grpc::InsecureChannelCredentials(), GetChannelArguments())),
			  _stub(StubType::NewStub(_channel))
		{
		}

	protected:
		template <typename ResponseType>
		struct ResponseWithClientContext final {
			ResponseType response;
			grpc::ClientContext cc;
		};

		std::unique_ptr<typename StubType::Stub> _stub;
	};

	template <typename StubType>
	inline BaseGrpcClient<StubType>::~BaseGrpcClient() = default;
}
