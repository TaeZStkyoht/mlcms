#pragma once

#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

namespace middleware {
	template <typename StubType>
	class BaseGrpcClient {
	private:
		std::string _url;
		std::shared_ptr<grpc::Channel> _channel;

	public:
		virtual ~BaseGrpcClient() = 0;

		[[nodiscard]] explicit BaseGrpcClient(std::string url)
			: _url(std::move(url)), _channel(CreateChannel(_url, grpc::InsecureChannelCredentials())), _stub(StubType::NewStub(_channel))
		{
		}

		void Reconnect()
		{
			_stub.reset();
			_channel.reset();
			_channel = CreateChannel(_url, grpc::InsecureChannelCredentials());
			_stub = StubType::NewStub(_channel);
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
