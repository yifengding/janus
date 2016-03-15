#ifdef CPU_PROFILE
# include <google/profiler.h>
#endif // ifdef CPU_PROFILE
#include "__dep__.h"
#include "frame.h"
#include "client_worker.h"
#include "command_marshaler.h"
#include "benchmark_control_rpc.h"

using namespace rococo;

#include "server_worker.h"

static ClientControlServiceImpl *ccsi_g = nullptr;
static rrr::PollMgr *cli_poll_mgr_g = nullptr;
static rrr::Server *cli_hb_server_g = nullptr;

static vector<ServerWorker> svr_workers;

void client_setup_heartbeat() {
  std::map<int32_t, std::string> txn_types;
  Frame(MODE_NONE).GetTxnTypes(txn_types);
  unsigned int num_threads = Config::GetConfig()->get_num_threads(); // func
  bool hb = Config::GetConfig()->do_heart_beat();
  if (hb) {
    // setup controller rpc server
    ccsi_g = new ClientControlServiceImpl(num_threads, txn_types);
    int n_io_threads = 1;
    cli_poll_mgr_g = new rrr::PollMgr(n_io_threads);
    base::ThreadPool *thread_pool = new base::ThreadPool(1);
    cli_hb_server_g = new rrr::Server(cli_poll_mgr_g, thread_pool);
    cli_hb_server_g->reg(ccsi_g);
    cli_hb_server_g->start(std::string("0.0.0.0:").append(
        std::to_string(Config::GetConfig()->get_ctrl_port())).c_str());
  }
}

void client_launch_workers(vector<Config::SiteInfo>& client_sites) {
  // load some common configuration
  // start client workers in new threads.
  Log_info("client enabled, number of sites: %d", client_sites.size());
  vector<std::thread> client_threads;
  vector<ClientWorker*> workers;
  for (uint32_t client_id = 0; client_id < client_sites.size(); client_id++) {
    ClientWorker* worker = new ClientWorker(client_id, client_sites[client_id],
                                   Config::GetConfig(), ccsi_g);
    workers.push_back(worker);
    client_threads.push_back(std::thread(&ClientWorker::work,
                                         worker));
  }
  for (auto &th: client_threads) {
    th.join();
  }
  for (auto worker : workers) {
    delete worker;
  }
}

void server_launch_worker(vector<Config::SiteInfo>& server_sites) {
  auto config = Config::GetConfig();
  Log_info("server enabled, number of sites: %d", server_sites.size());
  svr_workers.resize(server_sites.size(), ServerWorker());
  int i=0;
  for (auto& site_info : server_sites) {
    Log_info("launching site: %x, bind address %s",
             site_info.id,
             site_info.GetBindAddress().c_str());
    auto& worker = svr_workers[i++];
    worker.site_info_ = const_cast<Config::SiteInfo*>(&config->SiteById(site_info.id));
    worker.SetupBase();
    // register txn piece logic
    worker.RegPiece();
    // setup communication between controller script
    worker.SetupHeartbeat();
    // populate table according to benchmarks
    worker.PopTable();
    // start server service
    worker.SetupService();
  }

  for (ServerWorker& worker : svr_workers) {
    // start communicator after all servers are running
    worker.SetupCommo();
  }
}

void server_shutdown() {
  for (auto &worker : svr_workers) {
    worker.ShutDown();
  }
}

void check_current_path() {
  auto path = boost::filesystem::current_path();
  Log_info("PWD : ", path.string().c_str());
}

int main(int argc, char *argv[]) {
  check_current_path();

  // read configuration
  int ret = Config::CreateConfig(argc, argv);
  if (ret != SUCCESS) {
    Log_fatal("Read config failed");
    return ret;
  }

  auto server_infos = Config::GetConfig()->GetMyServers();
  if (server_infos.size() > 0) {
    server_launch_worker(server_infos);
  }

  auto client_infos = Config::GetConfig()->GetMyClients();
  if (client_infos.size() > 0) {
    client_setup_heartbeat();
    client_launch_workers(client_infos);
    server_shutdown();
  } else {
    Log_info("No clients running in this process, just sleep and wait.");
    while (1) {
      sleep(1000);
    }
  }

  RandomGenerator::destroy();
  Config::DestroyConfig();

  return 0;
}
