#include <algorithm>

#include <assert.h>
#include <sys/prctl.h>

#include <media/utils/flow.h>
#include <media/utils/utils.h>
#include <media/utils/buffer.h>
#include <media/utils/key_string.h>

namespace libdrm {
class FlowCoroutine {
public:
    FlowCoroutine(Flow *f, Model sync_model, FunctionProcess func, float inter);
    ~FlowCoroutine();

    void Bind(std::vector<int> &in, std::vector<int> &out);

    bool Start();
    void RunOnce();
    int GetCachedBufferCnt();
    bool IsProcessing();
    void ClearCachedBuffers();

private:
    void WhileRun();
    void WhileRunSleep();
    void SyncFetchInput(MediaBufferVector &in);
    void ASyncFetchInputCommon(MediaBufferVector &in);
    void ASyncFetchInputAtomic(MediaBufferVector &in);

    void SendNullBufferDown(Flow::FlowMap &fm, const MediaBufferVector &in, std::list<Flow::FlowInputMap> &flows);
    void SendBufferDown(Flow::FlowMap &fm, const MediaBufferVector &in, std::list<Flow::FlowInputMap> &flows, bool process_ret);
    void SendBufferDownFromDeque(Flow::FlowMap &fm, const MediaBufferVector &in, std::list<Flow::FlowInputMap> &flows, bool process_ret);
    size_t OutputHoldRelated(Flow::FlowMap &fm, std::shared_ptr<MediaBuffer> &out_buffer, const MediaBufferVector &input_vector);

    Flow                                     *flow;
    Model                                    model;
    float                                    interval;
    std::vector<int>                         in_slots;
    std::vector<int>                         out_slots;
    std::thread                              *th;
    FunctionProcess                          th_run;
    bool                                     is_processing;
    bool                                     clear_buffers_enable;
    ConditionLockMutex                       clear_buffers_mtx;

    MediaBufferVector                        in_vector;
    decltype(&FlowCoroutine::SyncFetchInput) fetch_input_func;
    decltype(&FlowCoroutine::SendBufferDown) send_down_func;

public:
    void SetMarkName(std::string s) {
        name = s;
    }

    void SetExpectProcessTime(int time) {
        expect_process_time = time;
    }

    std::string name;
    int         expect_process_time;
};

FlowCoroutine::FlowCoroutine(Flow *f, Model sync_model, FunctionProcess func, float inter)
    : flow(f), model(sync_model), interval(inter), th(nullptr), th_run(func), is_processing(false), clear_buffers_enable(false), expect_process_time(0)
{

}

FlowCoroutine::~FlowCoroutine()
{
    if (th) {
        th->join();
        delete th;
    }

    DRM_MEDIA_LOGD("%s quit", name.c_str());
}

void FlowCoroutine::Bind(std::vector<int> &in, std::vector<int> &out)
{
    assert(in_slots.size() == 0 && out_slots.size() == 0 && "flow coroutine binded");
    in_slots = in;
    out_slots = out;
}

bool FlowCoroutine::Start()
{
    bool need_thread = false;
    auto func = &FlowCoroutine::WhileRun;

    switch (model) {
        case Model::ASYNCCOMMON:
            need_thread = true;
            fetch_input_func = &FlowCoroutine::ASyncFetchInputCommon;
            send_down_func = &FlowCoroutine::SendBufferDownFromDeque;
            break;

        case Model::ASYNCATOMIC:
            need_thread = true;
            fetch_input_func = &FlowCoroutine::ASyncFetchInputAtomic;
            send_down_func = &FlowCoroutine::SendBufferDown;
            func = &FlowCoroutine::WhileRunSleep;
            break;

        case Model::SYNC:
            fetch_input_func = &FlowCoroutine::SyncFetchInput;
            send_down_func = &FlowCoroutine::SendBufferDown;
            break;

        default:
            DRM_MEDIA_LOGE("invalid model %d", (int)model);
            return false;
    }

    in_vector.resize(in_slots.size());
    if (need_thread) {
        th = new std::thread(func, this);
        if (!th) {
            errno = ENOMEM;
            return false;
        }
    }

    return true;
}

static void check_consume_time(const char *name, int expect, int exactly)
{
    if (exactly > expect) {
        DRM_MEDIA_LOGW("%s, expect consume %d ms, however %d ms", name, expect, exactly);
    }
}

void FlowCoroutine::RunOnce()
{
    bool ret = true;
    (this->*fetch_input_func)(in_vector);

    for (unsigned int i = 0; i < in_vector.size(); i++) {
        if (in_vector[i]) {
            std::string node_name = std::string(flow->GetFlowTag()) + ":FetchInput";
            in_vector[i]->TimeStampRecord(node_name, gettimeofday());
        }
    }

    if (flow->GetRunTimesRemaining()) {
        {
            AutoDuration ad;
            is_processing = true;
            ret = (*th_run)(flow, in_vector);
            is_processing = false;

            if (expect_process_time > 0) {
                check_consume_time(name.c_str(), expect_process_time, (int)(ad.Get() / 1000));
            }
        }
    }

    for (int idx : out_slots) {
        auto &fm = flow->downflowmap[idx];
        std::list<Flow::FlowInputMap> flows;

        fm.list_mtx.read_lock();
        flows = fm.flows;
        fm.list_mtx.unlock();
        (this->*send_down_func)(fm, in_vector, flows, ret);
    }

    for (auto &buffer : in_vector) {
        buffer.reset();
    }

    pthread_yield();
}

void FlowCoroutine::WhileRun()
{
    prctl(PR_SET_NAME, this->name.c_str());
    DRM_MEDIA_LOGI("flow-name:[%s]", this->name.c_str());

    while (!flow->quit) {
        RunOnce();
    }
}

void FlowCoroutine::WhileRunSleep()
{
    AutoDuration ad;
    int64_t times = 0;

    assert(interval > 0);

    prctl(PR_SET_NAME, this->name.c_str());
    DRM_MEDIA_LOGI("flow-name:[%s]", this->name.c_str());

    while (!flow->quit) {
        if (times == 0) {
            ad.Reset();
        }

        RunOnce();
        ++times;

        int64_t remain = interval * times - ad.Get() / 1000.0;
        if (remain > 1000 || remain < -1000) {
            times = 0;
            continue;
        }

        if (remain > 0) {
            msleep((int)remain);
        }

        if (times >= 10000) {
            times = 0;
        }
    }
}

void FlowCoroutine::SyncFetchInput(MediaBufferVector &in)
{
    int i = 0;
    for (int idx : in_slots) {
        auto &buffer = flow->v_input[idx].cached_buffer;
        in[i++] = buffer;
        buffer.reset();
    }
}

void FlowCoroutine::ASyncFetchInputCommon(MediaBufferVector &in)
{
    flow->cond_mtx.lock();
    bool empty = true;
    for (size_t i = 0; i < in_slots.size(); i++) {
        int idx = in_slots[i];
        auto &input = flow->v_input[idx];
        auto &v = input.cached_buffers;
        if (!v.empty()) {
            empty = false;
        }
    }

    if (clear_buffers_enable) {
        clear_buffers_mtx.lock();
        clear_buffers_enable = false;
        for (size_t i = 0; i < in_slots.size(); i++) {
            int idx = in_slots[i];
            auto &input = flow->v_input[idx];
            auto &v = input.cached_buffers;
            v.clear();
        }

        clear_buffers_mtx.unlock();
        empty = true;
    }

    if (empty && !flow->quit) {
        flow->cond_mtx.wait();
    }
    flow->cond_mtx.unlock();

    for (size_t i = 0; i < in_slots.size(); i++) {
        int idx = in_slots[i];
        auto &input = flow->v_input[idx];
        auto &v = input.cached_buffers;
        if (v.empty()) {
            continue;
        }

        if (!flow->enable) {
            in.assign(in_slots.size(), nullptr);
            break;
        }

        AutoLockMutex _alm(input.mtx);
        assert(!v.empty());
        in[i] = v.front();
        v.pop_front();
    }
}

void FlowCoroutine::ASyncFetchInputAtomic(MediaBufferVector &in)
{
    int i = 0;
    for (int idx : in_slots) {
        std::shared_ptr<MediaBuffer> buffer;
        auto &input = flow->v_input[idx];
        input.spin_mtx.lock();
        buffer = input.cached_buffer;
        input.spin_mtx.unlock();
        in[i++] = buffer;
    }
}

void FlowCoroutine::SendNullBufferDown(Flow::FlowMap &fm, const MediaBufferVector &in, std::list<Flow::FlowInputMap> &flows)
{
    std::shared_ptr<MediaBuffer> nullbuffer;
    if (fm.hold_input != HoldInputMode::NONE) {
        auto empty_result = std::make_shared<libdrm::MediaBuffer>();
        if (empty_result && OutputHoldRelated(fm, empty_result, in) > 0) {
            nullbuffer = empty_result;
        }
    }

    for (auto &f : flows) {
        f.flow->SendInput(nullbuffer, f.index_of_in);
    }
}

void FlowCoroutine::SendBufferDown(Flow::FlowMap &fm, const MediaBufferVector &in, std::list<Flow::FlowInputMap> &flows, bool process_ret)
{
    if (!process_ret) {
        SendNullBufferDown(fm, in, flows);
        return;
    }

    for (auto &f : flows) {
        auto &input = f.flow->v_input[f.index_of_in];
        if (input.thread_model == Model::SYNC) {
            OutputHoldRelated(fm, fm.cached_buffer, in);
            f.flow->SendInput(fm.cached_buffer, f.index_of_in);
        }
    }

    for (auto &f : flows) {
        auto &input = f.flow->v_input[f.index_of_in];
        if (input.thread_model != Model::SYNC) {
            OutputHoldRelated(fm, fm.cached_buffer, in);
            f.flow->SendInput(fm.cached_buffer, f.index_of_in);
        }
    }

    fm.cached_buffer.reset();
}

void FlowCoroutine::SendBufferDownFromDeque(Flow::FlowMap &fm, const MediaBufferVector &in, std::list<Flow::FlowInputMap> &flows, bool process_ret)
{
    if (!process_ret) {
        SendNullBufferDown(fm, in, flows);
        return;
    }

    if (fm.cached_buffers.empty()) {
        return;
    }

    for (auto &buffer : fm.cached_buffers) {
        OutputHoldRelated(fm, buffer, in);
        for (auto &f : flows) {
            auto &input = f.flow->v_input[f.index_of_in];
            if (input.thread_model == Model::SYNC) {
                f.flow->SendInput(buffer, f.index_of_in);
            }
        }

        for (auto &f : flows) {
            auto &input = f.flow->v_input[f.index_of_in];
            if (input.thread_model != Model::SYNC) {
                f.flow->SendInput(buffer, f.index_of_in);
            }
        }
    }

    fm.cached_buffers.clear();
}

size_t FlowCoroutine::OutputHoldRelated(Flow::FlowMap &fm, std::shared_ptr<MediaBuffer> &out_buffer, const MediaBufferVector &input_vector)
{
    if (fm.hold_input == HoldInputMode::HOLD_INPUT) {
        return FlowOutputHoldInput(out_buffer, input_vector);
    } else if (fm.hold_input == HoldInputMode::INHERIT_FORM_INPUT) {
        return FlowOutputInheritFromInput(out_buffer, input_vector);
    }

    return 0;
}

int FlowCoroutine::GetCachedBufferCnt()
{
    int cnt = 0;
    for (auto inv : in_vector) {
        if (inv) {
            cnt++;
        }
    }

    return cnt;
}

bool FlowCoroutine::IsProcessing()
{
    return is_processing;
}

void FlowCoroutine::ClearCachedBuffers()
{
    clear_buffers_mtx.lock();
    clear_buffers_enable = true;
    clear_buffers_mtx.unlock();
}

DEFINE_REFLECTOR(Flow)

DEFINE_FACTORY_COMMON_PARSE(Flow)

DEFINE_PART_FINAL_EXPOSE_PRODUCT(Flow, Flow)

const FunctionProcess Flow::void_transaction00 = void_transaction<0, 0>;

Flow::Flow()
    : out_slot_num(0), input_slot_num(0), down_flow_num(0),
    waite_down_flow(true), event_handler2_(nullptr), event_callback_(nullptr),
    fps_in(-1), fps_out(-1), fps_cnt(-1), enable(true), quit(false),
    event_handler_(nullptr), play_video_handler_(nullptr),
    play_audio_handler_(nullptr), user_handler_(nullptr),
    user_callback_(nullptr), out_handler_(nullptr), out_callback_(nullptr),
    run_times(-1)
{

}

Flow::~Flow()
{
    StopAllThread();
}

void Flow::StopAllThread()
{
    cond_mtx.lock();
    enable = false;
    quit = true;
    cond_mtx.notify();
    cond_mtx.unlock();

    for (auto &coroutine : coroutines) {
        coroutine.reset();
    }

    coroutines.clear();
}

bool Flow::IsAllBuffEmpty()
{
    int i = 0;

    for (auto &input : v_input) {
        DRM_MEDIA_LOGI("#FLOW v_input-%d cached_buffers size:%zu", i, input.cached_buffers.size());
        DRM_MEDIA_LOGI("#FLOW v_input-%d cached_buffer :%s", i++, input.cached_buffer ? "NotNull" : "Null");
    }

    i = 0;
    for (auto &coroutin : coroutines) {
        DRM_MEDIA_LOGI("#FLOW coroutin-%d in_vector size:%d", i++, coroutin->GetCachedBufferCnt());
    }

    i = 0;
    for (auto &fm : downflowmap) {
        DRM_MEDIA_LOGI("#FLOW downflowmap-%d cached_buffers size:%zu", i, fm.cached_buffers.size());
        DRM_MEDIA_LOGI("#FLOW downflowmap-%d cached_buffer : %s", i++, fm.cached_buffer ? "NotNull" : "Null");
    }

    for (auto &input : v_input) {
        if (input.cached_buffers.size() || input.cached_buffer) {
            return false;
        }
    }

    for (auto &coroutin : coroutines) {
        if (coroutin->GetCachedBufferCnt()) {
            return false;
        }
    }

    for (auto &fm : downflowmap) {
        if (fm.cached_buffers.size() || fm.cached_buffer) {
            return false;
        }
    }

    return true;
}

int Flow::GetCachedBufferNum(unsigned int &total, unsigned int &used)
{
    unsigned int buf_used_cnt = 0;
    unsigned int buf_total_cnt = 0;

    for (auto &input : v_input) {
        if (input.cached_buffers.size() > 0) {
            buf_used_cnt += input.cached_buffers.size();
        } else if (input.cached_buffer) {
            buf_used_cnt += 1;
        }

        buf_total_cnt += (input.max_cache_num + 1);
    }

    for (auto &coroutin : coroutines) {
        if (coroutin->IsProcessing()) {
            buf_used_cnt++;
        }
    }

    total = buf_total_cnt;
    used = buf_used_cnt;

    return 0;
}

void Flow::ClearCachedBuffers()
{
    for (auto &coroutin : coroutines) {
        coroutin->ClearCachedBuffers();
    }
}

void Flow::StartStream()
{
    source_start_cond_mtx->lock();
    waite_down_flow = false;
    source_start_cond_mtx->notify();
    source_start_cond_mtx->unlock();
}

int Flow::SetRunTimes(int _run_times)
{
    run_times = _run_times;
    DRM_MEDIA_LOGI("Flow:%s set run_times to %d", flow_tag.c_str(), run_times);
    return run_times;
}

int Flow::GetRunTimesRemaining()
{
    int remaining_value = run_times;

    if (run_times > 0) {
        run_times--;
    }

    return remaining_value;
}

void Flow::DumpBase(std::string &dump_info)
{
    int idx = 0;
    char str_line[1024] = {0};

    dump_info = "";
    sprintf(str_line, "#Dump Flow(%s) base info:\r\n", GetFlowTag());
    dump_info.append(str_line);

    memset(str_line, 0, sizeof(str_line));
    sprintf(str_line, "  InSlotNum: %d\r\n", input_slot_num);
    dump_info.append(str_line);

    memset(str_line, 0, sizeof(str_line));
    sprintf(str_line, "  OutSlotNum: %d\r\n", out_slot_num);
    dump_info.append(str_line);

    for (auto &input : v_input) {
        memset(str_line, 0, sizeof(str_line));
        sprintf(str_line, "  ->Input[%d] info:\r\n", idx);
        dump_info.append(str_line);

        memset(str_line, 0, sizeof(str_line));
        sprintf(str_line, "    Valid: %s\r\n", input.valid ? "True" : "False");
        dump_info.append(str_line);

        memset(str_line, 0, sizeof(str_line));
        if (input.thread_model == Model::ASYNCCOMMON) {
            sprintf(str_line, "    ThreadMode: ASYNCCOMMON\r\n");
        } else if (input.thread_model == Model::ASYNCATOMIC) {
            sprintf(str_line, "    ThreadMode: ASYNCATOMIC\r\n");
        } else if (input.thread_model == Model::SYNC) {
            sprintf(str_line, "    ThreadMode: SYNC\r\n");
        } else {
            sprintf(str_line, "    ThreadMode: NONE\r\n");
        }
        dump_info.append(str_line);

        memset(str_line, 0, sizeof(str_line));
        if (input.mode_when_full == InputMode::BLOCKING) {
            sprintf(str_line, "    InputMode: BLOCKING\r\n");
        } else if (input.mode_when_full == InputMode::DROPCURRENT) {
            sprintf(str_line, "    InputMode: DROPCURRENT\r\n");
        } else if (input.mode_when_full == InputMode::DROPFRONT) {
            sprintf(str_line, "    InputMode: DROPFRONT\r\n");
        } else {
            sprintf(str_line, "    InputMode: NONE\r\n");
        }
        dump_info.append(str_line);

        memset(str_line, 0, sizeof(str_line));
        sprintf(str_line, "    BufferCnt: current:%zu, max:%d\r\n", input.cached_buffers.size(), input.max_cache_num);
        dump_info.append(str_line);
    }

    idx = 0;
    for (auto &fm : downflowmap) {
        memset(str_line, 0, sizeof(str_line));
        sprintf(str_line, "  ->FlowMap[%d] info:\r\n", idx);
        dump_info.append(str_line);

        memset(str_line, 0, sizeof(str_line));
        sprintf(str_line, "    Valid: %s\r\n", fm.valid ? "True" : "False");
        dump_info.append(str_line);

        memset(str_line, 0, sizeof(str_line));
        sprintf(str_line, "    BufferCnt: %zu\r\n", fm.cached_buffers.size());
        dump_info.append(str_line);

        dump_info.append("    NextFlow: ");
        for (auto &nflow : fm.flows) {
            dump_info.append(nflow.flow->GetFlowTag());
            dump_info.append(" ");
        }
        dump_info.append("\r\n");
    }
}

static bool check_slots(std::vector<int> &slots, const char *debugstr)
{
    if (slots.empty()) {
        return true;
    }

    std::sort(slots.begin(), slots.end());
    auto iend = std::unique(slots.begin(), slots.end());
    if (iend != slots.end()) {
        printf("%s slot duplicate :", debugstr);
        while (iend != slots.end()) {
            printf(" %d", *iend);
            iend++;
        }
        printf("\n");

        return false;
    }

    return true;
}

Flow::FlowMap::FlowMap(FlowMap &&fm)
{
    if (fm.valid) {
        DRM_MEDIA_LOGE("Flow::FlowMap is not copyable and moveable after inited");
        assert(0);
    }
}

void Flow::FlowMap::Init(Model m, HoldInputMode hold_in)
{
    assert(!valid);

    valid = true;
    if (m == Model::ASYNCCOMMON) {
        set_output_behavior = &FlowMap::SetOutputToQueueBehavior;
    } else {
        set_output_behavior = &FlowMap::SetOutputBehavior;
    }
    hold_input = hold_in;
}

void Flow::FlowMap::SetOutputBehavior(const std::shared_ptr<MediaBuffer> &output)
{
    cached_buffer = output;
}

void Flow::FlowMap::SetOutputToQueueBehavior(const std::shared_ptr<MediaBuffer> &output)
{
    cached_buffers.push_back(output);
}

Flow::Input::Input(Input &&in)
{
    if (in.valid) {
        DRM_MEDIA_LOGE("Flow::Input is not copyable and moveable after inited");
        assert(0);
    }
}

void Flow::Input::Init(Flow *f, Model m, int mcn, InputMode im, bool f_block, std::shared_ptr<FlowCoroutine> fc)
{
    assert(!valid);

    valid = true;
    flow = f;
    thread_model = m;
    fetch_block = f_block;
    max_cache_num = mcn;
    mode_when_full = im;

    switch (m) {
        case Model::ASYNCCOMMON:
            send_input_behavior = &Input::ASyncSendInputCommonBehavior;
            break;

        case Model::ASYNCATOMIC:
            send_input_behavior = &Input::ASyncSendInputAtomicBehavior;
            break;

        case Model::SYNC:
            send_input_behavior = &Input::SyncSendInputBehavior;
            coroutine = fc;
            break;

        default:
            break;
    }

    switch (im) {
        case InputMode::BLOCKING:
            async_full_behavior = &Input::ASyncFullBlockingBehavior;
            break;

        case InputMode::DROPFRONT:
            async_full_behavior = &Input::ASyncFullDropFrontBehavior;
            break;

        case InputMode::DROPCURRENT:
            async_full_behavior = &Input::ASyncFullDropCurrentBehavior;
            break;

        default:
            break;
    }
}

bool Flow::SetAsSource(const std::vector<int> &output_slots, FunctionProcess f, const std::string &mark)
{
    source_start_cond_mtx = std::make_shared<ConditionLockMutex>();
    if (!source_start_cond_mtx) {
        return false;
    }

    SlotMap sm;
    sm.input_slots = std::vector<int>{0};
    sm.output_slots = output_slots;
    sm.process = f;
    sm.thread_model = Model::SYNC;
    sm.mode_when_full = InputMode::DROPFRONT;

    if (!InstallSlotMap(sm, mark, 0)) {
        DRM_MEDIA_LOGE("Fail to InstallSlotMap, read mark:[%s]", mark.c_str());
        return false;
    }

    return true;
}

bool Flow::InstallSlotMap(SlotMap &map, const std::string &mark, int exp_process_time)
{
    DRM_MEDIA_LOGI("mark:[%s], thread_model:[%d], mode_when_full:[%d]", mark.c_str(), (int)map.thread_model, (int)map.mode_when_full);

    auto &in_slots = map.input_slots;
    if (in_slots.size() > 1 && map.thread_model == Model::SYNC) {
        DRM_MEDIA_LOGE("More than 1 input to flow, can not set sync input");
        return false;
    }

    if (!check_slots(in_slots, "input")) {
        return false;
    }

    bool ret = true;
    for (int i : in_slots) {
        if (i >= (int)v_input.size()) {
            continue;
        }

        if (v_input[i].valid) {
            DRM_MEDIA_LOGW("input slot %d has been set", i);
            ret = false;
        }
    }

    if (!ret) {
        return false;
    }

    auto &out_slots = map.output_slots;
    if (!check_slots(out_slots, "output")) {
        return false;
    }

    for (int i : out_slots) {
        if (i >= (int)downflowmap.size()) {
            continue;
        }

        if (downflowmap[i].valid) {
            DRM_MEDIA_LOGW("output slot %d has been set", i);
            ret = false;
        }
    }

    if (!ret) {
        return false;
    }

    auto c = std::make_shared<FlowCoroutine>(this, map.thread_model, map.process, map.interval);
    if (!c) {
        errno = ENOMEM;
        return false;
    }

    c->Bind(in_slots, out_slots);
    coroutines.push_back(c);

    if (!in_slots.empty()) {
        int max_idx = in_slots[in_slots.size() - 1];
        if ((int)v_input.size() <= max_idx) {
            v_input.resize(max_idx + 1);
        }

        for (size_t i = 0; i < in_slots.size(); i++) {
            v_input[in_slots[i]].Init(this, map.thread_model,
                (map.thread_model == Model::ASYNCCOMMON) ? map.input_maxcachenum[i] : 0, map.mode_when_full,
                (map.thread_model == Model::ASYNCCOMMON && map.fetch_block.size() > i) ? map.fetch_block[i] : true, c);

            input_slot_num++;
        }
    }

    if (!out_slots.empty()) {
        int max_idx = out_slots[out_slots.size() - 1];
        if ((int)downflowmap.size() <= max_idx) {
            downflowmap.resize(max_idx + 1);
        }

        for (size_t i = 0; i < out_slots.size(); i++) {
            downflowmap[out_slots[i]].Init(map.thread_model, map.hold_input.size() > i ? map.hold_input[i] : HoldInputMode::NONE);
            out_slot_num++;
        }
    }

    c->SetMarkName(mark);
    c->SetExpectProcessTime(exp_process_time);
    c->Start();

    return true;
}

void Flow::FlowMap::AddFlow(std::shared_ptr<Flow> flow, int index)
{
    AutoLockMutex _lg(list_mtx);

    auto i = std::find(flows.begin(), flows.end(), flow);
    if ((i != flows.end()) && (i->index_of_in == index)) {
        DRM_MEDIA_LOGE("repeatedly add, update index");
        i->index_of_in = index;
        return;
    }

    flows.emplace_back(flow, index);
}

void Flow::FlowMap::RemoveFlow(std::shared_ptr<Flow> flow)
{
    AutoLockMutex _lg(list_mtx);
    flows.remove_if([&flow](FlowInputMap &fm) {
        return fm == flow;
    });
}

bool Flow::AddDownFlow(std::shared_ptr<Flow> down, int out_slot_index, int in_slot_index_of_down)
{
    if (out_slot_num <= 0 || (int)downflowmap.size() != out_slot_num) {
        DRM_MEDIA_LOGE("Uncompleted or final flow");
        return false;
    }

    if (out_slot_index >= (int)downflowmap.size()) {
        DRM_MEDIA_LOGE("output slot index exceed max");
        return false;
    }

    if (this == down.get()) {
        DRM_MEDIA_LOGE("can not set self loop flow");
        return false;
    }

    downflowmap[out_slot_index].AddFlow(down, in_slot_index_of_down);
    if (source_start_cond_mtx) {
        source_start_cond_mtx->lock();
        down_flow_num++;
        source_start_cond_mtx->notify();
        source_start_cond_mtx->unlock();
    }

    return true;
}

void Flow::RemoveDownFlow(std::shared_ptr<Flow> down)
{
    if (out_slot_num <= 0 || (int)downflowmap.size() != out_slot_num) {
        return;
    }

    for (auto &dm : downflowmap) {
        if (!dm.valid) {
            continue;
        }

        dm.RemoveFlow(down);
        if (source_start_cond_mtx) {
            source_start_cond_mtx->lock();
            down_flow_num--;
            source_start_cond_mtx->notify();
            source_start_cond_mtx->unlock();
        }
    }
}

void Flow::SendInput(std::shared_ptr<MediaBuffer> &input, int in_slot_index)
{
    int drop_by_fps_ctrl = 0;

    if (in_slot_index < 0 || in_slot_index >= input_slot_num) {
        errno = EINVAL;
        DRM_MEDIA_LOGE("Input slot[%d] is vaild", in_slot_index);
        return;
    }

    if (enable) {
        if (fps_out == 0) {
            DRM_MEDIA_LOGE("%s: %s: drop all buffer(fps_out=0)", GetFlowTag(), __func__);
            return;
        } else if ((fps_in > 0) && (fps_out > 0)) {
            if (fps_cnt < 0) {
                fps_cnt = fps_in - fps_out;
            }

            fps_cnt += fps_out;
            if (fps_cnt < fps_in) {
                drop_by_fps_ctrl = 1;
            } else {
                fps_cnt -= fps_in;
            }
        }

        if (drop_by_fps_ctrl) {
            DRM_MEDIA_LOGE("%s: %s: drop buffer by fps ctrl(%d --> %d)", GetFlowTag(), __func__, fps_in, fps_out);
            return;
        }

        auto &in = v_input[in_slot_index];

        if (input) {
            std::string node_name = std::string(GetFlowTag()) + ":SendInput";
            input->TimeStampRecord(node_name, gettimeofday());
        }

        CALL_MEMBER_FN(in, in.send_input_behavior)(input);
    }
}

bool Flow::SetOutput(const std::shared_ptr<MediaBuffer> &output, int out_slot_index)
{
    if (out_slot_index < 0 || out_slot_index >= out_slot_num) {
        errno = EINVAL;
        DRM_MEDIA_LOGE("Output slot[%d] is vaild", out_slot_index);
        return false;
    }

    std::string node_name = std::string(GetFlowTag()) + ":SetOutput";
    output->TimeStampRecord(node_name, gettimeofday());

    if (out_callback_ && output) {
        out_callback_(out_handler_, output);
    }

    if (enable) {
        auto &out = downflowmap[out_slot_index];
        CALL_MEMBER_FN(out, out.set_output_behavior)(output);
        return true;
    }

    return false;
}

bool Flow::ParseWrapFlowParams(const char *param, std::map<std::string, std::string> &flow_params, std::list<std::string> &sub_param_list)
{
    sub_param_list = ParseFlowParamToList(param);
    if (sub_param_list.empty()) {
        return false;
    }

    if (!parse_media_param_map(sub_param_list.front().c_str(), flow_params)) {
        return false;
    }

    sub_param_list.pop_front();
    if (flow_params[DRM_KEY_NAME].empty()) {
        DRM_MEDIA_LOGE("missing key name");
        return false;
    }

    return true;
}

void Flow::RegisterEventHandler(std::shared_ptr<Flow> flow, EventHook proc)
{
    event_handler_.reset(new EventHandler());
    if (event_handler_) {
        event_handler_->RegisterEventHook(flow, proc);
    }
}

void Flow::UnRegisterEventHandler()
{
    if (event_handler_) {
        event_handler_->UnRegisterEventHook();
        event_handler_.reset(nullptr);
    }
}

void Flow::NotifyToEventHandler(EventParamPtr param, int type)
{
    if (event_handler_) {
        MessagePtr msg = std::make_shared<EventMessage>(this, param, type);
        event_handler_->NotifyToEventHandler(msg);
        event_handler_->SignalEventHook();
    }
}

void Flow::NotifyToEventHandler(int id, int type)
{
    if (event_handler_) {
        EventParamPtr event_param = std::make_shared<EventParam>(id, 0);
        MessagePtr msg = std::make_shared<EventMessage>(this, event_param, type);
        event_handler_->NotifyToEventHandler(msg);
        event_handler_->SignalEventHook();
    }
}

void Flow::EventHookWait()
{
    if (event_handler_) {
        event_handler_->EventHookWait();
    }
}

MessagePtr Flow::GetEventMessage()
{
    if (event_handler_) {
        return event_handler_->GetEventMessage();
    }

    return nullptr;
}

EventParamPtr Flow::GetEventParam(MessagePtr msg)
{
    if (msg) {
        return msg->GetEventParam();
    }

    return nullptr;
}

void Flow::Input::SyncSendInputBehavior(std::shared_ptr<MediaBuffer> &input)
{
    cached_buffer = input;
    coroutine->RunOnce();
    cached_buffer.reset();
}

void Flow::Input::ASyncSendInputCommonBehavior(std::shared_ptr<MediaBuffer> &input)
{
    mtx.lock();
    if (max_cache_num > 0 && max_cache_num <= (int)cached_buffers.size()) {
        bool ret = (this->*async_full_behavior)(flow->enable);
        if (!ret) {
            mtx.unlock();
            return;
        }
    }

    cached_buffers.push_back(input);
    mtx.unlock();

    AutoLockMutex _alm(flow->cond_mtx);
    flow->cond_mtx.notify();
    pthread_yield();
}

void Flow::Input::ASyncSendInputAtomicBehavior(std::shared_ptr<MediaBuffer> &input)
{
    AutoLockMutex _alm(spin_mtx);
    cached_buffer = input;
}

bool Flow::Input::ASyncFullBlockingBehavior(volatile bool &pred)
{
    AutoDuration ad;

    do {
        mtx.unlock();
        msleep(5);
        if (max_cache_num > (int)cached_buffers.size()) {
            mtx.lock();
            break;
        }
        mtx.lock();
    } while (pred);

    if (ad.Get() > 100000) {
        DRM_MEDIA_LOGW("Flow[%s]: Input[block mode]: block too long(%.2fms) > 5ms", flow ? flow->GetFlowTag() : "Name is null", ad.Get() / 1000.0);
    }

    return pred;
}

bool Flow::Input::ASyncFullDropFrontBehavior(volatile bool &pred _DRM_UNUSED)
{
    DRM_MEDIA_LOGW("Flow:[%s]: Input: drop front buffer", flow ? flow->GetFlowTag() : "Name is null");
    cached_buffers.pop_front();
    return true;
}

bool Flow::Input::ASyncFullDropCurrentBehavior(volatile bool &pred _DRM_UNUSED)
{
    DRM_MEDIA_LOGW("Flow:[%s]: Input: drop current buffer", flow ? flow->GetFlowTag() : "Name Is Null");
    return false;
}

std::string gen_datatype_rule(std::map<std::string, std::string> &params)
{
    std::string rule;
    std::string value;

    CHECK_EMPTY_SETERRNO_RETURN(, value, params, DRM_KEY_INPUTDATATYPE, , "");
    PARAM_STRING_APPEND(rule, DRM_KEY_INPUTDATATYPE, value);
    CHECK_EMPTY_SETERRNO_RETURN(, value, params, DRM_KEY_OUTPUTDATATYPE, , "");
    PARAM_STRING_APPEND(rule, DRM_KEY_OUTPUTDATATYPE, value);

    return rule;
}

Model GetModelByString(const std::string &model)
{
    static std::map<std::string, Model> model_map = {
        {DRM_KEY_ASYNCCOMMON, Model::ASYNCCOMMON},
        {DRM_KEY_ASYNCATOMIC, Model::ASYNCATOMIC},
        {DRM_KEY_SYNC,        Model::SYNC}
    };

    auto it = model_map.find(model);
    if (it != model_map.end()) {
        return it->second;
    }

    return Model::NONE;
}

InputMode GetInputModelByString(const std::string &in_model)
{
    static std::map<std::string, InputMode> in_model_map = {
        {DRM_KEY_BLOCKING,    InputMode::BLOCKING},
        {DRM_KEY_DROPFRONT,   InputMode::DROPFRONT},
        {DRM_KEY_DROPCURRENT, InputMode::DROPCURRENT}
    };

    auto it = in_model_map.find(in_model);
    if (it != in_model_map.end()) {
        return it->second;
    }

    return InputMode::NONE;
}

void ParseParamToSlotMap(std::map<std::string, std::string> &params, SlotMap &sm, int &input_maxcachenum)
{
    float fps = 0.0f;
    std::string &fps_str = params[DRM_KEY_FPS];

    if (!fps_str.empty()) {
        fps = std::stof(fps_str);
        if (fps > 0.0f) {
            sm.interval = 1000.0f / fps;
        }
    }

    sm.thread_model = GetModelByString(params[DRM_KEK_THREAD_SYNC_MODEL]);
    sm.mode_when_full = GetInputModelByString(params[DRM_KEK_INPUT_MODEL]);

    int cache_num = -1;
    std::string &cache_num_str = params[DRM_KEY_INPUT_CACHE_NUM];

    if (!cache_num_str.empty()) {
        cache_num = std::stoi(cache_num_str);
        if (cache_num <= 0) {
            DRM_MEDIA_LOGI("input cache num = %d", cache_num);
        }

        input_maxcachenum = cache_num;
    }
}

size_t FlowOutputHoldInput(std::shared_ptr<MediaBuffer> &out_buffer, const MediaBufferVector &input_vector)
{
    assert(out_buffer);

    size_t i = 0;
    for (; i < input_vector.size(); i++) {
        out_buffer->SetRelatedSPtr(input_vector[i], i);
    }

    return i;
}

size_t FlowOutputInheritFromInput(std::shared_ptr<MediaBuffer> &out_buffer, const MediaBufferVector &input_vector)
{
  assert(out_buffer);

    size_t ret = 0;
    auto &vec = out_buffer->GetRelatedSPtrs();

    for (size_t i = 0; i < input_vector.size(); i++) {
        auto &input_vec = input_vector[i]->GetRelatedSPtrs();
        ret += input_vec.size();
        vec.insert(vec.end(), input_vec.begin(), input_vec.end());
    }

    return ret;
}

std::string JoinFlowParam(const std::string &flow_param, size_t num_elem, ...)
{
    std::string ret;
    ret.append(flow_param);

    va_list va;
    va_start(va, num_elem);

    while (num_elem--) {
        const std::string &elem_param = va_arg(va, const std::string);
        ret.append(1, FLOW_PARAM_SEPARATE_CHAR).append(elem_param);
    }
    va_end(va);

    return ret;
}

std::list<std::string> ParseFlowParamToList(const char *param)
{
    std::list<std::string> separate_list;
    if (!parse_media_param_list(param, separate_list, FLOW_PARAM_SEPARATE_CHAR) || separate_list.size() < 2) {
        separate_list.clear();
    }

    return separate_list;
}
}
