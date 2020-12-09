// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: FileExchange.proto
#ifndef GRPC_FileExchange_2eproto__INCLUDED
#define GRPC_FileExchange_2eproto__INCLUDED

#include "FileExchange.pb.h"

#include <functional>
#include <grpc/impl/codegen/port_platform.h>
#include <grpcpp/impl/codegen/async_generic_service.h>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/client_context.h>
#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/stub_options.h>
#include <grpcpp/impl/codegen/sync_stream.h>

namespace FileExchange {

class SimpleFileServer final {
 public:
  static constexpr char const* service_full_name() {
    return "FileExchange.SimpleFileServer";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    std::unique_ptr< ::grpc::ClientWriterInterface< ::FileExchange::UploadRequest>> Upload(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response) {
      return std::unique_ptr< ::grpc::ClientWriterInterface< ::FileExchange::UploadRequest>>(UploadRaw(context, response));
    }
    std::unique_ptr< ::grpc::ClientAsyncWriterInterface< ::FileExchange::UploadRequest>> AsyncUpload(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncWriterInterface< ::FileExchange::UploadRequest>>(AsyncUploadRaw(context, response, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncWriterInterface< ::FileExchange::UploadRequest>> PrepareAsyncUpload(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncWriterInterface< ::FileExchange::UploadRequest>>(PrepareAsyncUploadRaw(context, response, cq));
    }
    std::unique_ptr< ::grpc::ClientReaderInterface< ::FileExchange::DownloadResponse>> Download(::grpc::ClientContext* context, const ::FileExchange::DownloadRequest& request) {
      return std::unique_ptr< ::grpc::ClientReaderInterface< ::FileExchange::DownloadResponse>>(DownloadRaw(context, request));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::FileExchange::DownloadResponse>> AsyncDownload(::grpc::ClientContext* context, const ::FileExchange::DownloadRequest& request, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::FileExchange::DownloadResponse>>(AsyncDownloadRaw(context, request, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::FileExchange::DownloadResponse>> PrepareAsyncDownload(::grpc::ClientContext* context, const ::FileExchange::DownloadRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::FileExchange::DownloadResponse>>(PrepareAsyncDownloadRaw(context, request, cq));
    }
    class experimental_async_interface {
     public:
      virtual ~experimental_async_interface() {}
      #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
      virtual void Upload(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response, ::grpc::ClientWriteReactor< ::FileExchange::UploadRequest>* reactor) = 0;
      #else
      virtual void Upload(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response, ::grpc::experimental::ClientWriteReactor< ::FileExchange::UploadRequest>* reactor) = 0;
      #endif
      #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
      virtual void Download(::grpc::ClientContext* context, ::FileExchange::DownloadRequest* request, ::grpc::ClientReadReactor< ::FileExchange::DownloadResponse>* reactor) = 0;
      #else
      virtual void Download(::grpc::ClientContext* context, ::FileExchange::DownloadRequest* request, ::grpc::experimental::ClientReadReactor< ::FileExchange::DownloadResponse>* reactor) = 0;
      #endif
    };
    #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
    typedef class experimental_async_interface async_interface;
    #endif
    #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
    async_interface* async() { return experimental_async(); }
    #endif
    virtual class experimental_async_interface* experimental_async() { return nullptr; }
  private:
    virtual ::grpc::ClientWriterInterface< ::FileExchange::UploadRequest>* UploadRaw(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response) = 0;
    virtual ::grpc::ClientAsyncWriterInterface< ::FileExchange::UploadRequest>* AsyncUploadRaw(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncWriterInterface< ::FileExchange::UploadRequest>* PrepareAsyncUploadRaw(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientReaderInterface< ::FileExchange::DownloadResponse>* DownloadRaw(::grpc::ClientContext* context, const ::FileExchange::DownloadRequest& request) = 0;
    virtual ::grpc::ClientAsyncReaderInterface< ::FileExchange::DownloadResponse>* AsyncDownloadRaw(::grpc::ClientContext* context, const ::FileExchange::DownloadRequest& request, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncReaderInterface< ::FileExchange::DownloadResponse>* PrepareAsyncDownloadRaw(::grpc::ClientContext* context, const ::FileExchange::DownloadRequest& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel);
    std::unique_ptr< ::grpc::ClientWriter< ::FileExchange::UploadRequest>> Upload(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response) {
      return std::unique_ptr< ::grpc::ClientWriter< ::FileExchange::UploadRequest>>(UploadRaw(context, response));
    }
    std::unique_ptr< ::grpc::ClientAsyncWriter< ::FileExchange::UploadRequest>> AsyncUpload(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncWriter< ::FileExchange::UploadRequest>>(AsyncUploadRaw(context, response, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncWriter< ::FileExchange::UploadRequest>> PrepareAsyncUpload(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncWriter< ::FileExchange::UploadRequest>>(PrepareAsyncUploadRaw(context, response, cq));
    }
    std::unique_ptr< ::grpc::ClientReader< ::FileExchange::DownloadResponse>> Download(::grpc::ClientContext* context, const ::FileExchange::DownloadRequest& request) {
      return std::unique_ptr< ::grpc::ClientReader< ::FileExchange::DownloadResponse>>(DownloadRaw(context, request));
    }
    std::unique_ptr< ::grpc::ClientAsyncReader< ::FileExchange::DownloadResponse>> AsyncDownload(::grpc::ClientContext* context, const ::FileExchange::DownloadRequest& request, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReader< ::FileExchange::DownloadResponse>>(AsyncDownloadRaw(context, request, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncReader< ::FileExchange::DownloadResponse>> PrepareAsyncDownload(::grpc::ClientContext* context, const ::FileExchange::DownloadRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReader< ::FileExchange::DownloadResponse>>(PrepareAsyncDownloadRaw(context, request, cq));
    }
    class experimental_async final :
      public StubInterface::experimental_async_interface {
     public:
      #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
      void Upload(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response, ::grpc::ClientWriteReactor< ::FileExchange::UploadRequest>* reactor) override;
      #else
      void Upload(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response, ::grpc::experimental::ClientWriteReactor< ::FileExchange::UploadRequest>* reactor) override;
      #endif
      #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
      void Download(::grpc::ClientContext* context, ::FileExchange::DownloadRequest* request, ::grpc::ClientReadReactor< ::FileExchange::DownloadResponse>* reactor) override;
      #else
      void Download(::grpc::ClientContext* context, ::FileExchange::DownloadRequest* request, ::grpc::experimental::ClientReadReactor< ::FileExchange::DownloadResponse>* reactor) override;
      #endif
     private:
      friend class Stub;
      explicit experimental_async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class experimental_async_interface* experimental_async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class experimental_async async_stub_{this};
    ::grpc::ClientWriter< ::FileExchange::UploadRequest>* UploadRaw(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response) override;
    ::grpc::ClientAsyncWriter< ::FileExchange::UploadRequest>* AsyncUploadRaw(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response, ::grpc::CompletionQueue* cq, void* tag) override;
    ::grpc::ClientAsyncWriter< ::FileExchange::UploadRequest>* PrepareAsyncUploadRaw(::grpc::ClientContext* context, ::FileExchange::UploadResponse* response, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientReader< ::FileExchange::DownloadResponse>* DownloadRaw(::grpc::ClientContext* context, const ::FileExchange::DownloadRequest& request) override;
    ::grpc::ClientAsyncReader< ::FileExchange::DownloadResponse>* AsyncDownloadRaw(::grpc::ClientContext* context, const ::FileExchange::DownloadRequest& request, ::grpc::CompletionQueue* cq, void* tag) override;
    ::grpc::ClientAsyncReader< ::FileExchange::DownloadResponse>* PrepareAsyncDownloadRaw(::grpc::ClientContext* context, const ::FileExchange::DownloadRequest& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_Upload_;
    const ::grpc::internal::RpcMethod rpcmethod_Download_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status Upload(::grpc::ServerContext* context, ::grpc::ServerReader< ::FileExchange::UploadRequest>* reader, ::FileExchange::UploadResponse* response);
    virtual ::grpc::Status Download(::grpc::ServerContext* context, const ::FileExchange::DownloadRequest* request, ::grpc::ServerWriter< ::FileExchange::DownloadResponse>* writer);
  };
  template <class BaseClass>
  class WithAsyncMethod_Upload : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_Upload() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_Upload() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Upload(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::FileExchange::UploadRequest>* /*reader*/, ::FileExchange::UploadResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestUpload(::grpc::ServerContext* context, ::grpc::ServerAsyncReader< ::FileExchange::UploadResponse, ::FileExchange::UploadRequest>* reader, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncClientStreaming(0, context, reader, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_Download : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_Download() {
      ::grpc::Service::MarkMethodAsync(1);
    }
    ~WithAsyncMethod_Download() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Download(::grpc::ServerContext* /*context*/, const ::FileExchange::DownloadRequest* /*request*/, ::grpc::ServerWriter< ::FileExchange::DownloadResponse>* /*writer*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestDownload(::grpc::ServerContext* context, ::FileExchange::DownloadRequest* request, ::grpc::ServerAsyncWriter< ::FileExchange::DownloadResponse>* writer, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncServerStreaming(1, context, request, writer, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_Upload<WithAsyncMethod_Download<Service > > AsyncService;
  template <class BaseClass>
  class ExperimentalWithCallbackMethod_Upload : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    ExperimentalWithCallbackMethod_Upload() {
    #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
      ::grpc::Service::
    #else
      ::grpc::Service::experimental().
    #endif
        MarkMethodCallback(0,
          new ::grpc::internal::CallbackClientStreamingHandler< ::FileExchange::UploadRequest, ::FileExchange::UploadResponse>(
            [this](
    #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
                   ::grpc::CallbackServerContext*
    #else
                   ::grpc::experimental::CallbackServerContext*
    #endif
                     context, ::FileExchange::UploadResponse* response) { return this->Upload(context, response); }));
    }
    ~ExperimentalWithCallbackMethod_Upload() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Upload(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::FileExchange::UploadRequest>* /*reader*/, ::FileExchange::UploadResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
    virtual ::grpc::ServerReadReactor< ::FileExchange::UploadRequest>* Upload(
      ::grpc::CallbackServerContext* /*context*/, ::FileExchange::UploadResponse* /*response*/)
    #else
    virtual ::grpc::experimental::ServerReadReactor< ::FileExchange::UploadRequest>* Upload(
      ::grpc::experimental::CallbackServerContext* /*context*/, ::FileExchange::UploadResponse* /*response*/)
    #endif
      { return nullptr; }
  };
  template <class BaseClass>
  class ExperimentalWithCallbackMethod_Download : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    ExperimentalWithCallbackMethod_Download() {
    #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
      ::grpc::Service::
    #else
      ::grpc::Service::experimental().
    #endif
        MarkMethodCallback(1,
          new ::grpc::internal::CallbackServerStreamingHandler< ::FileExchange::DownloadRequest, ::FileExchange::DownloadResponse>(
            [this](
    #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
                   ::grpc::CallbackServerContext*
    #else
                   ::grpc::experimental::CallbackServerContext*
    #endif
                     context, const ::FileExchange::DownloadRequest* request) { return this->Download(context, request); }));
    }
    ~ExperimentalWithCallbackMethod_Download() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Download(::grpc::ServerContext* /*context*/, const ::FileExchange::DownloadRequest* /*request*/, ::grpc::ServerWriter< ::FileExchange::DownloadResponse>* /*writer*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
    virtual ::grpc::ServerWriteReactor< ::FileExchange::DownloadResponse>* Download(
      ::grpc::CallbackServerContext* /*context*/, const ::FileExchange::DownloadRequest* /*request*/)
    #else
    virtual ::grpc::experimental::ServerWriteReactor< ::FileExchange::DownloadResponse>* Download(
      ::grpc::experimental::CallbackServerContext* /*context*/, const ::FileExchange::DownloadRequest* /*request*/)
    #endif
      { return nullptr; }
  };
  #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
  typedef ExperimentalWithCallbackMethod_Upload<ExperimentalWithCallbackMethod_Download<Service > > CallbackService;
  #endif

  typedef ExperimentalWithCallbackMethod_Upload<ExperimentalWithCallbackMethod_Download<Service > > ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_Upload : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_Upload() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_Upload() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Upload(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::FileExchange::UploadRequest>* /*reader*/, ::FileExchange::UploadResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_Download : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_Download() {
      ::grpc::Service::MarkMethodGeneric(1);
    }
    ~WithGenericMethod_Download() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Download(::grpc::ServerContext* /*context*/, const ::FileExchange::DownloadRequest* /*request*/, ::grpc::ServerWriter< ::FileExchange::DownloadResponse>* /*writer*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_Upload : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_Upload() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_Upload() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Upload(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::FileExchange::UploadRequest>* /*reader*/, ::FileExchange::UploadResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestUpload(::grpc::ServerContext* context, ::grpc::ServerAsyncReader< ::grpc::ByteBuffer, ::grpc::ByteBuffer>* reader, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncClientStreaming(0, context, reader, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawMethod_Download : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_Download() {
      ::grpc::Service::MarkMethodRaw(1);
    }
    ~WithRawMethod_Download() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Download(::grpc::ServerContext* /*context*/, const ::FileExchange::DownloadRequest* /*request*/, ::grpc::ServerWriter< ::FileExchange::DownloadResponse>* /*writer*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestDownload(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncWriter< ::grpc::ByteBuffer>* writer, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncServerStreaming(1, context, request, writer, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class ExperimentalWithRawCallbackMethod_Upload : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    ExperimentalWithRawCallbackMethod_Upload() {
    #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
      ::grpc::Service::
    #else
      ::grpc::Service::experimental().
    #endif
        MarkMethodRawCallback(0,
          new ::grpc::internal::CallbackClientStreamingHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
    #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
                   ::grpc::CallbackServerContext*
    #else
                   ::grpc::experimental::CallbackServerContext*
    #endif
                     context, ::grpc::ByteBuffer* response) { return this->Upload(context, response); }));
    }
    ~ExperimentalWithRawCallbackMethod_Upload() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Upload(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::FileExchange::UploadRequest>* /*reader*/, ::FileExchange::UploadResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
    virtual ::grpc::ServerReadReactor< ::grpc::ByteBuffer>* Upload(
      ::grpc::CallbackServerContext* /*context*/, ::grpc::ByteBuffer* /*response*/)
    #else
    virtual ::grpc::experimental::ServerReadReactor< ::grpc::ByteBuffer>* Upload(
      ::grpc::experimental::CallbackServerContext* /*context*/, ::grpc::ByteBuffer* /*response*/)
    #endif
      { return nullptr; }
  };
  template <class BaseClass>
  class ExperimentalWithRawCallbackMethod_Download : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    ExperimentalWithRawCallbackMethod_Download() {
    #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
      ::grpc::Service::
    #else
      ::grpc::Service::experimental().
    #endif
        MarkMethodRawCallback(1,
          new ::grpc::internal::CallbackServerStreamingHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
    #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
                   ::grpc::CallbackServerContext*
    #else
                   ::grpc::experimental::CallbackServerContext*
    #endif
                     context, const::grpc::ByteBuffer* request) { return this->Download(context, request); }));
    }
    ~ExperimentalWithRawCallbackMethod_Download() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Download(::grpc::ServerContext* /*context*/, const ::FileExchange::DownloadRequest* /*request*/, ::grpc::ServerWriter< ::FileExchange::DownloadResponse>* /*writer*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    #ifdef GRPC_CALLBACK_API_NONEXPERIMENTAL
    virtual ::grpc::ServerWriteReactor< ::grpc::ByteBuffer>* Download(
      ::grpc::CallbackServerContext* /*context*/, const ::grpc::ByteBuffer* /*request*/)
    #else
    virtual ::grpc::experimental::ServerWriteReactor< ::grpc::ByteBuffer>* Download(
      ::grpc::experimental::CallbackServerContext* /*context*/, const ::grpc::ByteBuffer* /*request*/)
    #endif
      { return nullptr; }
  };
  typedef Service StreamedUnaryService;
  template <class BaseClass>
  class WithSplitStreamingMethod_Download : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithSplitStreamingMethod_Download() {
      ::grpc::Service::MarkMethodStreamed(1,
        new ::grpc::internal::SplitServerStreamingHandler<
          ::FileExchange::DownloadRequest, ::FileExchange::DownloadResponse>(
            [this](::grpc::ServerContext* context,
                   ::grpc::ServerSplitStreamer<
                     ::FileExchange::DownloadRequest, ::FileExchange::DownloadResponse>* streamer) {
                       return this->StreamedDownload(context,
                         streamer);
                  }));
    }
    ~WithSplitStreamingMethod_Download() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status Download(::grpc::ServerContext* /*context*/, const ::FileExchange::DownloadRequest* /*request*/, ::grpc::ServerWriter< ::FileExchange::DownloadResponse>* /*writer*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with split streamed
    virtual ::grpc::Status StreamedDownload(::grpc::ServerContext* context, ::grpc::ServerSplitStreamer< ::FileExchange::DownloadRequest,::FileExchange::DownloadResponse>* server_split_streamer) = 0;
  };
  typedef WithSplitStreamingMethod_Download<Service > SplitStreamedService;
  typedef WithSplitStreamingMethod_Download<Service > StreamedService;
};

}  // namespace FileExchange


#endif  // GRPC_FileExchange_2eproto__INCLUDED