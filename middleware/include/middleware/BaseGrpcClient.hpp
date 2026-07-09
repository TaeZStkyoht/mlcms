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
			: _url(std::move(url)), _channel(CreateChannel(_url, grpc::InsecureChannelCredentials())), _stub(StubType::NewStub(_channel)),
			  _connected(_channel && _stub)
		{
		}

		void Stop()
		{
			const std::lock_guard lg(_mtxChannelStub);
			_connected = {};
			_stub.reset();
			_channel.reset();
		}

		void Connect()
		{
			if (const std::lock_guard lg(_mtxChannelStub); !_connected) {
				_channel = CreateChannel(_url, grpc::InsecureChannelCredentials());
				_stub = StubType::NewStub(_channel);
				_connected = _channel && _stub;
			}
		}

	protected:
		template <typename ResponseType>
		struct ResponseWithClientContext final {
			ResponseType response;
			grpc::ClientContext cc;
		};

		std::unique_ptr<typename StubType::Stub> _stub;
		std::atomic_bool _connected;
		std::mutex _mtxChannelStub;
	};

	template <typename StubType>
	inline BaseGrpcClient<StubType>::~BaseGrpcClient() = default;
}
